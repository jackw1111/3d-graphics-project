#!/usr/bin/python

import sys,os
sys.path.append("/home/me/Documents/3d-graphics-project/engine/bin")
sys.path.append("/home/me/Documents/3d-graphics-project/engine/utils")
from engine.graphics import *
from OpenGL.GL import *
from keys import *
from grid import *

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

import math
import random
import time

WIDTH = 420
HEIGHT = 236

import png

def create_png(color):

    width = 255
    height = 255
    img = []
    for y in range(height):
        row = ()
        for x in range(width):
            row = row + (int(color.x * 255), int(color.y * 255), int(color.z * 255), 255)
        img.append(row)
    with open('./data/gradient.png', 'wb') as f:
        w = png.Writer(width, height, greyscale=False, alpha='RGBA')
        w.write(f, img)



def convert_vertices(vertices):
    converted = []
    for v in vertices:
        s = "v " + str(v.x) + " " + str(v.y) + " " + str(v.z) + "\n"
        converted += s
    return "".join(converted)

def convert_faces(vertices, indices=[]):

    converted = []
    i = 0
    for f in range(int(len(vertices)/3)):
        print (int(len(vertices)/3))
        s = "f "
        if (len(indices)):
            for j in range(3):
                i+=1
                s += str(indices[i]) + "/1/" + str(indices[i])
                if (j == 2):
                    s += '\n'
                else:
                    s += ' '
        else:      
            for j in range(3):
                i += 1
                s += str(i) + "/1/" + str(i)
                if (j == 2):
                    s += "\n"
                else:
                    s += " "

        converted += s

    return "".join(converted)



def create_obj_file(vertices, filename, indices=[]):
    contents = """
# Blender v2.79 (sub 0) OBJ File: ''
# www.blender.org
mtllib mesh.mtl
""" + convert_vertices(vertices) + """
vt 0.999900 0.000100
""" + convert_normals(vertices) + """
usemtl None
s off
""" + convert_faces(vertices, indices)


    with open(filename, 'w') as f:
        f.write(contents)

def convert_normals(vertices, reversed_normals=False):
    normals = []
    for f in range(0, int(len(vertices)/3), 3):
        for j in range(3):
            n = "vn "
            p1, p2, p3 = vertices[f*3], vertices[f*3 + 1], vertices[f*3 + 2]
            norm = cross(p2 - p1, p3 - p1)
            n += str(norm.x)
            n += " "
            n += str(norm.y)
            n += " "
            n += str(norm.z)
            n += '\n'
            normals.append(n)
    return "".join(normals)

def mesh_method1(equation, detail=2.5, xdomain=[-2,2], ydomain=[-2,2], reversed_normals=False):
    mesh = []
    detail = 1.0/detail
    for i in frange(xdomain[0], xdomain[1], detail):
        for j in frange(ydomain[0], ydomain[1], detail):

                x = i
                y = j
                try:
                    p1 = vec3(x, eval(equation), y)
                except ValueError:
                    return []
                x = i+detail
                y = j
                try:
                    p2 = vec3(x, eval(equation), y)
                except ValueError:
                    return []
                x = i
                y = j+detail
                try:
                    p3 = vec3(x, eval(equation), y)
                except ValueError:
                    return []
                x = i+detail
                y = j+detail
                try:
                    p4 = vec3(x, eval(equation), y)
                except ValueError:
                    return []

                if (reversed_normals):
                    mesh.append(p1)
                    mesh.append(p2)
                    mesh.append(p3)
                    mesh.append(p2)
                    mesh.append(p4)
                    mesh.append(p3)
                else:
                    mesh.append(p1)
                    mesh.append(p3)
                    mesh.append(p2)
                    mesh.append(p2)
                    mesh.append(p3)
                    mesh.append(p4)
    return mesh

def map(value, in_start, in_end, out_start, out_end):
    m = (out_end - out_start) / (in_end - in_start)
    return out_start + m * value

def mesh_method2(equation, detail=2.5, xdomain=[-2,2], ydomain=[-2,2], reversed_normals=False):
    layers             = 100
    circumferenceTiles = 100
    sphereVertices = []
    mesh = []

    for i in range(layers+1):
        for j in range(circumferenceTiles+1):

            x = i
            y = j

            lon     = map(x, 0, layers, -math.pi, math.pi)
            lon_sin = math.sin( lon )
            lon_cos = math.cos( lon )

            lat     = map(y, 0, circumferenceTiles, -math.pi/2, math.pi/2)
            lat_sin = math.sin( lat)
            lat_cos = math.cos( lat)
            p1 = vec3(lon_cos * lat_cos, lon_sin, lon_cos * lat_sin )

            x = i+1
            y = j    

            lon     = map(x, 0, layers, -math.pi, math.pi)
            lon_sin = math.sin( lon )
            lon_cos = math.cos( lon )

            lat     = map(y, 0, circumferenceTiles, -math.pi/2, math.pi/2)
            lat_sin = math.sin( lat)
            lat_cos = math.cos( lat)
            p2 = vec3(lon_cos * lat_cos, lon_sin, lon_cos * lat_sin )

            x = i
            y = j+1

            lon     = map(x, 0, layers, -math.pi, math.pi)
            lon_sin = math.sin( lon )
            lon_cos = math.cos( lon )

            lat     = map(y, 0, circumferenceTiles, -math.pi/2, math.pi/2)
            lat_sin = math.sin( lat)
            lat_cos = math.cos( lat)
            p3 = vec3(lon_cos * lat_cos, lon_sin, lon_cos * lat_sin )

            x = i+1
            y = j+1

            lon     = map(x, 0, layers, -math.pi, math.pi)
            lon_sin = math.sin( lon )
            lon_cos = math.cos( lon )

            lat     = map(y, 0, circumferenceTiles, -math.pi/2, math.pi/2)
            lat_sin = math.sin( lat)
            lat_cos = math.cos( lat)
            p4 = vec3(lon_cos * lat_cos, lon_sin, lon_cos * lat_sin )


            mesh.append(p1)
            mesh.append(p3)
            mesh.append(p2)
            mesh.append(p2)
            mesh.append(p3)
            mesh.append(p4)

    return mesh
    

def create_mesh(equation, color=vec3(-1,-1,-1), detail=2.5, xdomain=[-2,2], ydomain=[-2,2], reversed_normals=False):
   
    # create a random color
    if (color == vec3(-1,-1,-1)):
        v1,v2,v3 = random.uniform(0,1),random.uniform(0,1),random.uniform(0,1)
        color = vec3(v1,v2,v3)

    create_png(color)


    mesh = mesh_method1(equation, detail, xdomain, ydomain, reversed_normals)
    indices = []
    if (mesh == []):
        mesh = mesh_method2(equation, detail, xdomain, ydomain, reversed_normals)

    create_obj_file(mesh, "./data/mesh.obj", indices)
    mesh_model = StaticObject("./data/mesh.obj")
    return mesh_model

def frange(start, end, increment):
    return [x * increment for x in range(int(start * 1.0/increment), int(end * 1.0/increment))]

class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)

    def setup(self, title, WIDTH, HEIGHT, fullscreen):

        super().setup(title, WIDTH, HEIGHT, fullscreen)
        self.sky_box.load_skybox = False

        # make_context_current(self.window)
        # set_cursor_visible(self.window, False)
        self.set_background_color(vec3(1.0, 1.0, 1.0))
        self._setup()

    def _setup(self):

        self.show_shadows = False
        self.show_ssao = False
        self.light = Light(vec3(0, 1, 4), vec3(1,1,1))
        self.start_time = time.time()
        self.axis_3d = StaticObject("../../data/3d_axis.obj")
        self.grid = Grid()
        #glLineWidth(5.0)
        #self.sky_box.load_skybox = False
        # build mesh
        self.mesh = create_mesh("(0.125*math.sin(4*x)-math.cos(5*y))", xdomain=[-2,2], ydomain=[-2,2], detail=15, reversed_normals=False)
        #self.mesh4 = create_mesh("-math.sqrt(4 - x**2 - y**2)", color=vec3(1,0,0), xdomain=[-2,2], ydomain=[-2,2], detail=4, reversed_normals=True)
        #self.fbo = FBO(WIDTH, HEIGHT, True, True, True, True)
        #self.fbo_shader = StaticShader()
        #self.fbo_shader.setup("../../data/fbo_shader.vs","../../data/fbo_shader.fs")

        #self.mesh3 = create_mesh("x**2 + y**2", xdomain=[-2,2], ydomain=[-2,2], detail=4, reversed_normals=True)

        #self.mesh2 = create_mesh("math.sin(x) + math.cos(y)",xdomain=[-5,5], ydomain=[-5,5], detail=4)

        #self.mesh_aabb = AABoundingBox(self.mesh)

        #self.active_camera.Position = vec3(0,0,10)
        #self.active_camera.Pitch = -35.0
        #self.active_camera.Yaw = -135.0
        self.mouse_down = False
        self.scrolled = False
        self.dist_from_centre = 1.0

    def update(self):
        #self.fbo.Draw(self.fbo_shader)
        pass

    def processInput(self, window):
        # if (get_key(window, KEY_ESCAPE) == PRESS):
        #     set_window_should_close(self.window, True);

        # if (get_key(window, KEY_C) == PRESS):
        #     self.set_wireframe = True

        # if (get_key(window, KEY_V) == PRESS):
        #     self.set_wireframe = False
        pass

    def onMouseMoved(self, xpos, ypos):
        if (self.mouse_down):
            xoffset = xpos - self.lastX
            yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

            self.lastX = xpos
            self.lastY = ypos

            modelHeight = 12.0 

            # algorithm from ThinMatrix video on third person cameras
            horizDist = modelHeight * math.cos(math.radians(self.active_camera.pitch))
            vertDist = modelHeight * math.sin(math.radians(self.active_camera.pitch))
            theta = self.active_camera.yaw - 90.0
            offsetx = horizDist * math.sin(math.radians(-theta))
            offsetz = horizDist * math.cos(math.radians(theta))
            self.active_camera.position = vec3(-offsetx, vertDist, -offsetz) * self.dist_from_centre

            # override behaviour of active_camera.ProcessMouseMovement
            # make sure you calculate front AND Right
            # otherwise the app's frustum culling won't function properly
            self.active_camera.yaw   += xoffset * 0.1;
            self.active_camera.pitch -= yoffset * 0.1;
            self.active_camera.front = self.active_camera.position * -1.0
            self.active_camera.Right = normalize(cross(self.active_camera.front, vec3(0,1,0)))

    def onMouseClicked(self, button, action, mods):
        if (button == MOUSE_BUTTON_1 and action == 1):
            self.mouse_down = True
        if (button == MOUSE_BUTTON_1 and action == 0):
            self.mouse_down = False
        print (button, action)

    def onMouseScrolled(self, xpos, ypos):
        self.dist_from_centre -= ypos * 0.1
        modelHeight = 12.0 

        # algorithm from ThinMatrix video on third person cameras
        horizDist = modelHeight * math.cos(math.radians(self.active_camera.pitch))
        vertDist = modelHeight * math.sin(math.radians(self.active_camera.pitch))
        theta = self.active_camera.yaw - 90.0
        offsetx = horizDist * math.sin(math.radians(-theta))
        offsetz = horizDist * math.cos(math.radians(theta))
        self.active_camera.position = vec3(-offsetx, vertDist, -offsetz) * self.dist_from_centre

        # override behaviour of active_camera.ProcessMouseMovement
        # make sure you calculate front AND Right
        # otherwise the app's frustum culling won't function properly

        self.active_camera.front = self.active_camera.position * -1.0
        self.active_camera.Right = normalize(cross(self.active_camera.front, vec3(0,1,0)))
    def onWindowResized(self, width, height):
        pass

    def onKeyPressed(self, key, scancode, action, mods):
        print (key, self.deltaTime, self.currentFrame, self.lastFrame)
        if (key == 119):
            self.active_camera.ProcessKeyboard(0, self.deltaTime)

        if (key == 115):
            self.active_camera.ProcessKeyboard(1, self.deltaTime)

        if (key == 97):
            self.active_camera.ProcessKeyboard(2, self.deltaTime)

        if (key == 100):
            self.active_camera.ProcessKeyboard(3, self.deltaTime)

class DrawArea(gtk.GLArea):

    app = App()

    def __init__(self, WIDTH, HEIGHT):
        gtk.GLArea.__init__(self)
        self.connect("realize", self.on_realize)
        self.connect("render", self.render)
        self.connect("motion-notify-event", self.on_drawing_area_mouse_motion)
        self.connect("button-press-event", self.on_drawing_area_button_press)
        self.connect("button-release-event", self.on_drawing_area_button_release)
        self.connect("scroll-event", self.on_drawing_area_scroll_event)
        self.connect("key-press-event", self.on_drawing_area_key_press)
        self.set_has_depth_buffer(True)

        self.lastX = 0
        self.lastY = 0

        self.set_size_request(400, 400)
        self.set_can_focus(True)

    def on_drawing_area_scroll_event(self, widget, event):
        did_scroll, dx, dy = event.get_scroll_deltas()
        self.app.onMouseScrolled(dx, dy)

    def on_drawing_area_mouse_motion(self, widget, event):
        xpos, ypos = event.x, event.y
        print (xpos, ypos)
        self.app.onMouseMoved(xpos, ypos)

    def on_drawing_area_button_release(self, widget, event): 
        self.app.onMouseClicked(0, 0, None)

    def on_drawing_area_button_press(self, widget, event): 
        self.app.onMouseClicked(0, 1, None)

    
    def on_drawing_area_key_press(self, widget, event):
        self.app.onKeyPressed(event.keyval, 0, 1, 0)

    
    def on_realize(self, area):
        print ("on realize")
        ctx = self.get_context()
        ctx.make_current()
        print (ctx)
        err = self.get_error()
        if err:
            print("The error is {}".format(err))

        # load GLAD
        load_GL()

        self.app.setup("dont load this", WIDTH, HEIGHT, False)

    def render(self, area, ctx):

        WIDTH = self.get_allocation().width
        HEIGHT = self.get_allocation().height
        #glViewport(0,0, WIDTH, HEIGHT);

        self.queue_render()
        self.app.gameLoop()

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


        self.area = DrawArea(600, 500)
        self.area.set_can_focus(True)

        self.vertical_panel = gtk.HPaned()
        self.vertical_panel2 = gtk.HPaned()
        self.horizontal_panel = gtk.VPaned()
        self.horizontal_panel.set_border_width(5)
        self.vertical_panel.set_border_width(5)
        self.vertical_panel2.set_border_width(5)


        self.vertical_panel.set_position(0)

        self.horizontal_panel.pack1(self.area, True, False)
        self.horizontal_panel.set_position(200)
        self.vertical_panel2.pack1(self.horizontal_panel, True, False)
        self.vertical_panel2.set_position(1000)
        self.ui_panel = gtk.HPaned()
        self.entry = gtk.Entry()
        self.entry.set_text("0.125*math.sin(4*x)-math.cos(5*y)")
        self.button = gtk.Button.new_with_label("Show")
        self.ui_panel.pack1(self.entry, True, False)
        self.ui_panel.pack2(self.button, True, False)
        self.button.connect("clicked", self.on_button_clicked)
        #hbox.pack_start(button, True, True, 0)

        self.main_frame.add(self.vertical_panel2)
        self.main_frame.add(self.ui_panel)
        self.window_frame = gtk.Box(orientation=gtk.Orientation.VERTICAL)
        self.window_frame.add(self.main_frame)

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
        self.window.set_title("grapher")
        self.window.show_all()
        self.check_sigint_timer(1)
        gtk.main()

    def on_button_clicked(self, button):
        #self.area.app.axis_3d = StaticModel("../../data/3d_axis.obj")
        self.mesh = create_mesh("(0.125*math.sin(4*x)-math.cos(5*y))", xdomain=[-2,2], ydomain=[-2,2], detail=2, reversed_normals=False)

    def on_window_key_press(self, widget, event, data):
        if (widget == self.area):
            modal_drawarea_window = None

        #if event.keyval == gdk.KEY_Escape:
            #modal_drawarea_window.destroy()
            #self.add_drawarea()

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


    def set_syntax_theme(self, buffer=None):
        manager = gtksource.StyleSchemeManager().get_default()
        #print (manager.get_scheme_ids())
        scheme = manager.get_scheme(self.syntax_theme)
        if (buffer):
            manager = gtksource.StyleSchemeManager().get_default()
            scheme = manager.get_scheme(self.syntax_theme)
            buffer.set_style_scheme(scheme)


    def set_theme(self, t):

        settings = gtk.Settings.get_default()
        settings.set_property("gtk-application-prefer-dark-theme", True)  # if you want use dark theme, set second arg to True
        self.syntax_theme = "oblivion"
        self.set_syntax_theme()


    def check_sigint_timer(self,timeout):
        gobject.timeout_add_seconds(timeout, self.check_sigint)

    def check_sigint(self):
        return True
    def on_selection_changed(self, button):
        self.selected_file = button.get_current_folder()

    def on_selection_changed2(self, button):
        self.selected_file = button.get_filename()


    def remove_drawarea(self):
        self.horizontal_panel.remove(self.area)

    def delete_drawarea(self):
        self.horizontal_panel.get_child1().destroy()

    def add_drawarea(self):
        self.area.set_hexpand(True)
        self.area.set_vexpand(True)
        self.horizontal_panel.pack1(self.area, True, False)


    def can_focus(self, widget):
        pass

    def set_focus(self, window, widget):
        print (widget, widget.has_focus())

    def on_window_clicked(self, widget, event, user_param):
        pass
    def focus_in(self, widget, data):
        print ("focus")

    def on_window_draw(self, window, cr):
        widget = window.get_focus()

if __name__ == "__main__":
        ed = Editor("", [])
