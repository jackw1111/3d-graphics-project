import sys
sys.path.append("/home/me/Documents/3d-graphics-project/engine/bin")
sys.path.append("/home/me/Documents/3d-graphics-project/engine/utils")

from engine.graphics import *
from keys import *
import random
import math

WIDTH = 1280
HEIGHT = 800

class HeliDemo(Application):
    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        set_cursor_visible(self.window, False)
        self.set_background_color(vec3(1.0, 1.0, 1.0))
        self.light = Light(vec3(-3,3,1), vec3(1,1,1))


        self.heli_position = vec3(-40.0,-37,-15)
        self.heli = AnimatedObject("./data/heli.dae")
        self.heli.set_frames(0.0, 5.0, 0.0)

        #self.heli.set_frames(0.0, 1.0, 0.0)
        self.map = StaticObject("./data/mountains.obj")
        #self.map.model_matrix = translate(self.map.model_matrix, vec3(21.2147, 30.0086, 28.7207) * -1.0)
        self.map.model_matrix = scale(self.map.model_matrix, vec3(400,200,400))

        #self.plane.set_to_draw = False
        #self.audio_window = AudioWindow()
        #self.audio_window.set_volume(0.4)
        #self.audio_window.play_audio("./data/heli.wav")

        #self.axis = StaticObject("data/3d_axis.obj")

        self.active_camera.yaw = 0

        self.heli.front = self.active_camera.front
        self.heli.yaw = 0.0
        self.heli.pitch = 0.0
        self.heli.right = self.active_camera.right
        self.heli.up = self.active_camera.up
        self.heli.position = vec3(0,5,0)
        self.heli.speed = 0.0
        self.heli.velocity = vec3(0,0,0)
        self.heli_max_speed = 10.0
        self.gravity = vec3(0,-10,0)
        self.third_person = True

    def update(self):

        self.processInput(self.window)
        front = vec3(0,0,0)
        front.x = math.cos(math.radians(self.heli.yaw)) * math.cos(math.radians(self.heli.pitch))
        front.y = math.sin(math.radians(self.heli.pitch))
        front.z = math.sin(math.radians(self.heli.yaw)) * math.cos(math.radians(self.heli.pitch))
        self.heli.front = normalize(front)
        self.heli.right = normalize(cross(self.heli.front, vec3(0,1,0))); 
        self.heli.up    = normalize(cross(self.heli.right, self.heli.front))


        dv = self.heli.up * self.deltaTime * self.heli.speed

        self.heli.velocity += dv
        self.heli.velocity += self.gravity * self.deltaTime
        if (length(self.heli.velocity) > self.heli_max_speed):
            self.heli.velocity = normalize(self.heli.velocity) * self.heli_max_speed
        self.heli.position += self.heli.velocity * self.deltaTime

        self.heli.velocity.x *= 0.9999
        self.heli.velocity.z *= 0.9999

        self.heli.model_matrix = translate(mat4(1.0), self.heli.position)

        if (not self.third_person):
            self.heli.model_matrix = translate(self.heli.model_matrix, self.heli.up*0.5)

        self.heli.model_matrix = rotate(self.heli.model_matrix, math.radians(-self.heli.yaw), self.heli.up)
        self.heli.model_matrix = rotate(self.heli.model_matrix, math.radians(self.heli.pitch), self.heli.right)
        self.heli.model_matrix = rotate(self.heli.model_matrix, math.radians(-90.0), vec3(1,0,0))

        t = self.currentFrame - int(self.currentFrame)
        self.heli.set_frames(0.0, 5.0, self.currentFrame)   

        if (self.third_person):
            modelHeight = 20.0
            ytheta = -self.active_camera.pitch
            # algorithm from ThinMatrix video on third person cameras
            horizDist = modelHeight * math.cos(math.radians(ytheta))
            vertDist = modelHeight * math.sin(math.radians(ytheta))
            xtheta = self.active_camera.yaw - 90.0
            offsetx = horizDist * math.sin(math.radians(-xtheta))
            offsetz = horizDist * math.cos(math.radians(xtheta))

            self.active_camera.position = self.heli.position + vec3(-offsetx, vertDist, -offsetz) + vec3(0,4,0)
        else:
            self.active_camera.position = self.heli.position - self.heli.up*0.5 + self.heli.front * 0.7

    def processInput(self, window):
        if (get_key(window, KEY_ESCAPE) == PRESS):
            set_window_should_close(self.window, True);

        if (get_key(window, KEY_W) == PRESS):
            #if (self.plane.pitch < 45.0):
            #self.heli.pitch -= 0.1
            self.heli.speed = 30.0

        if (get_key(window, KEY_S) == PRESS):
            #if (self.plane.pitch > -45.0):
            self.heli.speed = -30.0

        if (get_key(window, KEY_A) == PRESS):
            #if (self.plane.yaw < 45.0):
            #self.heli.yaw -= 1
            self.heli.yaw -= 1
            if (not self.third_person):
                self.active_camera.yaw += -1

        if (get_key(window, KEY_D) == PRESS):
            #if (self.plane.yaw > -45.0):
            #self.heli.yaw +=1
            self.heli.yaw += 1
            if (not self.third_person):
                self.active_camera.yaw -= -1

        if (get_key(window, KEY_W) != PRESS and get_key(window, KEY_S) != PRESS):
            self.heli.speed = 0.0

    def onMouseMoved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos

        if (not self.third_person):
            self.active_camera.pitch += -yoffset * self.deltaTime

        self.heli.pitch += -yoffset * self.deltaTime
        self.active_camera.yaw += xoffset * self.deltaTime * 3.0

        #self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)

    def onKeyPressed(self, key, scancode, action, mods):
        if (action == 1):
            if (key == KEY_F):
                if (self.third_person):
                    self.third_person = False
                else:
                    self.third_person = True
        if (key == KEY_V and action == 1):
            self.debug = True
        elif (key == KEY_V and action == 0):
            self.debug = False
        

if __name__ == "__main__":
    app = HeliDemo("HeliDemo", WIDTH, HEIGHT, True)
    run(app)

