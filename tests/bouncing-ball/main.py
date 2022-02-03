#!/usr/bin/python
import sys,os
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from engine.graphics import *
from OpenGL.GL import *
from keys import *
from engine.graphics import *
import random
import time
import math
from OpenGL.GL import *

WIDTH = 600
HEIGHT = 600

def reflect(I, N):
    return I  + N  * -2.0 * dot(N, I) 

def sphere_plane_intersect(sphere_pos, sphere_radius, plane_pos, plane_normal):
    u = sphere_pos - plane_pos
    return dot(u, plane_normal) - sphere_radius

class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        set_cursor_visible(self.window, False)
        self.ball = AnimatedObject("./data/basketball.dae")
        self.ball.position = vec3(0,5,0)
        self.ball.velocity = vec3(0,0,0)
        self.gravity = vec3(0,-10,0)
        self.plane = StaticObject("../example/data/plane.obj")
        self.plane.position = vec3(0,0,0)

        self.light = Light(vec3(-4, 8,-2), vec3(1,1,1))
        self.ball.translation_matrix = mat4(1.0)
        self.ball.rotation_matrix = mat4(1.0)
        self.active_camera.position = vec3(0,3,20)
        self.active_camera.set_far_plane(100.0)

        self.rotation_z = 0
        self.rotation_x = 0

    def update(self):

        self.process_input(self.window)

        self.plane.normal_matrix = transpose(inverse(mat3_cast(self.plane.model_matrix)))
        self.plane.normal = vec3(0,1,0)
        self.plane.normal = normalize(self.plane.normal_matrix * self.plane.normal)

        self.ball.velocity += self.gravity * self.deltaTime
        self.ball.position += self.ball.velocity * self.deltaTime

        t = sphere_plane_intersect(self.ball.position, 1.0, vec3(0,0,0), self.plane.normal)
        if (t < 0):
            self.ball.position -= self.plane.normal * t
            self.ball.velocity = reflect(self.ball.velocity, self.plane.normal) #TO DO restitution

            self.rotation_z = self.plane.normal.z * 10
            self.rotation_x = self.plane.normal.x * 10

        self.ball.rotation_matrix = mat4_cast(angle_axis(math.radians(self.rotation_z) ,vec3(1, 0, 0))) * self.ball.rotation_matrix
        self.ball.rotation_matrix = mat4_cast(angle_axis(math.radians(-self.rotation_x) ,vec3(0, 0, 1)))  * self.ball.rotation_matrix
        self.ball.translation_matrix = translate(mat4(1.0), self.ball.position)
        self.ball.model_matrix = self.ball.translation_matrix * self.ball.rotation_matrix

    def process_input(self, window):
        if (get_key(window, KEY_ESCAPE) == PRESS):
            set_window_should_close(self.window, True);

        if (get_key(window, KEY_R) == PRESS):
            self.ball.position = vec3(0,5,0)
            self.ball.velocity = vec3(0,0,0)
            self.rotation_z = 0
            self.rotation_x = 0

    def on_mouse_moved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos

        self.plane.model_matrix = rotate(mat4(1.0), math.radians(self.lastY*0.01), vec3(1,0,0))
        self.plane.model_matrix = rotate(self.plane.model_matrix, math.radians(-self.lastX*0.01), vec3(0,0,1))

    def on_mouse_clicked(self, button, action, mods):
        if (button == 2 and action == 1):
            set_cursor_visible(self.window, False)

    def on_window_resized(self, width, height):
        pass

    def on_key_pressed(self, key, scancode, action, mods):
        pass

app = App("bouncing-ball", WIDTH, HEIGHT, False)
run(app)