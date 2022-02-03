import sys
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from console import *
from axis_3d import *
from engine.graphics import *
from keys import *
import random
import time
import math
from OpenGL.GL import *
from collision2 import *


WIDTH = 800
HEIGHT = 600


class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        make_context_current(self.window)
        set_cursor_visible(self.window, False)
        self.active_camera.set_far_plane(30.0)

        self.light = Light(vec3(0, 1, 4), vec3(1,1,1))

        self.start_time = time.time()

        self.map = StaticObject("./data/map3.obj")
        self.map.model_matrix = scale(translate(mat4(1.0), vec3(0,-40,0)), vec3(100,100,100))
        self.entity = CharacterEntity2(self.map.model, self.map.model_matrix, 1.0)
        self.ball = AnimatedObject("./data/red_ball.dae")

    def update(self):

        self.process_input(self.window)

        self.end_time = time.time()
        self.elapsed_time = self.end_time - self.start_time
        self.entity.velocity = vec3(0,-0.1,0)
        self.entity.update(self.deltaTime)
        self.ball.model_matrix = translate(mat4(1.0), self.entity.position)

    def process_input(self, window):
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

    def on_mouse_moved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos
        self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)

    def on_mouse_clicked(self, button, action, mods):
        pass

    def on_window_resized(self, width, height):
        pass

    def on_key_pressed(self, key, scancode, action, mods):
        #self.console.onKeyPressed(key, scancode, action, mods)
        pass
if __name__ == "__main__":
    app = App("test", WIDTH, HEIGHT, False)
    run(app)
