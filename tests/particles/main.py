import sys
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from engine.graphics import *
from keys import *
import random
import time
import math
from OpenGL.GL import *

WIDTH = 800
HEIGHT = 600


class App(Application):


    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        make_context_current(self.window)
        set_cursor_visible(self.window, False)
        self.active_camera.position = vec3(0.0, 0.0, 10.0)
        self.light = Light(vec3(0, 100, 0), vec3(1,1,1))
        self.sky_box.load_skybox = True
        self.wood_fire = StaticObject("./data/wood.obj")
        self.wood_fire.model_matrix = translate(mat4(1.0), vec3(0.0, -1, 0.0))
        
        self.set_background_color(vec3(1.0, 1.0, 1.0))


        self.particle1 = Particle("./data/smoke_atlas.png", translate(scale(mat4(1.0), vec3(5,5,5)), vec3(0,0.1,0)),8,8)
        self.particle1.frame = 0
        self.particle2 = Particle("./data/smoke_atlas.png", translate(scale(mat4(1.0), vec3(3,3,3)), vec3(0,0,0.05)),8,8)
        self.particle2.frame = 0

        self.particle3 = Particle("./data/fire_texture_atlas.png", translate(scale(mat4(1.0), vec3(3,3,3)), vec3(0,0.12,0.1)),8,4)
        self.particle3.frame = 0

        self.particle4 = Rect2D(vec2(WIDTH-50,50), vec2(100,100), "./data/loading_spritesheet.png",12,4)
        self.particle4.frame = 1

        self.particle5 = Particle("./data/green_particle_sprite_sheet.png", translate(mat4(1.0), vec3(6,3,3)),8,4)
        self.particle5.frame = 0

        #self.r = Rect(vec2(50,50), vec2(50,50), "./data/sparkle.png")
        #print (self.particle)

        self.smoke_trail = []
        self.no_of_smoke_puffs = 100

        self.eco_trail = []
        self.no_of_eco_particles = 100

    def update(self):
        self.process_input(self.window)

        self.particle1.frame += 1

        self.particle2.frame += 1

        self.particle3.frame += 1

        self.particle4.frame += 1

        self.particle5.frame += 1
        self.particle5.blend_mode = GL_ONE
        #self.fire_particles.update(self.active_camera, self.currentFrame)  

        if (len(self.eco_trail) < self.no_of_eco_particles):
            self.particle6 = Particle("./data/green_particle_sprite_sheet.png", mat4(1.0),8,4)
            self.particle6.frame = random.randrange(1,32,1)
            self.particle6.blend_mode = GL_ONE

            _scale = random.uniform(1.0,4.0)
            self.particle6.model_matrix = scale(translate(mat4(1.0), vec3(-10,0,0)), vec3(_scale, _scale, _scale))
            self.particle6.alpha = random.uniform(0.05, 0.1)
            self.eco_trail.append(self.particle6)
        else:
            self.particle6 = self.eco_trail[0]
            self.particle6.frame = random.randrange(1,32,1)
            self.particle6.alpha = random.uniform(0.05, 0.1)
            _scale = random.uniform(1.0,4.0)

            self.particle6.model_matrix = scale(translate(mat4(1.0), vec3(-10,0,0)), vec3(_scale, _scale, _scale))
            self.eco_trail.append(self.particle6)
            self.eco_trail = self.eco_trail[1:]

        for particle in self.eco_trail:
            particle.model_matrix = scale(translate(particle.model_matrix, vec3(0,self.deltaTime*0.5,0)), vec3(1, 1, 1))
            particle.frame += 1
            particle.alpha *=0.99

        if (len(self.smoke_trail) < self.no_of_smoke_puffs):
            self.particle1 = Particle("./data/smoke_atlas.png", mat4(1.0),8,8)
            self.particle1.frame = random.randrange(1,64,1)
            _scale = random.uniform(1.0,4.0)
            self.particle1.model_matrix = scale(translate(mat4(1.0), vec3(10,0,0)), vec3(_scale, _scale, _scale))
            self.particle1.alpha = random.uniform(0.05, 0.1)
            self.smoke_trail.append(self.particle1)
        else:
            self.particle1 = self.smoke_trail[0]
            self.particle1.frame = random.randrange(1,64,1)
            self.particle1.alpha = random.uniform(0.05, 0.1)
            _scale = random.uniform(1.0,4.0)

            self.particle1.model_matrix = scale(translate(mat4(1.0), vec3(10,0,0)), vec3(_scale, _scale, _scale))
            self.smoke_trail.append(self.particle1)
            self.smoke_trail = self.smoke_trail[1:]

        for particle in self.smoke_trail:
            particle.model_matrix = scale(translate(particle.model_matrix, vec3(0,self.deltaTime*0.5,0)), vec3(1+self.deltaTime*0.1, 1+self.deltaTime*0.1, 1+self.deltaTime*0.1))
            particle.frame += 1
            particle.alpha *=0.99

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
        pass            

if __name__ == "__main__":
    app = App("test", WIDTH, HEIGHT, False)
    run(app)
