#!/usr/bin/python

import sys,os
sys.path.append("/home/me/Documents/3d-graphics-project/engine/bin")
sys.path.append("/home/me/Documents/3d-graphics-project/engine/utils")
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
        self.stadium = StaticObject("./data/stadium.obj")
        self.collision_mesh = StaticObject("./data/collision_mesh.obj")
        self.collision_mesh.set_to_draw = False
        self.earth = AnimatedObject("./data/rl_ball.dae")
        self.earth.position = vec3(0,5,0)
        self.earth.velocity = vec3(0,0,0)
        self.gravity = vec3(0,-10,0)
        self.plane1 = StaticObject("../example/data/plane.obj")
        self.plane1.position = vec3(0,0,0)

        self.car = AnimatedObject("./data/car.dae")
        self.car.front = self.active_camera.front
        self.car.yaw = 0.0
        self.car.pitch = 0.0
        self.car.wheel_yaw = 0.0
        self.car.wheel_pitch = 0.0
        self.car.wheel_front = vec3(0,0,0)

        self.car.right = self.active_camera.right
        self.car.up = self.active_camera.up
        self.car.position = vec3(0,0.2,0)
        self.car.speed = 0.0
        self.car.velocity = vec3(0,0,0)
        self.car_max_speed = 10.0
        self.car.wheel_normal = vec3(0,0,0)
        self.gravity = vec3(0,-10,0)

        self.set_background_color(vec3(0,0,0))
        self.light = Light(vec3(-4, 8,-2), vec3(1,1,1))
        self.earth.translation_matrix = mat4(1.0)
        self.earth.rotation_matrix = mat4(1.0)
        self.active_camera.position = vec3(0,3,20)
        self.rotation_z = 0
        self.rotation_x = 0
        self.set_far_plane(1000.0)
        self.offset = 0
        self.car.set_frames(0.0, 2.0, 1.2)   

        self.map_position =self.collision_mesh.model_matrix
        collision_objects = [self.map_position]
        v = [self.collision_mesh.model]
        self.entity = CharacterEntity(v, collision_objects, vec3(1.0, 1.0, 1.0))
        self.entity.position = vec3(0,5,0)

        self.car_entity = CharacterEntity(v, collision_objects, vec3(1.0, 1.0, 1.0))
        self.car_entity.position = vec3(3,5,0)
        self.forward = False
        self.backward = False

        self.rotation_z = 0
        self.rotation_x = 0
        self.car_speed = 15
        self.jump = vec3(0,0,0)
    def update(self):

        #self.shadow_map_center = self.earth.position
        self.processInput(self.window)
        front = vec3(0,0,0)
        front.x = math.cos(math.radians(self.car.yaw)) * math.cos(math.radians(self.car.pitch))
        front.y = math.sin(math.radians(self.car.pitch))
        front.z = math.sin(math.radians(self.car.yaw)) * math.cos(math.radians(self.car.pitch))
        self.car.front = normalize(front)
        self.car.right = normalize(cross(self.car.front, vec3(0,1,0))); 
        self.car.up    = normalize(cross(self.car.right, self.car.front))
        #dv = self.car.up * self.deltaTime * self.heli.speed
        #self.car.velocity += dv
        #friction = self.car.velocity * -0.7
        #self.car.velocity += friction * self.deltaTime
        #if (length(self.car.velocity) > self.car_max_speed):
            #self.car.velocity = normalize(self.car.velocity) * self.car_max_speed
        #self.car.position += self.car.velocity * self.deltaTime


        self.car_entity.velocity.x *= 0.6
        self.car_entity.velocity.z *= 0.6
        self.car_entity.velocity += self.gravity * self.deltaTime * 0.1
        self.car_entity.velocity += self.jump * self.deltaTime * 0.3

        self.car_entity.update()

        if (self.entity.grounded):
            self.jump.y = 0

        if (self.jump.y > 0):
            self.jump.y -= self.deltaTime * 30
        else:
            self.jump.y = 0

        self.car.position = self.car_entity.position

        self.car.model_matrix = translate(mat4(1.0), self.car.position - vec3(0,1,0))
        self.car.model_matrix = rotate(self.car.model_matrix, math.radians(-self.car.yaw), vec3(0,1,0))

        #self.car.model_matrix = scale(self.car.model_matrix, vec3(3,3,3))
        #self.car.model_matrix = rotate(self.car.model_matrix, math.radians(-90.0), vec3(1,0,0))
        #self.car.model_matrix = angle_axis(math.radians(-self.car.yaw) ,vec3(0, 1, 0)) * self.car.model_matrix

        self.offset += self.deltaTime
        #print (self.offset)
        self.earth.translation_matrix = translate(mat4(1.0), self.earth.position)

        self.earth.model_matrix = self.earth.translation_matrix * self.earth.rotation_matrix

        self.plane1.normal_matrix = transpose(inverse(mat3_cast(self.plane1.model_matrix)))
        self.plane1.normal = vec3(0,1,0)
        self.plane1.normal = normalize(self.plane1.normal_matrix * self.plane1.normal)

        self.earth.velocity += self.gravity * self.deltaTime * 0.1
        #self.earth.position += self.earth.velocity * self.deltaTime
        #self.line = Line3D(vec3(0,0,0), self.plane.normal * 3.0)
        #self.line.color = vec3(1,0,0)


        self.entity.velocity = self.earth.velocity
        self.entity.update()
        self.earth.position = self.entity.position
        if (self.entity.grounded):
            #self.earth.velocity = self.earth.velocity * -1.0
            self.earth.velocity = reflect(self.earth.velocity, self.entity.colliding_normal) * 0.95
            self.rotation_z = self.rotation_z * 0.7 + self.entity.colliding_normal.z * 30
            self.rotation_x = self.rotation_x * 0.7 + self.entity.colliding_normal.x * 30
        self.earth.rotation_matrix = angle_axis(math.radians(self.rotation_z) ,vec3(1, 0, 0)) * self.earth.rotation_matrix
        self.earth.rotation_matrix = angle_axis(math.radians(-self.rotation_x) ,vec3(0, 0, 1))  * self.earth.rotation_matrix

        # t = sphere_plane_intersect(self.earth.position, 1.0, vec3(0,0,0), self.plane1.normal)
        # if (t < 0):
        #     self.earth.position -= self.plane1.normal * t
        #     self.earth.velocity = reflect(self.earth.velocity, self.plane1.normal) #TO DO restitution

        #     self.rotation_z = self.plane1.normal.z * 30
        #     self.rotation_x = self.plane1.normal.x * 30
        # self.earth.rotation_matrix = angle_axis(math.radians(self.rotation_z) ,vec3(1, 0, 0)) * self.earth.rotation_matrix
        # self.earth.rotation_matrix = angle_axis(math.radians(-self.rotation_x) ,vec3(0, 0, 1))  * self.earth.rotation_matrix

        #self.earth.model.meshes[0].bounding_cube.get_translated_vertices(self.earth.model_matrix)
        self.plane1.model.meshes[0].bounding_cube.get_translated_vertices(self.plane1.model_matrix)

        modelHeight = 10.0
        ytheta = -self.active_camera.pitch
        # algorithm from ThinMatrix video on third person cameras
        horizDist = modelHeight * math.cos(math.radians(ytheta))
        vertDist = modelHeight * math.sin(math.radians(ytheta))
        xtheta = self.active_camera.yaw - 90.0
        offsetx = horizDist * math.sin(math.radians(-xtheta))
        offsetz = horizDist * math.cos(math.radians(xtheta))

        self.active_camera.position = self.car.position + vec3(-offsetx, vertDist, -offsetz) + vec3(0,4,0)

        self.l1 = Line3D(self.car.position + vec3(0,2,0), self.car.position + self.car.wheel_front  + vec3(0,2,0))
        self.l1.color = vec3(0,1,0)            
        self.l2 = Line3D(self.car.position + vec3(0,2,0), self.car.position + self.car.wheel_normal  + vec3(0,2,0))
        self.l2.color = vec3(1,0,0)
        self.l3 = Line3D(self.car.position + vec3(0,2,0), self.car.position + self.car.front  + vec3(0,2,0))
        self.l3.color = vec3(0,0,1)
        #print ("intersect:", box_intersect_box(self.earth.model.meshes[0].bounding_cube, self.plane1.model.meshes[0].bounding_cube))

        if (distance(self.car.position, self.earth.position) < 2.0):
            self.earth.velocity += self.car.front * 0.3
        else:
            print ("not colliding!")

    def processInput(self, window):
        if (get_key(window, KEY_ESCAPE) == PRESS):
            set_window_should_close(self.window, True);


        if (get_key(window, KEY_SPACE) == PRESS):
            if (self.car_entity.grounded):
                if (self.jump == vec3(0,0,0)):
                    self.jump = vec3(0,20,0)

        if (get_key(window, KEY_W) == PRESS):
            self.car.yaw -= dot(self.car.front, self.car.wheel_front)
            #self.car.front += self.car.wheel_front
            self.car_entity.velocity += (self.car.front) * self.deltaTime * self.car_speed
            self.car.set_frames(0.0, 2.0, 1.2)   
            self.forward = True
            self.backward = False
        if (get_key(window, KEY_S) == PRESS):
            self.car_entity.velocity -= self.car.front * self.deltaTime * self.car_speed
            self.backward = True
            self.forward = False
        if (get_key(window, KEY_A) == PRESS):
            if (self.backward):
                self.car.yaw += 0.1 * self.car_speed
            else:
                self.car.yaw -= 0.1 * self.car_speed

            self.car.set_frames(0.0, 2.0, 0.9)   

        if (get_key(window, KEY_D) == PRESS):
            if (self.backward):
                self.car.yaw -= 0.1 * self.car_speed
            else:
                self.car.yaw += 0.1 * self.car_speed

            self.car.set_frames(0.0, 2.0, 1.4)   


        if (get_key(window, KEY_R) == PRESS):
            self.earth.position = vec3(0,5,0)
            self.earth.velocity = vec3(0,0,0)
            self.rotation_z = 0
            self.rotation_x = 0
        # else:
        #     if (get_key(window, KEY_W) == PRESS):
        #         self.earth.rotation_matrix = angle_axis(math.radians(-self.deltaTime*70) ,vec3(1, 0, 0)) * self.earth.rotation_matrix
        #         self.earth.position -= vec3(0, 0, self.deltaTime)

        #     if (get_key(window, KEY_S) == PRESS):
        #         self.earth.rotation_matrix = angle_axis(math.radians(self.deltaTime*70) ,vec3(1, 0, 0))  * self.earth.rotation_matrix
        #         self.earth.position += vec3(0, 0, self.deltaTime)


        #     if (get_key(window, KEY_A) == PRESS):
        #         self.earth.rotation_matrix = angle_axis(math.radians(self.deltaTime*70) ,vec3(0, 0, 1))  * self.earth.rotation_matrix
        #         self.earth.position -= vec3(self.deltaTime,0,0)

        #     if (get_key(window, KEY_D) == PRESS):
        #         self.earth.rotation_matrix = angle_axis(math.radians(-self.deltaTime*70) ,vec3(0, 0, 1))  * self.earth.rotation_matrix
        #         self.earth.position += vec3(self.deltaTime,0,0)


    def onMouseMoved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos

        #self.plane1.model_matrix = rotate(mat4(1.0), math.radians(self.lastY*0.01), vec3(1,0,0))
        #self.plane1.model_matrix = rotate(self.plane1.model_matrix, math.radians(-self.lastX*0.01), vec3(0,0,1))
        self.active_camera.pitch += yoffset * self.deltaTime * 3.0
        self.active_camera.yaw += xoffset * self.deltaTime * 3.0
        #self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)

    def onMouseClicked(self, button, action, mods):
        if (button == 2 and action == 1):
            set_cursor_visible(self.window, False)
        if (button == 2 and action == 0):
            self.lastX = WIDTH/2
            self.lastY = HEIGHT/2

    def onWindowResized(self, width, height):
        pass

    def onKeyPressed(self, key, scancode, action, mods):
        if (key == KEY_V and action == 1):
            self.debug = True
        elif (key == KEY_V and action == 0):
            self.debug = False


app = App("bouncing-ball", WIDTH, HEIGHT, False)
run(app)
