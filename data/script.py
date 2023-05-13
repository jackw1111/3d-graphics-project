import engine
from engine.graphics import *
import random
import time
import math

PRESS = 1
RELEASE = 0
KEY_LEFT_SHIFT = 340

KEY_W = 87
KEY_A = 65
KEY_S = 83
KEY_D = 68

WIDTH = 800
HEIGHT = 600

class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        make_context_current(self.window)
        set_cursor_visible(self.window, True)

        self.light = Light(vec3(0, 1, 4), vec3(1,1,1))
        self.control_camera = False
        self.active_camera.position = vec3(0,1,5)
        self.lx = Line3D(vec3(0,0,0), vec3(1,0,0))
        self.lx.color = vec3(1,0,0)
        self.ly = Line3D(vec3(0,0,0), vec3(0,1,0))
        self.ly.color = vec3(0,1,0)
        self.lz = Line3D(vec3(0,0,0), vec3(0,0,1))
        self.lz.color = vec3(0,0,1)
    def update(self):
        self.process_input(self.window)

    def process_input(self, window):

        if (get_key(window, KEY_W) == PRESS):
            self.active_camera.ProcessKeyboard(0, self.deltaTime)

        if (get_key(window, KEY_S) == PRESS):
            self.active_camera.ProcessKeyboard(1, self.deltaTime)

        if (get_key(window, KEY_A) == PRESS):
            self.active_camera.ProcessKeyboard(2, self.deltaTime)

        if (get_key(window, KEY_D) == PRESS):
            self.active_camera.ProcessKeyboard(3, self.deltaTime)

        if (get_key(window, KEY_LEFT_SHIFT) == PRESS):
            self.active_camera.MovementSpeed = 20.0

        if (get_key(window, KEY_LEFT_SHIFT) == RELEASE):
            self.active_camera.MovementSpeed = 4.0

    def on_mouse_moved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos
        if self.control_camera:
            self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)

        self.current_coord = vec3(self.lastX, HEIGHT-self.lastY, 0.0)

    def on_mouse_clicked(self, button, action, mods):
        if (button == 2 and action == 1):
            self.control_camera = True
            set_cursor_visible(self.window, False)
        if (button == 2 and action == 0):
            self.control_camera = False
            set_cursor_visible(self.window, True)

    def on_window_resized(self, width, height):
        pass

    def on_key_pressed(self, key, scancode, action, mods):
        pass

app = App("example", WIDTH, HEIGHT, False, False)
run(app)