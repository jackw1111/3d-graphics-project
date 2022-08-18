import sys,os
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from engine.graphics import *
import math
import random

from keys import *
from selection_box import *

WIDTH = 800
HEIGHT = 600

def get_position(model_matrix):
    v = model_matrix[3]
    return vec3(v.x, v.y, v.z)

class App(Application):
    
    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        set_cursor_visible(self.window, True)
        self.sky_box.load_skybox = False
        self.active_camera.set_far_plane(50.0)
        self.active_camera.set_near_plane(0.1)

        self.selection_box = SelectionBox(WIDTH, HEIGHT)

        self.mouse_down = False

        self.control_camera = False
        self.light = Light(vec3(0, 1, 4), vec3(1,1,1))
        self.active_camera.position = vec3(0,0,20)

        self.players = []

        for i in range(10):
            self.player = StaticObject("../example/data/astroboy.dae")
            self.player.model_matrix = translate(mat4(1.0), vec3(i*5,0,0))
            #self.player.draw_bounding_box = True
            offset = random.uniform(0, 10)
            self.player.offset = offset
            #self.player.set_frames(0.0, 1.0, self.player.offset)
            self.players.append(self.player)

        self.i = 0
        self.first_mouse = True
        self.paused = False

    def update(self):

        self.process_input(self.window)

        self.selection_box.update(self.lastX, self.lastY, self.active_camera, self.mouse_down, self.players)

        for m in self.players:
            m.offset += 0.01
            #m.set_frames(0.0, 1.11, m.offset)
        #print (distance(self.active_camera.position, get_position(self.players[0].model_matrix)))

    def process_input(self, window):
        if (get_key(window, KEY_ESCAPE) == PRESS):
            set_window_should_close(self.window, True);

        if (get_key(window, KEY_W) == PRESS):
            self.active_camera.position += self.active_camera.front * self.deltaTime * self.active_camera.MovementSpeed
        if (get_key(window, KEY_S) == PRESS):
            self.active_camera.position -= self.active_camera.front * self.deltaTime * self.active_camera.MovementSpeed
        if (get_key(window, KEY_A) == PRESS):
            self.active_camera.position -= self.active_camera.right * self.deltaTime * self.active_camera.MovementSpeed
        if (get_key(window, KEY_D) == PRESS):
            self.active_camera.position += self.active_camera.right * self.deltaTime * self.active_camera.MovementSpeed

        if (get_key(window, KEY_LEFT_SHIFT) == PRESS):
            self.active_camera.MovementSpeed = 5.0
        else:
            self.active_camera.MovementSpeed = 1.0

        if get_key(window, KEY_P) == PRESS:
            self.paused = True
        if get_key(window, KEY_U) == PRESS:
            self.paused = False

    def on_mouse_moved(self, xpos, ypos):

        if self.first_mouse:
            self.lastX = xpos
            self.lastY = ypos
            self.first_mouse = False
    

        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos

        if not self.mouse_down:
            self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)


    def on_mouse_clicked(self, button, action, mods):

        if (button == 2 and action == 1):
            self.control_camera = True
            set_cursor_visible(self.window, False)
        if (button == 2 and action == 0):
            self.control_camera = False
            set_cursor_visible(self.window, True)
            self.first_mouse = True

        if (button == 0 and action == 1):
            self.mouse_down = True
        if (button == 0 and action == 0):
            self.mouse_down = False

    def on_key_pressed(self, key, scancode, action, mods):
        pass

if __name__ == "__main__":
    app = App("aom", WIDTH, HEIGHT, False, True)
    run(app)

