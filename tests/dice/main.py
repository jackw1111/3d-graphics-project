import sys
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from third_person_camera import *
from axis_3d import *
from math_funcs import *
from engine.graphics import *
from keys import *
from player import *
import random
import time
import math

WIDTH = 640
HEIGHT = 480 


def reflect(I, N):
    return I  + N  * -2.0 * dot(N, I) 

class Die():
    all_dies = []
    def __init__(self, position):
        choices = ["../minecraft/data/sand_block.obj", "../minecraft/data/tree_block.obj", "../rubiks-cube/data/block.obj", "../minecraft/data/leaves_block.obj", "./data/die.dae", "../minecraft/data/grass_block.obj", "../minecraft/data/wood_block.obj", "../minecraft/data/water_block.obj", "../minecraft/data/gravel_block.obj"]
        self.model = StaticObject(random.choice(choices))
        self.position = position
        self.model.model_matrix = translate(mat4(1.0), self.position)
        rx, ry, rz = random.uniform(-1,1), random.uniform(-1,1), random.uniform(-1,1)
        self.collision_box = CollisionBox(self.position.x, self.position.y, self.position.z, -10.0, rx, ry, rz, 1.0, 1.0, 1.0)
        Die.all_dies.append(self)

    def update(self):
        self.model.model_matrix = scale(self.collision_box.model_matrix, vec3(0.5, 0.5, 0.5))
        self.position = get_position(self.model.model_matrix)

class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        make_context_current(self.window)
        set_cursor_visible(self.window, False)
        self.active_camera.set_near_plane(1.0)
        self.light = Light(vec3(-4,8,-2), vec3(1,1,1))


        self.floor = StaticObject("./data/plane.obj")
        self.floor.model_matrix = translate(rotate(scale(mat4(1.0), vec3(20,1,20)), math.radians(-90.0), vec3(1,0,0)), vec3(0,0,1))

        self.floor_collision_box = CollisionBox(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 100000.0, 1.0, 100000.0)

        self.third_person_camera = ThirdPersonCamera(vec3(0,0,0), vec3(0,0,-1), vec3(0,1,0), 0.0, -90.0)
        self.active_camera.position = vec3(0,5,10)

    def update(self):
        self.process_input(self.window)

        for die in Die.all_dies:
            die.update()

    def process_input(self, window):
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

        self.active_camera.position += normalize(total_velocity) * speed

    def on_mouse_moved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos
        self.active_camera.ProcessMouseMovement(xoffset, yoffset, False)

    def on_mouse_clicked(self, button, action, mods):
        if (action == 1):
            world_pos = ray_cast(WIDTH/2, HEIGHT/2, self.active_camera.projection_matrix, self.active_camera.view_matrix)
            self.die = Die(self.active_camera.position + world_pos * 10)

    def on_window_resized(self, width, height):
        pass

    def on_key_pressed(self, key, scancode, action, mods):
        if (key == KEY_R and action == 1):
            pass

if __name__ == "__main__":
    app = App("dice", WIDTH, HEIGHT, False)
    run(app)
