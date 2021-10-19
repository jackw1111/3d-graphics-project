import sys
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from console import *
from engine.graphics import *
#from engine.core.physics import collision_SAT
from keys import *
from player import *
import random
import time
import math
from OpenGL.GL import *
import threading

WIDTH = 800
HEIGHT = 600

def play_frames(frame_interval, current_time):
    start, end = frame_interval
    mantissa = current_time - int(current_time)
    return current_time % (end-start) + start

def get_position(mat1):
    v4 = mat1[3]
    return vec3(v4.x, v4.y, v4.z)

class Bullet():

    def __init__(self, pos, direction, original_time):
        self.bulletModel = StaticObject("./data/bullet_sphere.obj")
        self.original_time = original_time 
        self.speed = 400
        self.direction = direction
        self.position = pos
        self.bulletModel.model_matrix = translate(self.bulletModel.model_matrix, pos)
        self.bulletModel
    def update(self, elapsed_time):
        distance_scalar = elapsed_time - self.original_time
        self.bulletModel.model_matrix = translate(mat4(1.0), self.position + self.direction * self.speed * distance_scalar)

class App(Application):
    camera = Camera(vec3(0.0, 0.0, 10.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0), -90.0, 0.0)
    model = mat4(1.0)
    view = lookAt(vec3(0.0, 0.0, 10.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0))

    projection = mat4(1.0)
    deltaTime = 0.0
    lastFrame = 0.0
    currentFrame = 0.0

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        #set_cursor_visible(self.window, False)
        self.setup()

    def setup(self):
        self.console = Console(WIDTH, HEIGHT)
        self.player =  AnimatedObject("../example/data/player.fbx")
        self.planeModel = StaticObject("./data/plane.obj")
        self.player_model = scale(mat4(1.0), vec3(0.01, 0.01, 0.01))
        self.player.model_matrix = self.player_model 
        #self.planeModel2 = StaticObject("./data/plane2.obj")
        #self.planeModel2.model_matrix = translate(mat4(1.0), vec3(0, 0.1, 0))
        self.light = Light(vec3(0, 1, 4), vec3(1,1,1))
        self.frame_interval = [0.2, 2.7] # stand
        self.start_time = time.time()
        self.sky_box.load_skybox = True
        self.last_angle = 0
        self.active_camera.pitch = -35.0
        self.active_camera.yaw = -135.0
        self.player_speed = 200.0
        #self.fbo = FBO(WIDTH, HEIGHT, True, True, True, True)
        #self.fbo_shader = StaticShader()
        #self.fbo_shader.setup("../../data/fbo_shader.vs","../../data/fbo_shader.fs")
        self.bullets = []
        self.audio_window = AudioWindow()

    def draw(self):
        self.currentFrame  = time.time()
        self.deltaTime = (self.currentFrame - self.lastFrame)
        self.lastFrame = self.currentFrame
        self.end_time = time.time()
        self.elapsed_time = self.end_time - self.start_time
        self.light.position = vec3(3,5,0)
        self.console.update(self.currentFrame, self.deltaTime)
        self.active_camera.position = vec3(4.2, 8.5, 5)
        self.active_camera.view_matrix = lookAt(self.active_camera.position + get_position(self.player_model), get_position(self.player_model), vec3(0.0, 1.0, 0.0))

        v4 = get_position(self.player.model_matrix)
        player_position = vec3(v4.x, 2, v4.z)
        for bullet in self.bullets:
            bullet.update(self.elapsed_time)
            v = get_position(bullet.bulletModel.model_matrix)
            bullet_position = vec3(v.x, 2, v.z)
            if distance(bullet_position, player_position) > 10.0:
                bullet.bulletModel.set_to_draw = False
                self.bullets.remove(bullet)

        #self.shadow_map_center = self.active_camera.position + self.active_camera.front

    def update(self):
        self.processInput(self.window)
        self.draw()

    def processInput(self, window):
        t = self.currentFrame - int(self.currentFrame)
        any_key_pressed = False
        if (get_key(window, KEY_ESCAPE) == PRESS):
            set_window_should_close(self.window, True);
        if (get_key(window, KEY_W) == PRESS):
            self.active_camera.ProcessKeyboard(0, self.deltaTime)
            self.player.model_matrix = translate(self.player.model_matrix, vec3(0,0,self.player_speed * self.deltaTime))  
            self.player.set_frames(2.85, 3.5, t)      
            any_key_pressed = True

        if (get_key(window, KEY_S) == PRESS):
            self.active_camera.ProcessKeyboard(1, self.deltaTime)
            self.player.model_matrix = translate(self.player.model_matrix, vec3(0,0,-self.player_speed * self.deltaTime))

            self.player.set_frames(3.65, 4.3, t)      

            any_key_pressed = True

        if (get_key(window, KEY_A) == PRESS):
            self.active_camera.ProcessKeyboard(2, self.deltaTime)
            self.player.model_matrix = translate(self.player.model_matrix, vec3(self.player_speed * self.deltaTime,0,0))

            self.player.set_frames(5.32, 6, t)      

            any_key_pressed = True

        if (get_key(window, KEY_D) == PRESS):
            self.active_camera.ProcessKeyboard(3, self.deltaTime)
            self.player.model_matrix = translate(self.player.model_matrix, vec3(-self.player_speed * self.deltaTime,0,0))

            self.player.set_frames(4.5, 5.15, t)      

            any_key_pressed = True

        if (not any_key_pressed):
            self.player.set_frames(0.2, 2.7, self.currentFrame)      

            
    def onMouseMoved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos
        #self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)

        x = (2.0 * self.lastX) / WIDTH - 1.0
        y = 1.0 - (2.0 * self.lastY) / HEIGHT
        #print (math.degrees(math.atan(y/float(x+0.000001))))
        self.angle = math.degrees(math.atan(y/float(x+0.000001)))
        if (x < 0 and y > 0):
            self.angle = 180.0 + self.angle
        # 3rd quad
        elif (x < 0 and y < 0):
            self.angle = 180.0 + self.angle
        # 4th quadangle
        elif (x > 0 and y < 0):
            self.angle = 360.0 + self.angle
        #print (self.angle)

        self.change_of_angle = self.angle - self.last_angle
        self.last_angle = self.angle

        #print (self.change_of_angle)
        self.player.model_matrix = rotate(self.player.model_matrix, math.radians(self.change_of_angle), vec3(0,1,0))


    def onMouseClicked(self, button, action, mods):
        if (button == 0 and action == 1):
            self.audio_window.play("./data/player_shooting_one.wav")
            v4 = get_position(self.player.model_matrix)
            player_position = vec3(v4.x, 2, v4.z)               
            front = self.player.model_matrix[2]
            front = vec3(front.x, front.y, front.z)
            print (player_position)
            self.bullets.append(Bullet(player_position, front, self.elapsed_time))


    def onWindowResized(self, width, height):
        pass

    def onKeyPressed(self, key, scancode, action, mods):
        self.console.onKeyPressed(key, scancode, action, mods)

if __name__ == "__main__":
    app = App("angry-bots", WIDTH, HEIGHT, False)
    run(app)
