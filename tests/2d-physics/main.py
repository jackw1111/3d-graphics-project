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

WIDTH = 800
HEIGHT = 600

BLOCK_SIZE = 50



class Circle():
    def __init__(self, pos, sz):
        self.icon_rect = Rect2D(pos, vec2(sz, sz), "./data/red_circle.png", 1, 1)
        self.r = sz/2
        self.icon_rect.ordering = 1
        self.icon_rect.frame = 0
        self.inverse_mass = 1
        self.velocity = vec2(0,0)
        self.position = pos
        self.angular_velocity = 40
        self.orientation = 0
        self.inverse_moment_of_inertia = 1

    def circle_collision(self, other):
        return distance(self.position, other.position) < self.r + other.r

    def update(self, force, dt):

        self.prev_position = self.position
        self.prev_velocity = self.velocity
        self.prev_orientation = self.orientation
        self.prev_angular_velocity = self.angular_velocity
        accel = force * self.inverse_mass

        self.velocity += accel  * dt

        self.position += self.velocity * dt
        # if (self.colliding):
        #     r_ap = normalize(perp(self.collision_point - self.position))
        #     torque = dot(accel, r_ap)
        #     self.angular_velocity += torque * (self.inverse_moment_of_inertia) * dt
        #print (self.angular_velocity)
        self.orientation += self.angular_velocity * dt

        # update graphics to match physics
        self.icon_rect.position = self.position
        self.icon_rect.orientation = self.orientation

class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        make_context_current(self.window)
        set_cursor_visible(self.window, False)

        self.light = Light(vec3(0, 1, 4), vec3(1,1,1))

        self.start_time = time.time()

        self.gravity = vec2(0,-400)

        self.circle1 = Circle(vec2(WIDTH/2, HEIGHT/2), 25)
        self.circle2 = Circle(vec2(WIDTH/2, HEIGHT/4), 25)
        self.sky_box.load_skybox = False

    def update(self):

        self.circle1.position = vec2(self.lastX, HEIGHT - self.lastY)
        self.circle1.icon_rect.position = vec2(self.lastX, HEIGHT - self.lastY)

        #self.circle1.update(self.gravity, self.deltaTime)
        #if (self.circle1.position.y < 0.0 + BLOCK_SIZE/2):
        #    self.circle1.velocity.y = self.circle1.velocity.y * -1

        #self.circle2.update(self.gravity, self.deltaTime)
        if (self.circle2.position.y < 0.0 + BLOCK_SIZE/2):
            self.circle2.velocity.y = self.circle2.velocity.y * -1

        if self.circle1.circle_collision(self.circle2):
            self.circle1.velocity.y = self.circle1.velocity.y * -1
            self.circle2.velocity.y = self.circle2.velocity.y * -1
            self.set_background_color(vec3(0,0,1))
        else:
            self.set_background_color(vec3(0,0,0))

        self.process_input(self.window)

        self.end_time = time.time()
        self.elapsed_time = self.end_time - self.start_time

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
