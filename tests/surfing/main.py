import sys
sys.path.append("/home/me/Documents/3d-graphics-project/engine/bin")
sys.path.append("/home/me/Documents/3d-graphics-project/engine/utils")
from console import *
from axis_3d import *
from engine.graphics import *
#from engine.core.physics import collision_SAT
from keys import *
from player import *
import random
import time
import math
from OpenGL.GL import *

WIDTH = 1280
HEIGHT = 720

class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        make_context_current(self.window)
        set_cursor_visible(self.window, False)
        self.set_far_plane(4000.0)

        self.light = Light(vec3(-4,8,-2), vec3(1,1,1))

        # simple UI
        self.crosshair = Rect(vec2(WIDTH/2, HEIGHT/2), vec2(25, 25), "./data/crosshair.png")
        self.scope = Rect(vec2(WIDTH/2, HEIGHT/2), vec2(WIDTH, HEIGHT), "./data/scope.png")
        self.scope.to_draw = False


        self.map = StaticObject("./data/surfing_map.obj")
        #self.map.model_matrix = translate(self.map.model_matrix, vec3(21.2147, 30.0086, 28.7207) * -1.0)
        #self.map.model_matrix = translate(mat4(1.0), vec3(-1,0,0))
        self.map.model_matrix = scale(self.map.model_matrix, vec3(100,100,100))


        self.console = Console(WIDTH, HEIGHT)

        self.show_shadows = True

        self.rlastX = 0
        self.rlastY = 0
        self.speed = 1

        self.map_position =self.map.model_matrix
        collision_objects = [self.map_position]
        v = [self.map.model]
        self.entity = CharacterEntity(v, collision_objects, vec3(3.0, 3.0, 3.0))

        self.active_camera.MovementSpeed = 900.0

        self.set_background_color(vec3(0.8, 0.9, 1.0))

        self.gravity = vec3(0,-10,0)
        self.jump = vec3(0,0,0)


    def update(self):
        self.processInput(self.window)
        self.console.update(self.currentFrame, self.deltaTime)

        if (length(self.entity.velocity) > 3.0):
            self.entity.velocity = normalize(self.entity.velocity) * 3.0
        self.entity.velocity += self.gravity * self.deltaTime
        self.entity.velocity += self.jump * self.deltaTime
        self.entity.update()

        if (self.entity.grounded):
            self.jump.y = 0

        if (self.jump.y > 0):
            self.jump.y -= self.deltaTime * 20
        else:
            self.jump.y = 0
        self.active_camera.position = self.entity.position


    def processInput(self, window):
        if (get_key(window, KEY_ESCAPE) == PRESS):
            set_window_should_close(self.window, True);
        speed = self.active_camera.MovementSpeed * self.deltaTime;
        total_velocity = vec3(0,0,0)
        if (get_key(window, KEY_W) == PRESS):
            total_velocity += self.active_camera.front
        if (get_key(window, KEY_S) == PRESS):
            total_velocity -= self.active_camera.front
        if (get_key(window, KEY_A) == PRESS):
            total_velocity -= self.active_camera.right
        if (get_key(window, KEY_D) == PRESS):
            total_velocity += self.active_camera.right

        self.entity.velocity += normalize(total_velocity) * speed

        if (get_key(window, KEY_R) == PRESS):
            self.entity.position = vec3(-9535.931641, 2140.882812, -155.871521)


    def onMouseMoved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos
        self.active_camera.ProcessMouseMovement(xoffset, yoffset, False)

    def onMouseClicked(self, button, action, mods):
        print (MOUSE_BUTTON_2, button, action)

    def onWindowResized(self, width, height):
        pass

    def onKeyPressed(self, key, scancode, action, mods):
        self.console.onKeyPressed(key, scancode, action, mods)
        if (key == KEY_1 and action == 1):
            self.gun = self.gun1
        if (key == KEY_2 and action == 1):
            self.gun = self.gun2
        if (key == KEY_V and action == 1):
            self.debug = True
        elif (key == KEY_V and action == 0):
            self.debug = False
        if (key == KEY_SPACE and action == 1):
            if (self.jump == vec3(0,0,0)):
                self.jump = vec3(0,30,0)

if __name__ == "__main__":
    app = App("surfing", WIDTH, HEIGHT, True)
    run(app)
