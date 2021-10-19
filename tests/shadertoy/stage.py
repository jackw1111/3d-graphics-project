
import sys, os
sys.path.append("/home/me/Documents/3d-graphics-project/engine/bin")
PROJECT_DIR = os.path.dirname(sys.argv[1])
sys.path.append(PROJECT_DIR)

import math
import random
import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk as gtk
from gi.repository import Pango

from engine.graphics import *
from OpenGL.GL import *
from keys import *
import time
from gi.repository import Gdk as gdk
WIDTH = 600
HEIGHT = 600

import main
WIDTH = 420
HEIGHT = 236


class App(Application):

    deltaTime = 0.0
    lastFrame = 0.0
    currentFrame = 0.0

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)

    def setup(self, title, WIDTH, HEIGHT, fullscreen):
        #Application.setup(title, WIDTH, HEIGHT, fullscreen)
        self._setup()

    def setImageChannel(self, channel):
        pass

    def setBufferChannel(self, channel):
        pass

    def editShader(self, original_file, new_file):
        header = "#version 330 core\n"+ \
        "in vec2 TexCoords;\n"+ \
        "uniform vec2 iMouse;\n"+ \
        "uniform vec2 iResolution;\n"+ \
        "uniform float iTime;\n"+ \
        "uniform sampler2D iChannel0;\n"+ \
        "uniform sampler2D iChannel1;\n"+ \
        "uniform sampler2D iChannel2;\n"+ \
        "uniform sampler2D iChannel3;\n"+ \
        "out vec4 fragColor;\n"+ \
        "vec2 fragCoord = gl_FragCoord.xy;\n" \
        "#define PI 3.14159265359\n"+ \
        "#define TWOPI 6.28318530718\n"

        with open(original_file, 'r') as f:
            with open(new_file, 'w') as f2:
                f2.write(header)
                lines = f.readlines()
                for line in lines:
                    # replace void mainImage(...) with void main()
                    if "mainImage" in line:
                        if "{" in line:
                            f2.write("void main(){")
                        else:
                            f2.write("void main()")
                    else:
                        f2.write(line)

    def _setup(self):
        self.postprocess_shader = StaticShader()
        self.output_shader = StaticShader()
        self.channel_0_shader = StaticShader()
        self.channel_1_shader = StaticShader()

        self.output_shader.setup("./data/screen_shader.vs", "./data/output_shader.fs")

        # add header to start of file, so file can be cut-and-paste directly from shadertoy

        original_file = "./data/screen_shader.fs"
        new_file = "./data/screen_shader_edit.fs"
        self.editShader(original_file, new_file)
        self.postprocess_shader.setup("./data/screen_shader.vs", new_file)

        original_file = "./data/channel_0_shader.fs"
        new_file = "./data/channel_0_shader_edit.fs"
        self.editShader(original_file, new_file)
        self.channel_0_shader.setup("./data/channel_0_shader.vs", new_file)


        original_file = "./data/channel_1_shader.fs"
        new_file = "./data/channel_1_shader_edit.fs"
        self.editShader(original_file, new_file)
        self.channel_1_shader.setup("./data/channel_0_shader.vs", new_file)

        #set_cursor_visible(self.window, True)

        self.screen_fbo = FBO(WIDTH, HEIGHT, True, True, True, True)
        self.buffer_a = FBO(WIDTH, HEIGHT, True, True, True, True)

        self.buffer_a.texture = texture_from_file("Diffuse.png", "./data/")
        self.texture = texture_from_file("Diffuse.png", "./data/")

        #set_wireframe(True)
    def draw(self):
        self.currentFrame  = time.time()
        self.deltaTime = (self.currentFrame - self.lastFrame)
        self.lastFrame = self.currentFrame

        # draw to screen buffer
        self.screen_fbo.bind(True)
        clear_depth_buffer()
        clear_color_buffer()
        self.postprocess_shader.use()
        BindTexture(GL_TEXTURE0 + self.buffer_a.texture, self.buffer_a.texture)
        BindTexture(GL_TEXTURE0 + self.texture, self.texture)
        self.postprocess_shader.setInt("iChannel0", self.buffer_a.texture)
        self.postprocess_shader.setVec2("iResolution", vec2(WIDTH, HEIGHT))
        self.postprocess_shader.setFloat("iTime", self.currentFrame % 60)
        self.screen_fbo.Draw(self.postprocess_shader)
        self.screen_fbo.bind(False)

        self.screen_fbo.Draw(self.postprocess_shader)

    def update(self):
        self.processInput(self.window)
        self.draw()

    def processInput(self, window):
        if (get_key(window, KEY_ESCAPE) == PRESS):
            set_window_should_close(self.window, True);

        if (get_mouse_button(window, MOUSE_BUTTON_1) == PRESS):
            self.postprocess_shader.setVec2("iMouse", vec2(self.lastX, HEIGHT - self.lastY))

    def onMouseMoved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos


    def onMouseClicked(self, button, action, mods):
        pass

    def onWindowResized(self, width, height):
        pass

    def onKeyPressed(self, key, scancode, action, mods):
        pass


#app.setup("dont load this", WIDTH, HEIGHT, False)
#run(app)


class DrawArea(gtk.GLArea):

    #player = Player()
    camera = Camera(vec3(0.0, 0.0, 10.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0), -90.0, 0.0)
    projection = mat4(1.0)
    get_key = {gdk.KEY_w : 0, gdk.KEY_a : 0, gdk.KEY_s : 0, gdk.KEY_d : 0}
    get_mouse = {gdk.ScrollDirection.UP: 0}
    lastX = 0
    lastY = 0
    # poly1 = Poly()
    # poly2 = Poly()
    focus_point = vec3(0,0,0)
    app = App("dont load this", WIDTH, HEIGHT, False)

    dist = 4
    def on_drawing_area_scroll_event(self, widget, event):
        print (event.direction)
        dx = 0
        dy = 0
        did_scroll, dx, dy = event.get_scroll_deltas()
        self.dist += dy
    def on_drawing_area_button_press(self, widget, event): 
        xpos, ypos = event.x, event.y
        self.get_mouse[event.button] = 1
        if (self.get_mouse[gdk.BUTTON_MIDDLE]):
                    x = (2.0 * WIDTH/2) / WIDTH - 1.0
                    y = 1.0 - (2.0 * HEIGHT/2) / HEIGHT
                    z = 1.0
                    ray_nds = vec3(x, y, z)
                    ray_clip = vec4(ray_nds.x, ray_nds.y, -1.0, 1.0)

                    # # eye space to clip we would multiply by projection so
                    # # clip space to eye space is the inverse projection
                    ray_eye = inverse(self.app.projection) * ray_clip

                    # # convert point to forwards
                    ray_eye = vec4(ray_eye.x, ray_eye.y, -1.0, 0.0)
                    # # world space to eye space is usually multiply by view so
                    # # eye space to world space is inverse view
                    #print (self.player.view)
                    inv_ray_wor = (inverse(self.app.view) * ray_eye)
                    ray_wor = vec3(inv_ray_wor.x, inv_ray_wor.y, inv_ray_wor.z)
                    ray_wor = normalize(ray_wor)
                    for current_object in self.app.all_objects:
                        #print (self.camera.Position)
                        if (intersect(self.camera.Position, ray_wor, vec3(-25.0, -3.0, 60.0))):
                            print ("player 2 hit!@!")
                        else:
                            pass
    def on_drawing_area_mouse_motion(self, widget, event):
        xpos, ypos = event.x, event.y
        xoffset = self.lastX - xpos
        yoffset = self.lastY - ypos
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos
        ctrl = (event.state & gdk.ModifierType.CONTROL_MASK)

        if (self.get_mouse[gdk.BUTTON_MIDDLE] and ctrl):
            cam_pos = self.camera.Position
            print (xoffset, yoffset)
            self.focus_point -= self.camera.Right * xoffset 
            self.focus_point += vec3(0,1,0) * yoffset
            self.view = lookAt(self.camera.Position, self.focus_point, vec3(0,1,0))

        elif (self.get_mouse[gdk.BUTTON_MIDDLE]):
            speed = 3.0
            self.camera.ProcessMouseMovement(xoffset * speed, yoffset * speed, True)

    def on_drawing_area_button_release(self, widget, event): 
        self.get_mouse[event.button] = 0

    def on_drawing_area_key_press(self, widget, event):
        self.get_key[event.keyval] = 1

    def on_drawing_area_key_release(self, widget, event):
        self.get_key[event.keyval] = 0

    def __init__(self, WIDTH, HEIGHT):
        gtk.GLArea.__init__(self)
        self.connect("realize", self.on_realize)
        self.connect("render", self.render)
        self.connect("draw", self.on_draw)
        self.connect("motion-notify-event", self.on_drawing_area_mouse_motion)
        self.connect("scroll-event", self.on_drawing_area_scroll_event)
        self.connect("button-press-event", self.on_drawing_area_button_press)
        self.connect("button-release-event", self.on_drawing_area_button_release)
        self.connect("key-press-event", self.on_drawing_area_key_press)
        self.connect("key-release-event", self.on_drawing_area_key_release)

        self.add_events(gdk.EventMask.BUTTON_PRESS_MASK)
        self.add_events(gdk.EventMask.KEY_PRESS_MASK)
        self.set_size_request(400, 400)
        self.set_can_focus(True)
        self.projection = perspective(45.0, float(WIDTH)/float(HEIGHT), 0.1, 100.0)
        self._model = mat4(1.0)
        # self.collision = False
        self.set_has_depth_buffer(True)



    def on_draw(self, darea, cr):
        WIDTH = self.get_allocation().width
        HEIGHT = self.get_allocation().height
        #self.test_shader.setMat4("projection", self.projection)
        self.process_input()
        # self.poly1.setup(mat4(1.0))
        # self.poly1.setup2(self.planeModel.meshes[0].vertices)
        # self.poly2.setup(self._model)
        # self.poly2.setup2(self.testModel.meshes[0].vertices)
        # if (check_collision(self.poly1, self.poly2)):
        #    	    self.collision = True
        # else:
        #    	    self.collision = False

    def ProcessKeyboard(self, direction, deltaTime):
        speed = self.camera.MovementSpeed * deltaTime;
        if (direction == 0):
            self.camera.position += self.camera.front * speed
        if (direction == 1):
            self.camera.position += self.camera.front * -speed
        if (direction == 2):
            self.camera.position += self.camera.right * -speed
        if (direction == 3):
            self.camera.position += self.camera.right * speed

    def ProcessMouseMovement(self, xoffset, yoffset):
        speed = 1.0
        #xoffset *= MouseSensitivity;
        #yoffset *= MouseSensitivity;

        #self.camera.Yaw   += xoffset
        #self.camera.Pitch += yoffset

        # Make sure that when pitch is out of bounds, screen doesn't get flipped
        #if (self.camera.Pitch > 89.0):
        #  self.camera.Pitch = 89.0;
        #if (self.camera.Pitch < -89.0):
        #  self.camera.Pitch = -89.0;

        # Update Front, Right and Up Vectors using the updated Euler angles
        #self.camera.updateCameraVectors();
        #self.camera.Position -= self.camera.Front * speed

    def process_input(self):
        try:
            if (self.get_key[gdk.KEY_w] == 1):
                self.ProcessKeyboard(0, 0.1)
            if (self.get_key[gdk.KEY_a] == 1):
                self.ProcessKeyboard(1, 0.1)
            if (self.get_key[gdk.KEY_s] == 1):
                self.ProcessKeyboard(2, 0.1)
            if (self.get_key[gdk.KEY_d] == 1):
                self.ProcessKeyboard(3, 0.1)
        except:
            pass


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

        self.camera.position = vec3(0, 4, 10)
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

    def render(self, area, ctx):
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glCullFace(GL_BACK)
        WIDTH = self.get_allocation().width
        HEIGHT = self.get_allocation().height
        glViewport(0,0, WIDTH, HEIGHT);
        self.queue_render()

        # draw scene
        horizDist = self.dist * math.cos(-math.radians(self.camera.pitch))
        vertDist = self.dist * math.sin(math.radians(self.camera.pitch))
        theta = self.camera.yaw
        offsetx = horizDist * math.sin(math.radians(theta))
        offsetz = horizDist * math.cos(math.radians(theta))
        self.camera.position = self.focus_point + vec3(-offsetx, vertDist, -offsetz)
        self.view = lookAt(self.camera.position, self.focus_point, vec3(0,1,0))

        glViewport(0,0,WIDTH,HEIGHT)
        self.projection = perspective(45.0, float(WIDTH)/float(HEIGHT), 0.1, 100.0)
        self.camera.position = vec3(0,0,10)

        # very hacky, revise structure of stage/main interaction
        self.app.view = self.view
        self.app.draw()

        # draw axis

        clear_depth_buffer()
        horizDist = 2 * math.cos(-math.radians(self.camera.pitch))
        vertDist = 2 * math.sin(math.radians(self.camera.pitch))
        theta = self.camera.yaw
        offsetx = horizDist * math.sin(math.radians(theta))
        offsetz = horizDist * math.cos(math.radians(theta))
        self.camera.position = self.focus_point + vec3(-offsetx, vertDist, -offsetz)
        self.view = lookAt(self.camera.position, self.focus_point, vec3(0,1,0))
        glViewport(WIDTH-50,HEIGHT-50,50, 50)
        self.projection = perspective(45.0, float(WIDTH)/float(HEIGHT), 0.1, 100.0)

