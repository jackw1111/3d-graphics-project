#!/usr/bin/python

import sys,os
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
import random
import time
import math
from OpenGL.GL import *
from keys import *
from third_person_camera import *
from engine.graphics import *

WIDTH = 600
HEIGHT = 600

def saturate(t):
    return min((max(t, 0), 1))

def closest_point_on_line_segment(A, B, Point):
    AB = B - A
    t = dot(Point - A, AB) / dot(AB, AB)
    return A + AB * saturate(t)

class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        set_cursor_visible(self.window, False)

        self.player = AnimatedObject("./data/basketball.dae")
        self.player.position = vec3(0,5,0)
        self.player.velocity = vec3(0,0,0)
        self.gravity = vec3(0,-10,0)

        self.plane1 = StaticObject("../example/data/plane.obj")
        self.plane1.position = vec3(0,0,0)

        self.gravity = vec3(0,-10,0)

        self.set_background_color(vec3(0,0,0))
        self.light = Light(vec3(-4, 8,-2), vec3(1,1,1))
        self.player.up = vec3(0,1,0)
        self.player.right = vec3(1,0,0)
        self.player.front = vec3(0,0,1)
        self.active_camera.position = vec3(0,3,20)
        self.rotation_z = 0
        self.rotation_y = 0
        self.rotation_x = 0

        self.active_camera.set_far_plane(50.0)
        self.active_camera.set_near_plane(0.1)
        self.active_camera.position = vec3(0,3,6)

        self.rotation_z = 0
        self.rotation_x = 5

        # self.smoke_trail = []
        # self.no_of_smoke_puffs = 100
        self.v1 = vec3(0,1,0)
        self.v2 = vec3(1,1,0)
        self.v3 = vec3(1,1,4)

        self.l1 = Line3D(self.v1, self.v2)
        self.l1.color = vec3(0,1,0)

        self.l2 = Line3D(self.v2, self.v3)
        self.l2.color = vec3(0,1,0)
        self.l3 = Line3D(self.v3, self.v1)
        self.l3.color = vec3(0,1,0)
        self.tri_normal = normalize(cross(self.v2 -self.v1, self.v3 - self.v1))
        self.l4 = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.l4.color = vec3(0,0,1)

        self.label1 = Label("false", vec2(100,100), "../minecraft/data/Minecraftia.ttf", 1)

        self.v1 = vec3(2,4,1)
        self.v2 = vec3(1,1,1)
        self.v3 = vec3(4,1,1)

        self.l1 = Line3D(self.v1, self.v2)
        self.l1.color = vec3(0,1,0)
        self.l2 = Line3D(self.v2, self.v3)
        self.l2.color = vec3(0,1,0)
        self.l3 = Line3D(self.v3, self.v1)
        self.l3.color = vec3(0,1,0)
        self.tri_centroid = vec3((self.v1.x + self.v2.x + self.v3.x)*0.333, (self.v1.y + self.v2.y + self.v3.y)*0.333, (self.v1.z + self.v2.z + self.v3.z)*0.333)
        self.tri_normal = normalize(cross(self.v2 - self.v1, self.v3 - self.v1))
        self.tri_normal_line = Line3D(self.tri_centroid, self.tri_centroid + self.tri_normal)
        self.tri_normal_line.color = vec3(0,0,1)
        glLineWidth(5.0)

        self.third_person_camera = ThirdPersonCamera(vec3(0,0,0), vec3(0,0,-1), vec3(0,1,0), 0.0, 90.0)
        self.third_person_camera.distance = 10.0
        self.sphere_radius = 1.0

    def update(self):
        self.t = ray_intersect_plane(self.tri_normal, self.tri_centroid, self.player.position, normalize(self.player.velocity))
        point0 = self.player.position + normalize(self.player.velocity) * self.t
        c0 = cross(point0 - self.v1, self.v2 - self.v1) 
        c1 = cross(point0 - self.v2, self.v3 - self.v2) 
        c2 = cross(point0 - self.v3, self.v1 - self.v3)
        inside = dot(c0, self.tri_normal) <= 0 and dot(c1, self.tri_normal) <= 0 and dot(c2, self.tri_normal) <= 0
        self.sphere_radius = 1.0
        best_point = vec3(0,0,0)
        intersection_vec = vec3(0,0,0)
        intersection_normal = vec3(0,0,0)
        best_distsq = sys.float_info.max
        pos = self.player.position + normalize(self.player.velocity) * self.sphere_radius

        if (self.t > 0 and dot(self.tri_normal, self.player.velocity) < 0 and inside):
            dist = dot(self.player.position - point0, self.tri_normal) # signed distance between sphere and plane

            if (dist < self.sphere_radius):
                print ('intersecting')
                best_point = point0
                intersection_vec = pos - point0
                intersection_normal = self.tri_normal
        else:
            radiussq = self.sphere_radius * self.sphere_radius # sphere radius squared

            # Edge 1:
            point1 = closest_point_on_line_segment(self.v1, self.v2, self.player.position)
            v1 = self.player.position - point1
            distsq1 = dot(v1, v1)
            intersects = distsq1 < radiussq
            if (intersects):
                d = self.player.position - point1
                if (dot(d, d) < best_distsq):
                    best_distsq = dot(d, d)
                    best_point = point1
                    intersection_vec = pos - point1

            # Edge 2:
            point2 = closest_point_on_line_segment(self.v2, self.v3, self.player.position)
            v2 = self.player.position - point2
            distsq2 = dot(v2, v2)
            intersects = distsq2 < radiussq
            if (intersects):
                d = self.player.position - point2
                if (dot(d, d) < best_distsq):
                    best_distsq = dot(d, d)
                    best_point = point2
                    intersection_vec = pos - point2

            # Edge 3:
            point3 = closest_point_on_line_segment(self.v3, self.v1, self.player.position)
            v3 = self.player.position - point3
            distsq3 = dot(v3, v3)
            intersects = distsq3 < radiussq
            if (intersects):
                d = self.player.position - point3
                if (dot(d, d) < best_distsq):
                    best_distsq = dot(d, d)
                    best_point = point3
                    intersection_vec = pos - point3

        if (best_point != vec3(0,0,0)):
            self.l4 = Line3D(self.player.position, best_point)
            self.l4.color = vec3(1,0,0)
        else:
            print ("not intersecting")
        self.player.position -= intersection_vec
        self.active_camera.position = self.third_person_camera.get_position(self.active_camera, self.player.position)
        self.process_input(self.window)

        self.player.model_matrix = translate(mat4(1.0), self.player.position)

    def process_input(self, window):
        if (get_key(window, KEY_ESCAPE) == PRESS):
            set_window_should_close(self.window, True);

        speed = self.deltaTime * self.active_camera.MovementSpeed;
        total_velocity = vec3(0,0,0)
        if (get_key(window, KEY_W) == PRESS):
            total_velocity += self.active_camera.front
        if (get_key(window, KEY_S) == PRESS):
            total_velocity -= self.active_camera.front
        if (get_key(window, KEY_A) == PRESS):
            total_velocity -= self.active_camera.right
        if (get_key(window, KEY_D) == PRESS):
            total_velocity += self.active_camera.right

        self.player.velocity = total_velocity
        self.player.position += total_velocity * speed


        if (get_key(window, KEY_R) == PRESS):
            self.player.position = vec3(0,5,0)
            self.player.velocity = vec3(0,0,0)
            self.rotation_z = 0
            self.rotation_x = 0

        if (get_key(window, KEY_SPACE) == PRESS):
            self.player.velocity = vec3(0,5,0)

    def on_mouse_moved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos

        self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)


    def on_mouse_clicked(self, button, action, mods):
        if (button == 2 and action == 1):
            set_cursor_visible(self.window, False)
        if (button == 2 and action == 0):
            self.lastX = WIDTH/2
            self.lastY = HEIGHT/2

    def on_window_resized(self, width, height):
        pass

    def on_key_pressed(self, key, scancode, action, mods):
        if (key == KEY_V and action == 1):
            self.debug = True
        elif (key == KEY_V and action == 0):
            self.debug = False


app = App("character-controller", WIDTH, HEIGHT, False)
run(app)
