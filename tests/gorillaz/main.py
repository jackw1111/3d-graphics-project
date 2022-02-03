#!/usr/bin/python

import sys,os
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from engine.graphics import *
from OpenGL.GL import *
from keys import *
from button import *
from bounding_sphere import *
from third_person_camera import *

from engine.graphics import *
import random
import time
import math
from OpenGL.GL import *

WIDTH = 512
HEIGHT = 384

class Car:
    def __init__(self, active_camera, _map):
        self.model = AnimatedObject("./data/geep2.dae")
        #self.set_to_draw = False
        self.front = active_camera.front
        self.yaw = 0.0
        self.pitch = 0.0
        self.right = active_camera.right
        self.up = active_camera.up
        #self.position = vec3(17.0, 308.0, 62.0)
        self.position = vec3(17.0, 50.0, 62.0)

        self.speed = 0.0
        self.velocity = vec3(0,0,0)
        self.max_speed = 10.0
        self.yaw = active_camera.yaw


        self.third_person = True
        self.third_person_camera = ThirdPersonCamera(vec3(0,0,0), vec3(0,0,-1), vec3(0,1,0), 0.0, -90.0)
        self.third_person_camera.distance = 15.0


        self.gravity = vec3(0,-10,0)

        self.t = 0


    def process_input(self, active_camera, window, deltaTime):

        speed = active_camera.MovementSpeed * deltaTime;
        if (get_key(window, KEY_W) == PRESS):
            self.velocity += self.front * speed
        if (get_key(window, KEY_S) == PRESS):
            self.velocity += self.front * -speed
        if (get_key(window, KEY_A) == PRESS):
            self.yaw -= 1
        if (get_key(window, KEY_D) == PRESS):
            self.yaw += 1


    def update(self, active_camera, deltaTime):
        self.t += deltaTime

        self.model.set_frames(0.0, 3.0, self.t) 

        if (self.third_person):
            active_camera.position = self.third_person_camera.get_position(active_camera, self.position)

        _front = vec3(0,0,0)
        _front.x = math.cos(math.radians(self.yaw)) * math.cos(math.radians(self.pitch))
        _front.y = math.sin(math.radians(self.pitch))
        _front.z = math.sin(math.radians(self.yaw)) * math.cos(math.radians(self.pitch))
        self.front = normalize(_front)
        self.right = normalize(cross(self.front, vec3(0,1,0))); 
        self.up    = normalize(cross(self.right, self.front))
        #self.velocity *= 0.1
        self.velocity += self.gravity * deltaTime
        self.position += self.velocity * deltaTime

        self.correct_orientation = translate(mat4(1.0), self.position)
        self.model_matrix = rotate(self.correct_orientation, math.radians(90-self.yaw), vec3(0,1,0))
        self.model_matrix = rotate(self.model_matrix, math.radians(-90.0), vec3(1,0,0))
        self.model_matrix = scale(self.model_matrix, vec3(3,3,3))
        self.model.model_matrix = self.model_matrix


class StartMenu:
    def __init__(self):

        self.bg = Rect2D(vec2(WIDTH/2, HEIGHT/2), vec2(WIDTH, HEIGHT), "./data/background.png",1,1)
        self.bg.ordering = 1
        self.play_button = Button("PLAY", vec2(100,0), vec2(150,90), "./data/button.png")
        self.play_button.rect.ordering = 2
        self.play_button.set_text_alignment(vec2(-50.0, 0))

    def update(self, lastX, lastY):
        if (self.play_button.cursor_in_bounds(lastX, HEIGHT - lastY)):
            self.play_button.rect.shaded = 1
        else:
            self.play_button.rect.shaded = 0

    def on_click(self, lastX, lastY):
        if (self.play_button.cursor_in_bounds(lastX, HEIGHT - lastY)):
            self.bg.to_draw = False
            self.play_button.rect.to_draw = False
            self.play_button.label.to_draw = False

class App(Application):

    def __init__(self, *args, **kwargs):

        Application.__init__(self, *args, **kwargs)
        set_cursor_visible(self.window, True)
        self.active_camera.set_far_plane(5000.0)
        self.background_color = vec3(1,1,0)
        self.light = Light(vec3(0, 1, 0), vec3(1,1,1))
        self.active_camera.position = vec3(0,0,5)
        self.active_camera.MovementSpeed = 75.0

        self.audio_window = AudioWindow()
        #self.audio_window.play("./data/19-2000.wav")
        #self.audio_window.set_volume("./data/19-2000.wav", 0.0)

        self.start_menu = StartMenu()

        self.map = StaticObject("./data/road4.obj")
        self.map.model_matrix = scale(self.map.model_matrix, vec3(100,100,100))

        self.car = Car(self.active_camera, self.map)

        self.mouse_down = False
        self.line = Line3D(vec3(0,0,0), vec3(0,1,0))
        self.line.color = vec3(1,0,0)
    def update(self):

        self.process_input(self.window)

        self.start_menu.update(self.lastX, self.lastY)
        self.car.update(self.active_camera, self.deltaTime)
        self.line.set_endpoints(self.car.position+vec3(2,0,3), self.car.position + vec3(2,-1,3))

        ray_dir = normalize(self.line.end_point - self.line.start_point)
        t =  ray_intersect_object(self.line.start_point, ray_dir, self.map)
        if (t == 0.0):
            self.car.position += vec3(0,5,0)
        print (t)
    def process_input(self, window):

        if (get_key(window, KEY_ESCAPE) == PRESS):
            set_window_should_close(self.window, True);

        self.car.process_input(self.active_camera, window, self.deltaTime)


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

        if (button == 0 and action == 1):
            self.mouse_down = True
            self.start_menu.on_click(self.lastX, self.lastY)

        if (button == 0 and action == 0):
            self.mouse_down = False

    def on_window_resized(self, width, height):
        pass

    def on_key_pressed(self, key, scancode, action, mods):
        pass

app = App("gorillaz", WIDTH, HEIGHT, False)
run(app)
