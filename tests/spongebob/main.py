#!/usr/bin/python

import sys,os
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from engine.graphics import *
from OpenGL.GL import *
from keys import *
from axis_3d import *
from third_person_camera import *
from engine.graphics import *
import random
import time
import math
from OpenGL.GL import *

WIDTH = 800
HEIGHT = 600

class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        set_cursor_visible(self.window, True)
        self.axid_3d = Axis3D()
        self.mesh = StaticObject("./data/mesh.obj")
        self.mesh.set_to_draw = False
        self.plane = StaticObject("./data/plane.obj")
        #self.plane.model_matrix = translate(self.plane.model_matrix, vec3(0,-2,0))
        #self.plane.model_matrix = scale(self.plane.model_matrix, vec3(10.0, 10.0, 10.0))
        self.spongebob = AnimatedObject("./data/spongebob2.dae")
        self.spongebob_arm = AnimatedObject("./data/spongebob2_arm.dae")
        self.third_person_camera = ThirdPersonCamera(vec3(0,0,0), vec3(0,1,0), vec3(0,0,1), 0.0, -90.0)
        self.third_person_camera.distance = 10.0
        self.all_jellyfish = []
        for i in range(10):
            x,y,z = random.randrange(-20,20,1), random.randrange(5, 7, 1), random.randrange(-20, 20, 1)
            self.jellyfish = AnimatedObject("./data/jellyfish2.dae")
            self.jellyfish.model_matrix = rotate(self.jellyfish.model_matrix, math.radians(90.0), vec3(0,1,0))
            self.jellyfish.model_matrix = translate(self.jellyfish.model_matrix, vec3(0,-1,0))
            self.jellyfish.model_matrix = translate(self.jellyfish.model_matrix, vec3(x,y,z))
            self.jellyfish.model_matrix = rotate(self.jellyfish.model_matrix, math.radians(-90.0), vec3(1,0,0))

            self.all_jellyfish.append(self.jellyfish)

        self.all_blocks_transforms = []
        self.all_blocks_transforms.append(self.plane.model_matrix)

        v = [self.plane.model]
        self.entity = CharacterEntity(v, self.all_blocks_transforms, vec3(0.7, 1.2, 0.7))
        self.entity.add_static_model(v, self.all_blocks_transforms)

        self.active_camera.MovementSpeed = 50.0

        self.pineapple = StaticObject("./data/pineapple.obj")
        self.pineapple.model_matrix = translate(mat4(1.0), vec3(5,-2.5,0))

        self.light = Light(vec3(0, 1, 4), vec3(1,1,1))
        self.mouse_down = False
        self.lines = []
        self.rot_axis = vec3(0,0,0)
        self.active_camera.position = vec3(0,0,5)
        self.a = 0.0
        self.t = 0
        self.third_person = True
        self.gravity = vec3(0,-10,0)
        self.y = 0
        self.jump = vec3(0,0,0)

        self.t2 = 0
        self.rotation_angle = 0
        self.key_pressed = False

        self.audio_window = AudioWindow()
        #self.audio_window.play("./data/theme.wav")
        #self.audio_window.set_volume("./data/theme.wav", 0.1)

    def update(self):
        self.process_input(self.window)

        rot_matrix = mat4(1.0)
        rot_matrix = rotate(rot_matrix, math.radians(90.0), vec3(1.0, 0.0, 0.0))
        rot_matrix = rotate(rot_matrix, math.radians(90.0+self.active_camera.yaw), vec3(0.0, 0.0, 1.0))
        #rot_matrix = rotate(rot_matrix, math.radians(self.active_camera.pitch), vec3(1.0, 0.0, 0.0))

        self.mesh.model_matrix = rot_matrix
        self.t += self.deltaTime*3
        self.t2 += self.deltaTime*2

        if (self.mouse_down):
            self.spongebob_arm.set_frames(8.0, 10.0, self.t2)   
        else:
            self.spongebob_arm.set_frames(0.0, 2.5, self.t2)   


        for i in range(len(self.all_jellyfish)):
            self.all_jellyfish[i].set_frames(0.0, 5.0, self.t + i)   

        self.entity.velocity.x *= 0.1
        self.entity.velocity.z *= 0.1
        self.entity.velocity.y *= 0.1
        self.entity.velocity  += self.gravity * self.deltaTime
        self.entity.velocity += self.jump * self.deltaTime
        self.entity.update()

        if (self.entity.grounded):
            self.jump.y = 0
            if (self.key_pressed):
                self.spongebob.set_frames(0.0, 5.0, self.t)   
            else:
                self.spongebob.set_frames(0.0, 5.0, 3.0)   
                self.spongebob_arm.set_frames(0.0, 5.0, 3.0)   

        if (self.jump.y > 0):
            self.jump.y -= self.deltaTime * 20
        else:
            self.jump.y = 0

        self.active_camera.position = self.third_person_camera.get_position(self.active_camera, self.entity.position)

        self.correct_orientation = translate(mat4(1.0), self.entity.position - vec3(0,0.5,0))
        self.correct_orientation = translate(self.correct_orientation, vec3(0,1,0))
        self.spongebob.model_matrix = rotate(self.correct_orientation, math.radians(self.rotation_angle - self.active_camera.yaw), vec3(0,1,0))

        self.spongebob.model_matrix = rotate(self.spongebob.model_matrix, math.radians(-90.0), vec3(1,0,0))

        self.spongebob_arm.model_matrix = scale(mat4(1.0), vec3(1,1,1))
        self.spongebob_arm.model_matrix = translate(mat4(1.0), vec3(0.0, 0.0, 0.9))

        self.spongebob_arm.model_matrix = translate(self.spongebob_arm.model_matrix, vec3(0,0,-0.9))

        self.spongebob_arm.model_matrix = rotate(self.correct_orientation * self.spongebob_arm.model_matrix, math.radians(self.rotation_angle  - self.active_camera.yaw), vec3(0,1,0))

        self.spongebob_arm.model_matrix = rotate(self.spongebob_arm.model_matrix, math.radians(-90.0), vec3(1,0,0))
        
    def process_input(self, window):
        self.key_pressed = False

        if (get_key(window, KEY_ESCAPE) == PRESS):
            set_window_should_close(self.window, True);

        speed = self.active_camera.MovementSpeed * self.deltaTime;
        if (get_key(window, KEY_W) == PRESS):
            self.entity.velocity += self.active_camera.front * speed
            self.rotation_angle = 0
            self.key_pressed = True

        if (get_key(window, KEY_S) == PRESS):
            self.entity.velocity += self.active_camera.front * -speed
            self.rotation_angle = 180
            self.key_pressed = True

        if (get_key(window, KEY_A) == PRESS):
            self.entity.velocity += self.active_camera.right * -speed
            self.rotation_angle = 90
            self.key_pressed = True

        if (get_key(window, KEY_D) == PRESS):
            self.entity.velocity += self.active_camera.right * speed
            self.rotation_angle = -90
            self.key_pressed = True
        if (get_key(window, KEY_R) == PRESS):
            self.entity.position = vec3(0,10,0)

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
            self.t2 = 0
            for i in range(len(self.all_jellyfish)):
                if distance(get_position(self.spongebob.model_matrix), get_position(self.all_jellyfish[i].model_matrix)) < 5.0:
                    self.all_jellyfish[i].model_matrix = scale(mat4(1.0), vec3(0,0,0))
                    print ("got here!")
        if (button == 0 and action == 0):
            self.mouse_down = False
            self.t2 = 0

    def on_window_resized(self, width, height):
        pass

    def on_key_pressed(self, key, scancode, action, mods):
        if (key == KEY_SPACE and action == 1):
            if (self.jump == vec3(0,0,0)):
                self.jump = vec3(0,20,0)

app = App("spongebob", WIDTH, HEIGHT, False, False)
run(app)
