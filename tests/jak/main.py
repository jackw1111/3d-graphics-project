import sys
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from console import *
from engine.graphics import *
from third_person_camera import *
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
        set_cursor_visible(self.window, False)
        self.audio_window = AudioWindow()
        #self.audio_window.play("/home/me/Documents/3d-graphics-project/tests/jak/data/sandover_village.wav")

        self.console = Console(WIDTH, HEIGHT)
        self.player =  AnimatedObject("./data/jak2.dae")
        self.player.position = vec3(0,10,0)
        #self.player.color = vec3(1,1,1)
        self.player_model = scale(mat4(1.0), vec3(1,1,1))
        self.player_model = rotate(self.player_model, math.radians(-90.0), vec3(1,0,0))
        self.player_model = translate(self.player_model, vec3(0,3,0))

        self.player.model_matrix = self.player_model 
        #self.player.draw_bounding_box = True
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
        self.bullets = []

        self.eco_trail = []
        self.no_of_eco_particles = 100

        self.eco_trail2 = []
        self.no_of_eco_particles2 = 100
        self.orbs = []
        for orb_count in range(10):
            self.orb = StaticObject("./data/metal_ellipsoid.obj")
            self.orbs.append(self.orb)

        self.i = 0

        self.power_cell = StaticObject("./data/power_cell.obj")

        self.ring = StaticObject("./data/ring.obj")

        self.ring.model_matrix = translate(mat4(1.0), vec3(0,4,0))

        self.eco_base = StaticObject("./data/eco_base.obj")
        self.eco_base.model_matrix = translate(mat4(1.0), vec3(-2,0,0))
        self.third_person = True
        self.third_person_camera = ThirdPersonCamera(vec3(0,0,0), vec3(0,0,-1), vec3(0,1,0), 0.0, -90.0)
        self.third_person_camera.distance = 15.0

        self.gravity = vec3(0,-10,0)
        self.y = 0
        self.jump = vec3(0,0,0)
        self.active_camera.MovementSpeed = 120
        v = self.player.bone_transforms

        self.orb_indicator = Rect2D(vec2(50,HEIGHT-50), vec2(100,100), "./data/orb_spritesheet.png",4,15)
        self.orb_indicator.frame = 1
        self.orb_indicator.ordering = 1

        self.orb_label = Label("1", vec2(100,HEIGHT-70), "../minecraft/data/Minecraftia.ttf", 1)
        self.orb_label.value = 1
        self.lastFrame = time.time()

    def update(self):

        self.orb_indicator.frame += 1
        self.process_input(self.window)
        self.i += 1
        self.ring.model_matrix = translate(mat4(1.0), vec3(0,4,0))
        self.ring.model_matrix = rotate(self.ring.model_matrix, radians(self.i), vec3(1,0,0))

        self.currentFrame  = time.time()
        self.player.set_frames(0.0, 2.5, self.currentFrame % 60)  

        self.deltaTime = (self.currentFrame - self.lastFrame)
        self.lastFrame = self.currentFrame
        self.end_time = time.time()
        self.elapsed_time = self.end_time - self.start_time
        self.light.position = vec3(3,5,0)
        self.console.update(self.currentFrame, self.deltaTime)
        self.active_camera.position = vec3(4.2, 8.5, 5)
        self.active_camera.view_matrix = lookAt(self.active_camera.position + get_position(self.player_model), get_position(self.player_model), vec3(0.0, 1.0, 0.0))

        if (len(self.eco_trail) < self.no_of_eco_particles):
            self.particle6 = Particle("../particles/data/green_particle2_sprite_sheet.png", mat4(1.0),8,4)
            self.particle6.frame_number = random.randrange(1,32,1)
            self.particle6.blend_mode = GL_ONE

            _scale = random.uniform(1.0,4.0)
            self.particle6.model_matrix = scale(translate(mat4(1.0), vec3(-2,0,0)), vec3(_scale, _scale, _scale))
            self.particle6.alpha = random.uniform(0.05, 0.1)
            self.eco_trail.append(self.particle6)
        else:
            self.particle6 = self.eco_trail[0]
            self.particle6.frame_number = random.randrange(1,32,1)
            self.particle6.alpha = random.uniform(0.05, 0.1)
            _scale = random.uniform(1.0,4.0)

            self.particle6.model_matrix = scale(translate(mat4(1.0), vec3(-2,0,0)), vec3(_scale, _scale, _scale))
            self.eco_trail.append(self.particle6)
            self.eco_trail = self.eco_trail[1:]

        for particle in self.eco_trail:
            particle.row = int(particle.frame_number / 8)
            particle.col = particle.frame_number - 8 * particle.row
            particle.model_matrix = scale(translate(particle.model_matrix, vec3(0,self.deltaTime*0.5,0)), vec3(1, 1, 1))
            particle.frame_number += 15.0 * self.deltaTime
            particle.alpha *= 0.99

        if (len(self.eco_trail2) < self.no_of_eco_particles2):
            self.particle6 = Particle("./data/white_light_sprite_sheet.png", mat4(1.0),8,4)
            self.particle6.frame_number = random.randrange(1,32,1)
            self.particle6.blend_mode = GL_ONE
            self.particle6.order = 0

            _scale = random.uniform(1.0,4.0)
            self.particle6.model_matrix = scale(translate(mat4(1.0), vec3(-6, 4, -5)), vec3(_scale, _scale, _scale))
            self.particle6.alpha = random.uniform(0.05, 0.1)
            self.eco_trail2.append(self.particle6)
        else:
            self.particle6 = self.eco_trail2[0]
            self.particle6.frame_number = random.randrange(1,32,1)
            self.particle6.alpha = random.uniform(0.05, 0.1)
            _scale = random.uniform(1.0,4.0)

            self.particle6.model_matrix = scale(translate(mat4(1.0), vec3(-6, 4, -5)), vec3(_scale, _scale, _scale))
            self.eco_trail2.append(self.particle6)
            self.eco_trail2 = self.eco_trail2[1:]

        for particle in self.eco_trail2:
            particle.row = int(particle.frame_number / 8)
            particle.col = particle.frame_number - 8 * particle.row
            v = vec3(random.randrange(-1,2,1),random.randrange(-1,2,1),random.randrange(-1,2,1))
            particle.model_matrix = scale(translate(particle.model_matrix, v*self.deltaTime*0.5), vec3(1, 1, 1))
            particle.frame_number += 15.0 * self.deltaTime
            particle.alpha *=0.99

        for z,orb in enumerate(self.orbs):
            orb.model_matrix = translate(mat4(1.0), vec3(8,4+0.2*math.sin(self.i/10),-12 + z*5))
            orb.model_matrix = rotate(orb.model_matrix, self.i/10, vec3(0,1,0))
        #self.power_cell.set_frames(0.0, 2.5, self.currentFrame % 60)  
        self.power_cell.model_matrix = translate(mat4(1.0), vec3(-6,8, -5))
        self.power_cell.model_matrix = rotate(self.power_cell.model_matrix, self.i/10, vec3(1,1,0))


        if (self.third_person):
            self.active_camera.position = self.third_person_camera.get_position(self.active_camera, self.player.position)


        self.player.model_matrix = translate(mat4(1.0), self.player.position)
        self.player.model_matrix = scale(self.player.model_matrix, vec3(0.6, 0.6, 0.6))
        self.player.model_matrix = rotate(self.player.model_matrix, math.radians(-90.0), vec3(1,0,0))
        self.player.model_matrix = translate(self.player.model_matrix, vec3(0,3,0))

        for orb in self.orbs:
            if orb.set_to_draw and distance(get_position(orb.model_matrix), get_position(self.player.model_matrix)) < 2.0:
                orb.set_to_draw = False
                self.orb_label.value += 1
                self.orb_label.text = str(self.orb_label.value)
                #self.audio_window.play("/home/me/Documents/3d-graphics-project/tests/jak/data/appear-online.wav")
                #self.audio_window.set_volume("/home/me/Documents/3d-graphics-project/tests/jak/data/appear-online.wav", 0.05)

    def process_input(self, window):
        t = self.currentFrame - int(self.currentFrame)
        any_key_pressed = False
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
        total_velocity.y = 0.0
        self.player.velocity = normalize(total_velocity) * speed  

    def on_mouse_moved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos
        #self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)

        # x = (2.0 * self.lastX) / WIDTH - 1.0
        # y = 1.0 - (2.0 * self.lastY) / HEIGHT
        # #print (math.degrees(math.atan(y/float(x+0.000001))))
        # self.angle = math.degrees(math.atan(y/float(x+0.000001)))
        # if (x < 0 and y > 0):
        #     self.angle = 180.0 + self.angle
        # # 3rd quad
        # elif (x < 0 and y < 0):
        #     self.angle = 180.0 + self.angle
        # # 4th quadangle
        # elif (x > 0 and y < 0):
        #     self.angle = 360.0 + self.angle
        # #print (self.angle)

        # self.change_of_angle = self.angle - self.last_angle
        # self.last_angle = self.angle

        # #print (self.change_of_angle)
        # self.player.model_matrix = rotate(self.player.model_matrix, math.radians(self.change_of_angle), vec3(0,0,1))

        self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)

    def on_mouse_clicked(self, button, action, mods):
        del self.orbs[0]

    def on_window_resized(self, width, height):
        pass

    def on_key_pressed(self, key, scancode, action, mods):
        self.console.on_key_pressed(key, scancode, action, mods)

if __name__ == "__main__":
    app = App("jak", WIDTH, HEIGHT, False)
    run(app)
