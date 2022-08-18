import sys
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from console import *
from axis_3d import *
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
        self.sky_box.load_skybox = False
        self.light = Light(vec3(0, 1, 4), vec3(1,1,1))

        self.start_time = time.time()


        self.frame_number = 0
        self.eco_trail = []
        self.eco_trail2 = []

        self.no_of_eco_particles = 500


        self.start_pos = vec3(0,0,0)

        self.active_camera.position = vec3(0,0,10)
        self.third_person_camera = ThirdPersonCamera(vec3(0,0,10), vec3(0,0,1), vec3(0,1,0), 0.0, -90.0)

    def update(self):

        self.process_input(self.window)

        self.end_time = time.time()
        self.elapsed_time = self.end_time - self.start_time

        self.set_background_color(vec3(math.sin(math.radians(self.elapsed_time)),math.sin(math.radians(-self.elapsed_time)),math.cos(math.radians(-self.elapsed_time))))
        self.active_camera.position = self.third_person_camera.get_position(self.active_camera, vec3(0,0,0))
        self.active_camera.yaw -= 0.3
        p = vec3(math.cos(100*math.radians(self.elapsed_time)),math.sin(math.radians(100*self.elapsed_time)),math.cos(math.radians(100*self.elapsed_time)))

        if (len(self.eco_trail) < self.no_of_eco_particles):
            self.particle6 = Particle("../particles/data/green_particle2_sprite_sheet.png", mat4(1.0),8,4)
            self.particle6.frame_number = random.randrange(1,32,1)
            self.particle6.blend_mode = GL_ONE

            _scale = random.uniform(1.0,4.0)
            self.particle6.model_matrix = scale(translate(mat4(1.0), p), vec3(_scale, _scale, _scale))
            self.particle6.alpha = random.uniform(0.05, 0.1)
            self.particle6.velocity = normalize(p)
            self.eco_trail.append(self.particle6)
        else:
            self.particle6 = self.eco_trail[0]
            self.particle6.frame_number = random.randrange(1,32,1)
            self.particle6.velocity = normalize(p)
            self.particle6.alpha = random.uniform(0.05, 0.1)
            _scale = random.uniform(1.0,4.0)

            self.particle6.model_matrix = scale(translate(mat4(1.0), p), vec3(_scale, _scale, _scale))
            self.eco_trail.append(self.particle6)
            self.eco_trail = self.eco_trail[1:]
        for particle in self.eco_trail:
            particle.row = int(particle.frame / 8)
            particle.col = particle.frame - 8 * particle.row
            particle.model_matrix = scale(translate(particle.model_matrix, particle.velocity * self.deltaTime), vec3(1, 1, 1))
            particle.frame += 1
            particle.alpha *= 0.99


        p = vec3(math.cos(100*math.radians(self.elapsed_time)),math.sin(math.radians(100*self.elapsed_time)),math.cos(math.radians(100*self.elapsed_time)))*-1

        if (len(self.eco_trail2) < self.no_of_eco_particles):
            self.particle7 = Particle("../particles/data/green_particle_sprite_sheet.png", mat4(1.0),8,4)
            self.particle7.frame_number = random.randrange(1,32,1)
            self.particle6.blend_mode = GL_ONE

            _scale = random.uniform(1.0,4.0)
            self.particle7.model_matrix = scale(translate(mat4(1.0), p), vec3(_scale, _scale, _scale))
            self.particle7.alpha = random.uniform(0.05, 0.1)
            self.particle7.velocity = normalize(p)
            self.eco_trail2.append(self.particle7)
        else:
            self.particle7 = self.eco_trail2[0]
            self.particle7.frame_number = random.randrange(1,32,1)
            self.particle7.velocity = normalize(p)
            self.particle7.alpha = random.uniform(0.05, 0.1)
            _scale = random.uniform(1.0,4.0)

            self.particle7.model_matrix = scale(translate(mat4(1.0), p), vec3(_scale, _scale, _scale))
            self.eco_trail2.append(self.particle7)
            self.eco_trail2 = self.eco_trail2[1:]
        for particle in self.eco_trail2:
            particle.row = int(particle.frame / 8)
            particle.col = particle.frame - 8 * particle.row
            particle.model_matrix = scale(translate(particle.model_matrix, particle.velocity * self.deltaTime), vec3(1, 1, 1))
            particle.frame += 1
            particle.alpha *= 0.99

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
        #self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)
        self.world_pos = ray_cast(self.lastX, self.lastY, self.active_camera.view_matrix, self.active_camera.projection_matrix, WIDTH, HEIGHT)
        #self.start_pos = self.active_camera + self.world_pos * 1.0

    def on_mouse_clicked(self, button, action, mods):
        pass

    def on_window_resized(self, width, height):
        pass

    def on_key_pressed(self, key, scancode, action, mods):
        #self.console.onKeyPressed(key, scancode, action, mods)
        pass
if __name__ == "__main__":
    app = App("test", WIDTH, HEIGHT, False, False)
    run(app)
