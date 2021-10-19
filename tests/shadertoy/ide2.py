#!/usr/bin/python

import sys
sys.path.append("/home/me/Documents/3d-graphics-project/engine/bin")
import engine
import stage
#import main

import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Pango
from gi.repository import Gtk as gtk
from gi.repository import Gdk as gdk
from gi.repository import GObject as gobject
from gi.repository import GtkSource as gtksource
from gi.repository.GdkPixbuf import Pixbuf
from gi.repository import GtkSource
from gi.repository import GObject

import importlib
import re
import signal
import gc,os,stat
import threading
import cairo

WIDTH = 800
HEIGHT = 600

PROJECT_DIR = os.path.dirname(sys.argv[1])

class SearchDialog(gtk.Dialog):
    def __init__(self, parent):
        gtk.Dialog.__init__(
            self, title="Search", transient_for=parent, modal=True,
        )
        self.add_buttons(
            gtk.STOCK_FIND,
            gtk.ResponseType.OK,
            gtk.STOCK_CANCEL,
            gtk.ResponseType.CANCEL,
        )

        box = self.get_content_area()

        label = gtk.Label(label="Insert text you want to search for:")
        box.add(label)

        self.entry = gtk.Entry()
        box.add(self.entry)


        self.show_all()


class CustomCompletionProvider(GObject.GObject, gtksource.CompletionProvider):
	"""
	This is a custom Completion Provider
	In this instance, it will do 2 things;
	1) always provide Hello World! (Not ideal but an option so its in the example)
	2) Utilizes the Gtk.TextIter from the TextBuffer to determine if there is a jinja
	example of '{{ custom.' if so it will provide you with the options of foo and bar.
	if select it will insert foo }} or bar }}, completing your syntax
	PLEASE NOTE the GtkTextIter Logic and regex are really rough and should be adjusted and tuned
	to fit your requirements
	# Implement the Completion Provider
	# http://stackoverflow.com/questions/32611820/implementing-gobject-interfaces-in-python
	# https://gist.github.com/andialbrecht/4463278 (Python example implementing TreeModel)
	# https://developer.gnome.org/gtk3/stable/GtkTreeModel.html (Gtk TreeModel interface specification)
	# A special thank you to @zeroSteiner
	"""

	# apparently interface methods MUST be prefixed with do_
	def do_get_name(self):
		return 'Custom'

	def do_match(self, context):
		# this should evaluate the context to determine if this completion
		# provider is applicable, for debugging always return True
		return True

	def do_populate(self, context):
		proposals = [
			gtksource.CompletionItem(label='Hello World!', text='Hello World!', icon=None, info=None)  # always proposed
		]

		# found difference in Gtk Versions
		end_iter = context.get_iter()
		if not isinstance(end_iter, gtk.TextIter):
			_, end_iter = context.get_iter()

		if end_iter:
			buf = end_iter.get_buffer()
			mov_iter = end_iter.copy()
			if mov_iter.backward_search('{{', gtk.TextSearchFlags.VISIBLE_ONLY):
				mov_iter, _ = mov_iter.backward_search('{{', gtk.TextSearchFlags.VISIBLE_ONLY)
				left_text = buf.get_text(mov_iter, end_iter, True)
			else:
				left_text = ''

			if re.match(r'.*\{\{\s*custom\.$', left_text):
				proposals.append(
					gtksource.CompletionItem(label='foo', text='foo }}')  # optionally proposed based on left search via regex
				)
				proposals.append(
					gtksource.CompletionItem(label='bar', text='bar }}')  # optionally proposed based on left search via regex
				)

		context.add_proposals(self, proposals, True)
		return

class Editor:

	def __init__(self,project_path, tabs=[]):

		# renders windows out of focus
		signal.signal(signal.SIGINT, self.signal_handler)

		self.window = gtk.Window(gtk.WindowType.TOPLEVEL)
		self.theme = "dark"    # [light/dark] type of gtk theme in use
		self.syntax_theme = "oblivion"
		self.window.connect("delete_event", self.delete_event_cb)
		self.window.connect("destroy", self.destroy_cb)
		self.window.connect("draw", self.on_window_draw)
		self.window.set_default_size(1000, 600);
		#self.window.maximize()
		self.sub_frame = gtk.VBox()
		self.main_frame = gtk.Grid(orientation=gtk.Orientation.VERTICAL)



		self.area = stage.DrawArea(600, 500)
		self.area.set_can_focus(True)

		mb = gtk.MenuBar()

		filemenu = gtk.Menu()
		filem = gtk.MenuItem("File")
		filem.set_submenu(filemenu)

		new_file = gtk.MenuItem("New")
		new_file.connect("activate", self.new_file_creator)

		filemenu.append(new_file)


		open_file = gtk.MenuItem("Open")
		open_file.connect("activate", self.open_file_chooser)

		filemenu.append(open_file)

		exit = gtk.MenuItem("Exit")
		exit.connect("activate", gtk.main_quit)

		filemenu.append(exit)

		mb.append(filem)

		editmenu = gtk.Menu()
		editm = gtk.MenuItem("Edit")
		editm.set_submenu(editmenu)

		mb.append(editm)

		viewmenu = gtk.Menu()
		viewm = gtk.MenuItem("View")
		viewm.set_submenu(viewmenu)

		theme_item = gtk.MenuItem("Toggle Night Mode")
		theme_item.connect("activate", self.set_theme)

		viewmenu.append(theme_item)

		mb.append(viewm)

		self.sub_frame.add(mb)

		toolbar = gtk.Toolbar()
		self.main_frame.add(toolbar)
		play_button = gtk.ToolButton()
		play_button.set_icon_name("gtk-media-play")
		build_button = gtk.ToolButton()
		build_button.set_icon_name("gtk-refresh")
		self.status_bar = gtk.Statusbar()
		self.id1 = self.status_bar.get_context_id("Statusbar")
		play_button.connect("clicked", self.on_play, self.id1)
		self.id2 = self.status_bar.get_context_id("Statusbar")
		build_button.connect("clicked", self.on_rebuild, self.id2)
		toolbar.insert(play_button, 0)
		toolbar.insert(build_button, 1)
		self.vertical_panel = gtk.HPaned()
		self.vertical_panel2 = gtk.HPaned()
		self.horizontal_panel = gtk.VPaned()
		self.horizontal_panel.set_border_width(5)
		self.vertical_panel.set_border_width(5)
		self.vertical_panel2.set_border_width(5)

		self.console_view = gtk.TextView()
		self.console_view.get_buffer().set_text(">_", 2)
		self.console_view.set_cursor_visible(False)
		self.console_view.set_border_width(5)
		self.console_view.modify_font(Pango.FontDescription('monospace 11'))
		sw = gtk.ScrolledWindow()
		sw.set_border_width(5)
		sw.add(self.console_view)
		self.vertical_panel.set_position(0)
		self.area_notebook = gtk.Notebook()
		self.area_notebook.set_group_name('0') # very important for DND
		tab_label = gtk.Label("Output")
		self.area_notebook.append_page(self.area, tab_label)
		self.area_notebook.set_tab_detachable(self.area, True)
		self.area_notebook.connect('create-window', self.notebook_create_window)
		self.horizontal_panel.pack1(self.area_notebook, True, False)
		self.horizontal_panel.pack2(sw, True, False)
		self.horizontal_panel.set_position(200)
		self.vertical_panel2.pack1(self.vertical_panel, True, False)
		self.vertical_panel2.pack2(self.horizontal_panel, True, False)
		self.vertical_panel2.set_position(1000)

		self.notebook = gtk.Notebook()
		self.notebook.set_border_width(5)
		
		for tab in tabs:	
			self.new_tab(tab)

		self.vertical_panel.pack2(self.notebook, True, False)
		self.main_frame.add(self.vertical_panel2)


		self.window_frame = gtk.Box(orientation=gtk.Orientation.VERTICAL)
		self.window_frame.add(self.sub_frame)
		self.window_frame.add(self.main_frame)
		self.window_frame.add(self.status_bar)

		self.area.set_hexpand(True)
		self.area.set_vexpand(True)
		self.sub_frame.set_hexpand(False)
		self.sub_frame.set_vexpand(False)
		self.main_frame.set_hexpand(True)
		self.main_frame.set_vexpand(True)
		self.window_frame.set_hexpand(True)
		self.window_frame.set_vexpand(True)
		self.window.add(self.window_frame)

		self.area.set_events(gdk.EventMask.ALL_EVENTS_MASK)
		#self.area.add_events(gdk.EventMask.BUTTON_PRESS_MASK)
		self.area.connect("key-press-event", self.on_window_key_press, None)
		self.area.connect("button-press-event", self.on_window_clicked, None)
		self.area.connect("focus-in-event", self.focus_in)
		self.window.add_events(gdk.EventMask.ALL_EVENTS_MASK)
		self.window.connect("set-focus", self.set_focus)

		self.set_theme(self.theme)
		self.window.show_all()
		self.check_sigint_timer(1)
		gtk.main()


	def quit_activated(self):
		dialog = gtk.MessageDialog(parent=self.window, type=gtk.MessageType.QUESTION, buttons=gtk.ButtonsType.YES_NO)   
		dialog.set_title("Question")
		dialog.set_position(gtk.WindowPosition.CENTER_ALWAYS)
		dialog.set_markup("Are you sure you want to quit?")
		response = dialog.run()
		if response == gtk.ResponseType.YES:
			dialog.destroy()
			gtk.main_quit()
		elif response == gtk.ResponseType.NO:
			dialog.destroy()

	def delete_event_cb(self, widget, data=None):
		print("delete_event signal occurred")
		self.quit_activated()
		return True

	def destroy_cb(self, widget, data=None):
		print("destroy signal occurred")
		self.quit_activated()

	def signal_handler(self, signal, frame):
		print('\nYou pressed Ctrl+C!, exiting')
		# gtk.main_quit()
		engine.terminate()
		sys.quit()
		# gtk.Window.present(self.window)
		# self.window.grab_focus()


	def dirwalk(self, path, parent=None):
		# Iterate over the contents of the specified path
		for f in os.listdir(path):
			# Get the absolute path of the item
			fullname = os.path.join(path, f)
			# Extract metadata from the item
			fdata = os.stat(fullname)
			# Determine if the item is a folder
			is_folder = stat.S_ISDIR(fdata.st_mode)
			# Generate an icon from the default icon theme
			img = gtk.IconTheme.get_default().load_icon(
			"folder" if is_folder else "document",
			gtk.IconSize.MENU, 0)
			# Append the item to the TreeStore
			# If the item is a folder, descend into it
			if is_folder: self.dirwalk(fullname, li)

	def on_open(self, widget, tag):
		self.save_open_files()
		PROJECT_DIR = self.selected_file
		widget.destroy()
		self.window.show_all()

	def on_new_file(self, widget, tag):
		self.create_file(self.selected_file)
		self.save_open_files()
		widget.destroy()
		self.window.show_all()
	


	def save_open_files(self):
		for n in range(self.notebook.get_n_pages()):
			page = self.notebook.get_nth_page(n)
			self.view = page.get_children()[0]
			buffer = self.view.get_buffer()
			contents = buffer.get_text(buffer.get_start_iter(), buffer.get_end_iter(), True)
			tab_name = self.notebook.get_tab_label(page).get_children()[0].get_text()
			if (tab_name.endswith(".py")):
				self.create_file(PROJECT_DIR + "/" + tab_name, contents)
			if (tab_name.endswith((".fs", ".vs"))):
				self.create_file(PROJECT_DIR + "/data/" + tab_name, contents)
		self.status_bar.push(self.id1, "Saved.")

	def on_rebuild(self, widget, data):
		self.save_open_files()

		self.delete_drawarea()

		self.area = stage.DrawArea(600, 500)
		self.area.set_hexpand(True)
		self.area.set_vexpand(True)
		self.area.set_can_focus(True)
		self.area.set_events(gdk.EventMask.ALL_EVENTS_MASK)
		self.area.connect("key-press-event", self.on_window_key_press, None)
		self.area.connect("button-press-event", self.on_window_clicked, None)
		self.add_drawarea()
		self.window.show_all()
		self.status_bar.push(data, "Finished.")

	def show_modal_window(self):
		import random
		del sys.modules["main"]
		import main
		importlib.reload(main)
		stage.app = main.Application("app", WIDTH, HEIGHT, False)
		stage.app.init("app", WIDTH, HEIGHT, False)
		engine.run(stage.app)
		#updater_thread = threading.Thread(target=self.update_console)
		#updater_thread.start()

	def new_file_creator(self, widget):
		self.file_chooser = gtk.FileChooserDialog("Please choose a filename",self.window, gtk.FileChooserAction.SAVE,
           (gtk.STOCK_CANCEL, gtk.ResponseType.CANCEL,
            gtk.STOCK_OPEN, gtk.ResponseType.OK))

		file_filter = gtk.FileFilter()
		file_filter.add_pattern("*.py")
		self.file_chooser.set_filter(file_filter)
		self.file_chooser.connect("selection-changed", self.on_selection_changed2)
		self.file_chooser.connect("response", self.on_new_file)
		self.file_chooser.show_all()

	def open_file_chooser(self, fc):
		self.file_chooser = gtk.FileChooserDialog("Please choose a file", self.window, gtk.FileChooserAction.OPEN,
           (gtk.STOCK_CANCEL, gtk.ResponseType.CANCEL,
            gtk.STOCK_OPEN, gtk.ResponseType.OK))
		file_filter = gtk.FileFilter()
		file_filter.add_pattern("*.py")
		self.file_chooser.set_filter(file_filter)
		self.file_chooser.connect("selection-changed", self.on_selection_changed)
		self.file_chooser.connect("response", self.on_open)
		self.file_chooser.show_all()
		response = self.file_chooser.run()
		if response == gtk.ResponseType.OK:
			print ("OK")
		elif response == gtk.ResponseType.CANCEL:
			print ("CANCEL")

	def set_syntax_theme(self, buffer=None):
		manager = gtksource.StyleSchemeManager().get_default()
		#print (manager.get_scheme_ids())
		scheme = manager.get_scheme(self.syntax_theme)
		if (buffer):
			manager = gtksource.StyleSchemeManager().get_default()
			scheme = manager.get_scheme(self.syntax_theme)
			buffer.set_style_scheme(scheme)
		else:
			# no args; change theme for all open tabs
			for n in range(self.notebook.get_n_pages()):
				page = self.notebook.get_nth_page(n)
				tab_name = self.notebook.get_tab_label(page).get_children()[0].get_text()
				self.sourceview = page.get_children()[0]
				buffer = self.sourceview.get_buffer()
				buffer.set_style_scheme(scheme)

	# def on_search_clicked(self, widget):
	# 	dialog = SearchDialog(self.window)
	# 	response = dialog.run()
	# 	if response == gtk.ResponseType.OK:
	# 		cursor_mark = widget.get_buffer().get_insert()
	# 		start = widget.get_buffer().get_iter_at_mark(cursor_mark)
	# 		if start.get_offset() == widget.get_buffer().get_char_count():
	# 			start = widget.get_buffer().get_start_iter()
	# 		self.search_and_mark(dialog.entry.get_text(), start)
	# 	dialog.destroy()


	def on_search_clicked(self, widget):
		current_sourceview = self.notebook.get_nth_page(self.notebook.get_current_page()).get_children()[0]
		buf = current_sourceview.get_buffer()
		dialog = SearchDialog(self.window)
		response = dialog.run()
		if response == gtk.ResponseType.OK:
			cursor_mark = buf.get_insert()
			start = buf.get_iter_at_mark(cursor_mark)
			if start.get_offset() == buf.get_char_count():
				start = buf.get_start_iter()

			self.search_and_mark(dialog.entry.get_text(), start)

		dialog.destroy()

	def search_and_mark(self, text, start):
		current_sourceview = self.notebook.get_nth_page(self.notebook.get_current_page()).get_children()[0]
		buf = current_sourceview.get_buffer()
		end = buf.get_end_iter()
		match = start.forward_search(text, 0, end)

		if match is not None:
			match_start, match_end = match
			buf.apply_tag(self.tag_found, match_start, match_end)
			self.search_and_mark(text, match_end)

	def sourceview_on_key_press(self, sourceview, event, user_param):
		ctrl = (event.state & gdk.ModifierType.CONTROL_MASK)
		if ctrl and event.keyval == gdk.KEY_s:
			self.save_open_files()

		if ctrl and event.keyval == gdk.KEY_f:
			self.on_search_clicked(self.sourceview)

		if event.keyval == gdk.KEY_Escape:
			self.remove_tags()
	def remove_tags(self):
		current_sourceview = self.notebook.get_nth_page(self.notebook.get_current_page()).get_children()[0]
		buf = current_sourceview.get_buffer()
		buf.remove_tag(self.tag_found, buf.get_start_iter(), buf.get_end_iter())


	def destroy_modal_window(self, widget):
		widget.remove(self.area)
		self.add_drawarea()
		widget.destroy()

	def on_window_key_press(self, widget, event, data):
		if (widget == self.area):
			modal_drawarea_window = None

		if event.keyval == gdk.KEY_Escape:
			modal_drawarea_window.destroy()
			#self.add_drawarea()

	def new_tab(self, title):

		# switch to tab if tab is already open
		for n in range(self.notebook.get_n_pages()):
			page = self.notebook.get_nth_page(n)
			tab_name = self.notebook.get_tab_label(page).get_children()[0].get_text()
			if title == tab_name:
				self.notebook.set_current_page(n)
				return False

		# create tab textview
		self.sourceview = gtksource.View()
		self.sourceview.set_tab_width(2)

		#self.window.connect("destroy", Gtk.main_quit)

		self.sourceview.connect("button-press-event",self.on_window_clicked, None)
		self.sourceview.connect("key-press-event",self.sourceview_on_key_press, None)
		self.sourceview.set_events(gdk.EventMask.ALL_EVENTS_MASK)
		self.sourceview.modify_font(Pango.FontDescription('monospace 11'))
		self.sourceview.set_show_line_numbers(True)
		self.sourceview.set_show_line_marks(True)
		self.sourceview.set_indent_width(2)
		buffer = self.sourceview.get_buffer()
		#self.textbuff = gtksource.Buffer()
		#self.sourceview.set_buffer(self.textbuff)
		if (title.endswith(".py")):
			txt = open(PROJECT_DIR + "/" + title).read()
		elif (title.endswith((".vs", ".fs"))):
			txt = open(PROJECT_DIR + "/data/" + title).read()
		else:
			txt = open(PROJECT_DIR + "/" + title).read()
		buffer.set_text(txt)
		# set language, syntax highlighting
		lm = gtksource.LanguageManager.new()
		lang = lm.guess_language(title)
		buffer.set_highlight_syntax(True)
		buffer.set_language(lang)
		buffer.create_tag("invisible",invisible=True)
		self.tag_found = buffer.create_tag("found", background="gray")

		self.set_syntax_theme(buffer)

		self.textbuff = self.sourceview.get_buffer()
		#self.sourceview.set_buffer(self.textbuff)
		self.keywords = """
				GtkSourceView
				Completion
			"""
		self.set_auto_completion()

		self.window.show_all()

		self.check_sigint_timer(1)

		self.sourceview.set_show_line_numbers(True)
		self.sourceview.set_show_line_marks(True)
		sw = gtk.ScrolledWindow()
		sw.set_border_width(5)
		sw.add(self.sourceview)
		self.page1 = sw
		self.page1.set_border_width(5)

		# create notebook tab handle
		header = gtk.HBox()
		title_label = gtk.Label(label=title)
		image = gtk.Image()
		image.set_from_stock(gtk.STOCK_CLOSE, gtk.IconSize.MENU)
		close_button = gtk.Button()
		close_button.set_image(image)
		close_button.set_relief(gtk.ReliefStyle.NONE)
		close_button.connect("clicked", self.on_tab_close)
		header.pack_start(title_label,
						expand=True, fill=True, padding=0)
		header.pack_end(close_button,
						expand=False, fill=False, padding=0)
		header.show_all()

		self.notebook.insert_page(self.page1, header, 0)
		self.notebook.child_set_property(self.page1, 'reorderable', True)
		self.notebook.show_all()
		self.notebook.set_current_page(0)

		return True



	def notebook_create_window (self, notebook, widget, x, y):
		# handler for dropping outside of current window
		window = gtk.Window()
		new_notebook = gtk.Notebook()
		window.add(new_notebook)
		new_notebook.set_group_name('0') # very important for DND
		new_notebook.connect('page-removed', self.notebook_page_removed, window)
		window.connect('destroy', self.sub_window_destroyed, new_notebook, notebook)
		window.set_transient_for(self.window)
		window.set_destroy_with_parent(True)
		window.set_size_request(400, 400)
		window.move(x, y)
		window.show_all()
		return new_notebook




	def sub_window_destroyed (self, window, cur_notebook, dest_notebook):
		# if the sub window gets destroyed, push pages back to the main window
		# detach the notebook pages in reverse sequence to avoid index errors
		for page_num in reversed(range(cur_notebook.get_n_pages())):
			widget = cur_notebook.get_nth_page(page_num)
			tab_label = cur_notebook.get_tab_label(widget)
			cur_notebook.detach_tab(widget)
			dest_notebook.append_page(widget, tab_label)
			dest_notebook.set_tab_detachable(widget, True)

	def set_auto_completion(self):
		"""
		1)
		Set up a provider that get words from what has already been entered
		in the gtkSource.Buffer that is tied to the GtkSourceView
		2)
		Set up a second buffer that stores the keywords we want to be available
		3)
		Setup an instance of our custome completion class to handle special characters with
		auto complete.
		"""
		# This gets the GtkSourceView completion that's already tied to the GtkSourceView
		# We need it to attached our providers to it
		self.view_completion = self.sourceview.get_completion()

		# 1) Make a new provider, attach it to the main buffer add to view_autocomplete
		self.view_autocomplete = gtksource.CompletionWords.new('testing')
		self.view_autocomplete.register(self.sourceview.get_buffer())
		self.view_completion.add_provider(self.view_autocomplete)

		# 2) Make a new buffer, add a str to it, make a provider, add it to the view_autocomplete
		self.keybuff = gtksource.Buffer()
		self.keybuff.begin_not_undoable_action()
		self.keybuff.set_text(self.keywords)
		self.keybuff.end_not_undoable_action()
		self.view_keyword_complete = gtksource.CompletionWords.new('keyword')
		self.view_keyword_complete.register(self.keybuff)
		self.view_completion.add_provider(self.view_keyword_complete)
		
		# 3) Set up our custom provider for syntax completion.
		custom_completion_provider = CustomCompletionProvider()
		self.view_completion.add_provider(custom_completion_provider)
		self.custom_completion_provider = custom_completion_provider
		return

	def set_theme(self, t):

		settings = gtk.Settings.get_default()
		if (self.theme == "dark"):

			settings.set_property("gtk-application-prefer-dark-theme", True)  # if you want use dark theme, set second arg to True
			self.theme = "light"
			self.syntax_theme = "oblivion"
			self.set_syntax_theme()
		else:
			settings.set_property("gtk-application-prefer-dark-theme", False)  # if you want use dark theme, set second arg to True
			self.theme = "dark"
			self.syntax_theme = "classic"
			self.set_syntax_theme()

	def check_sigint_timer(self,timeout):
		gobject.timeout_add_seconds(timeout, self.check_sigint)

	def check_sigint(self):
		return True
	def on_selection_changed(self, button):
		self.selected_file = button.get_current_folder()

	def on_selection_changed2(self, button):
		self.selected_file = button.get_filename()

	def create_file(self, filename, contents=""):
		f = open(filename, "w")
		f.write(contents)
		f.close()

	def remove_drawarea(self):
		self.horizontal_panel.remove(self.area)

	def delete_drawarea(self):
		self.horizontal_panel.get_child1().destroy()

	def add_drawarea(self):
		self.area.set_hexpand(True)
		self.area.set_vexpand(True)
		self.horizontal_panel.pack1(self.area, True, False)

	def on_play(self, widget, data):
		#modal_window = threading.Thread(target=self.show_modal_window)
		#modal_window.start()
		os.system("python3 " + PROJECT_DIR + "/main.py")

	def on_activated(self, widget, row, col):
		model = widget.get_model()
		text = model[row][0]
		# try creating a new tab
		self.new_tab(text)
		self.window.show_all()

	def on_tab_close(self, button):
		self.notebook.remove_page(self.notebook.get_current_page())
	def update(self, widget, clock, data):
		self.update_console()
		return 1

	def update_console(self):
		pass
		#self.console_view.get_buffer().set_text(stage.app.catchOutErr.value, len(stage.app.catchOutErr.value))

	def can_focus(self, widget):
		pass

	def set_focus(self, window, widget):
		print (widget, widget.has_focus())

	def on_window_clicked(self, widget, event, user_param):
		widget.grab_focus()
		if (event.type == gdk.EventType.DOUBLE_BUTTON_PRESS):
			buffer = widget.get_buffer()
			selected = buffer.get_text(buffer.get_iter_at_mark(buffer.get_insert()), buffer.get_iter_at_mark(buffer.get_selection_bound()), True)
			self.search_and_mark(selected, buffer.get_start_iter())
		else:
			self.remove_tags()
	def focus_in(self, widget, data):
		print ("focus")

	def on_window_draw(self, window, cr):
		widget = window.get_focus()
		self.update_console()
		# if (widget.has_focus()):
		# 	print ("focused on", widget)
		# 	gtk.render_focus(widget.get_style_context(),
		# 					cr,
		# 					3, 3,
		# 					400, 400)
		# 	widget.show_all()
	def notebook_page_removed (self, notebook, child, page, window):
		#destroy the sub window after the notebook is empty
		if notebook.get_n_pages() == 0:
			window.destroy()

if __name__ == "__main__":
		ed = Editor(PROJECT_DIR, ["screen_shader.fs"])
