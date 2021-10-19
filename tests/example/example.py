import sys
sys.path.append("/home/me/Documents/3d-graphics-project/engine/bin")
sys.path.append("/home/me/Documents/3d-graphics-project/engine/utils")
from console import *
from axis_3d import *
from engine.graphics import *
from engine.core.physics import collision_SAT
from keys import *
from player import *
import random
import time
import math
from OpenGL.GL import *

WIDTH = 800
HEIGHT = 600


class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        make_context_current(self.window)
        set_cursor_visible(self.window, False)

        self.show_shadows = True
        self.show_ssao = False
        self.light = Light(vec3(0, 1, 4), vec3(1,1,1))

        self.start_time = time.time()

    def update(self):

        self.processInput(self.window)

        self.end_time = time.time()
        self.elapsed_time = self.end_time - self.start_time

    def processInput(self, window):
        if (get_key(window, KEY_ESCAPE) == PRESS):
            set_window_should_close(self.window, True);

        if (get_key(window, KEY_W) == PRESS):
            self.active_camera.ProcessKeyboard(0, self.deltaTime)

        if (get_key(window, KEY_S) == PRESS):
            self.active_camera.ProcessKeyboard(1, self.deltaTime)

        if (get_key(window, KEY_A) == PRESS):
            self.active_camera.ProcessKeyboard(2, self.deltaTime)

        if (get_key(window, KEY_D) == PRESS):
            self.active_camera.ProcessKeyboard(3, self.deltaTime)

        if (get_key(window, KEY_E) == PRESS):
            self.show_ssao = False
            self.use_normal_map = False

        if (get_key(window, KEY_R) == PRESS):
            self.show_ssao = True


        if (get_key(window, KEY_Y) == PRESS):
            self.use_normal_map = True

    def onMouseMoved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos
        self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)

    def onMouseClicked(self, button, action, mods):
        pass

    def onWindowResized(self, width, height):
        pass

    def onKeyPressed(self, key, scancode, action, mods):
        #self.console.onKeyPressed(key, scancode, action, mods)
        pass
if __name__ == "__main__":
    app = App("test", WIDTH, HEIGHT, False)
    run(app)
