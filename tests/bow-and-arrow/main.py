import sys
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from engine.graphics import *
from keys import *
import random
import time
import math
from OpenGL.GL import *
from third_person_camera import *

WIDTH = 800
HEIGHT = 600

class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        make_context_current(self.window)
        set_cursor_visible(self.window, False)
        self.light = Light(vec3(0, 100, 0), vec3(1,1,1))

        self.active_camera.position = vec3(0.0, 6.0, 0.0)
        self.map_model = StaticObject("./data/map.obj")
        self.map_model.model_matrix = translate(mat4(1.0), vec3(0,-3,0))
        self.crosshair = Rect2D(vec2(WIDTH/2, HEIGHT/2), vec2(25, 25), "./data/crosshair.png",1,1)
        self.crosshair.ordering = 1
        self.all_arrows = []
        self.bow = AnimatedObject("./data/bow.dae")
        self.bow.set_frames(0.0, 0.01, 0.0)      
        self.animation_time = 0
        self.lmb_down = False
        self.arrow_placeholder = StaticObject("./data/arrow.obj")
        self.character = AnimatedObject("./data/character.dae")
        self.character.model_matrix = translate(self.character.model_matrix, vec3(0,-2.5,0))
        self.character.model_matrix = scale(self.character.model_matrix, vec3(0.5, 0.5, 0.5))
        self.character.model_matrix = rotate(self.character.model_matrix, math.radians(-90.0), vec3(1,0,0))

        self.map_position =self.map_model.model_matrix
        collision_objects = [self.map_position]
        v = [self.map_model.model]
        self.entity = CharacterEntity(v, collision_objects, vec3(0.7, 1.2, 0.7))
        self.entity.add_static_model(v, collision_objects)
        self.gravity = vec3(0,-10,0)
        self.jump = vec3(0,0,0)
        self.t = 0
        self.player_position = vec3(0,0,0)
        self.player_velocity = vec3(0,0,0)

        self.label1 = Label2D("no intersect", vec2(100,100), "../minecraft/data/Minecraftia.ttf", 1)

        self.third_person_camera = ThirdPersonCamera(vec3(0,0,0), vec3(0,0,-1), vec3(0,1,0), 0.0, -90.0)
        self.third_person_camera.distance = 10.0
        self.third_person = False

    def update(self):

        self.process_input(self.window)

        #update all arrows
        for arrow_model in self.all_arrows:
            if (arrow_model.intersect_position != vec3(-1,-1,-1)):
                if (distance(arrow_model.start_position, arrow_model.intersect_position) < distance(arrow_model.start_position, arrow_model.position)):
                    continue
            #arrow_model.pitch -= 0.5
            arrow_model.velocity += self.gravity * self.deltaTime
            arrow_model.position += arrow_model.velocity * self.deltaTime
            arrow_model.model_matrix = translate(mat4(1.0), arrow_model.position)
            arrow_model.model_matrix = rotate(arrow_model.model_matrix, math.radians(arrow_model.yaw), vec3(0,1,0))
            arrow_model.model_matrix = rotate(arrow_model.model_matrix, math.radians(arrow_model.pitch), vec3(1,0,0))


        # update entity
        self.player_velocity += self.gravity * self.deltaTime
        self.player_position += self.player_velocity * self.deltaTime

        collision_info = sphere_intersect_object(self.player_position, self.player_velocity, self.deltaTime, 1.0, self.map_model)
        t = collision_info.depth
        normal = collision_info.normal
        #print (t)
        if (t <= 1.0 and t > 0.0):
            t2 = 1.0 - t
            self.collision_point = self.player_position - normal * t
            self.player_position += normal * t2
            self.player_velocity = vec3(0,0,0)
            self.player_velocity = reflect(self.player_velocity, normal)
        self.active_camera.position = self.player_position

        # update bow
        self.bow.model_matrix = translate(mat4(1.0), self.active_camera.position + self.active_camera.front*0.2 + self.active_camera.up * 0.11)
        self.bow.model_matrix = translate(self.bow.model_matrix, self.active_camera.right*0.1)
        self.bow.model_matrix = rotate(self.bow.model_matrix, math.radians(self.active_camera.pitch), self.active_camera.right)
        self.bow.model_matrix = rotate(self.bow.model_matrix, math.radians(-180-self.active_camera.yaw), vec3(0,1,0))
        self.bow.model_matrix = rotate(self.bow.model_matrix, math.radians(-90), vec3(1,0,0))
        self.bow.position = get_position(self.bow.model_matrix)
        self.bow.model_matrix = scale(self.bow.model_matrix, vec3(0.05, 0.05, 0.05))


        # update arrow
        self.arrow_placeholder.position = self.active_camera.position + self.active_camera.front * 0.5
        self.arrow_placeholder.yaw = -self.active_camera.yaw-90.0
        self.arrow_placeholder.pitch = self.active_camera.pitch
        self.arrow_placeholder.up = self.active_camera.up
        self.arrow_placeholder.right = self.active_camera.right

        if (self.lmb_down):
            self.bow.set_frames(0.0,10.0, self.animation_time)  
            if (self.animation_time < 2.4):    
                self.animation_time += self.deltaTime
                self.arrow_placeholder.position -= self.active_camera.front * self.animation_time * 0.1 


        self.arrow_placeholder.model_matrix = translate(mat4(1.0), self.arrow_placeholder.position + self.active_camera.front * 0.15 + self.active_camera.right * 0.2)
        self.arrow_placeholder.model_matrix = rotate(self.arrow_placeholder.model_matrix, math.radians(self.arrow_placeholder.yaw), vec3(0,1,0))
        self.arrow_placeholder.model_matrix = rotate(self.arrow_placeholder.model_matrix, math.radians(self.arrow_placeholder.pitch), vec3(1,0,0))


        self.t += self.deltaTime
        self.character.set_frames(0.0, 2.5, self.t)      

        if (self.third_person):
            if len(self.all_arrows):
                arrow = self.all_arrows[len(self.all_arrows)-1]
                self.active_camera.position = self.third_person_camera.get_position(self.active_camera, arrow.position)


    def process_input(self, window):
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

        self.player_position += normalize(total_velocity) * speed

    def on_mouse_moved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos
        self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)

        ray_wor = ray_cast(WIDTH/2, HEIGHT/2, self.active_camera.view_matrix, self.active_camera.projection_matrix, WIDTH, HEIGHT)
        t = ray_intersect_object(self.active_camera.position, self.active_camera.front, self.map_model)
        if (t):
            self.label1.text = "intersect!"
        else:
            self.label1.text = "no intersect!"

        # if (ray_intersect_sphere(self.entity.position, ray_wor, self.other_player.position, 2.0)):
        #     print ("player 2 hit!")
        #     self.other_player.color = vec3(1,0,0)
        #     self.other_player.set_frames(6.0, 8.0, 0.0)   
        # else:
        #     self.other_player.color = vec3(-1,-1,-1)
        #     self.other_player.set_frames(0.2, 2.7, 0.0)  

    def on_mouse_clicked(self, button, action, mods):

        if (button == 2 and action == 1):
            self.third_person = True
        else:
            self.third_person = False
        if (button == 0 and action == 1):
            self.lmb_down = True
            self.arrow_placeholder.set_to_draw = True
        if (button == 0 and action == 0):
            self.arrow_placeholder.set_to_draw = False
            self.lmb_down = False
            self.animation_time = 0
            self.bow.set_frames(0.0,10.0, self.animation_time)      

            self.arrow_model = StaticObject("./data/arrow.obj")
            self.arrow_model.speed = 20.0
            self.arrow_model.intersect_position = vec3(-1,-1,-1)

            #self.arrow_model.model_matrix = translate(mat4(1.0), self.active_camera.position)
            #self.arrow_model.model_matrix = rotate(self.arrow_model.model_matrix, math.radians(-self.active_camera.yaw-90.0), self.active_camera.up)
            #self.arrow_model.model_matrix = rotate(self.arrow_model.model_matrix, math.radians(self.active_camera.pitch), self.active_camera.right)
            t = 20
            self.arrow_model.intersect_position = self.active_camera.position + self.active_camera.front * t

            t = ray_intersect_object(self.active_camera.position, self.active_camera.front, self.map_model)
            if (t):
                self.arrow_model.intersect_position = self.active_camera.position + self.active_camera.front * t
            t =  ray_intersect_animated_object(self.active_camera.position, self.active_camera.front, self.character)
            if (t):
                self.arrow_model.intersect_position = self.active_camera.position + self.active_camera.front * t

            self.arrow_model.direction = normalize(self.arrow_model.intersect_position - self.bow.position)
            self.arrow_model.position = self.bow.position
            self.arrow_model.start_position = self.arrow_model.position
            self.arrow_model.velocity = self.arrow_model.direction * self.arrow_model.speed
            self.arrow_model.yaw = -self.active_camera.yaw-90.0
            self.arrow_model.pitch = self.active_camera.pitch
            self.arrow_model.up = self.active_camera.up
            self.arrow_model.right = self.active_camera.right
            self.all_arrows.append(self.arrow_model)

    def on_window_resized(self, width, height):
        pass

    def on_key_pressed(self, key, scancode, action, mods):
        if (key == KEY_ESCAPE and action == 1):
            set_window_should_close(self.window, True);
        if (key == KEY_SPACE and action == 1):
            if (self.jump == vec3(0,0,0)):
                self.jump = vec3(0,16,0)
        if (key == KEY_R and action == 1):
            self.player_position = vec3(0,2,0)

if __name__ == "__main__":
    app = App("bow-and-arrow", WIDTH, HEIGHT, False, False)
    run(app)
