import sys
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
#from console import *
from engine.graphics import *
from keys import *
from player import *
import random
import time
import math
from OpenGL.GL import *
from aa_bounding_box import *
from bounding_sphere import *
from axis_3d import *

WIDTH = 600
HEIGHT = 600

# TO DO load default skybox if no skybox in data/ and load_skybox != false

class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        set_cursor_visible(self.window, False)

        self.rect = Rect2D(vec2(100,100),vec2(100,100), "./data/cube.png",1,1)
        self.rect.ordering = 1
        self.rect.color = vec3(0,1,0)
        self.label = Label("text", vec2(100,100), "../minecraft/data/Minecraftia.ttf", 1)
        self.light = Light(vec3(-4, 8,-2), vec3(1,1,1))
        self.val = 0
        self.use_normal_map = True
        self.start_time = time.time()
        self.active_camera.MovementSpeed = 10.0
        self.active_camera.position = vec3(0.328692, -0.001445, -3.051301)
        self.active_camera.yaw = 45.0

        self.all_models = []
        #self.sky_box.load_skybox = False
        self.set_background_color(vec3(1.0, 1.0, 1.0))
        #self.house = StaticObject("data/house.obj")
        #self.p = self.house.model.meshes[7].vertices[0].Position
        #self.line = Line3D(self.active_camera.position, self.p)
        self.axis_3d = Axis3D()
        self.plane = StaticObject("data/plane.obj")
        self.plane.model_matrix = translate(mat4(1.0), vec3(0,3,0))
        self.plane.model_matrix = scale(self.plane.model_matrix, vec3(5,5,5))

        self.mountains = StaticObject("./data/cube.obj")
        # set fog color
        #set_clear_color(0.4, 0.2, 1.0)
        #set_clear_color(1.0, 0.0, 0.0)
        self._line = Line3D(self.active_camera.position+self.active_camera.up * 0.01, self.active_camera.position + self.active_camera.front * 1)
        self._line.color = vec3(0,1,0)
        self.x_val = 1.0
        self.y_val = 1.0
        self._offset = 0

    def update(self):

        self.x = 1
        self.shadow_map_center = vec3(0,0,0)
        #self.light.position = vec3(math.cos(time.time()), 3, math.sin(time.time()))
        self.process_input(self.window)


        for m in self.all_models:
            m.offset += 0.01
            m.set_frames(0.0, 1.11, m.offset)

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

        if (get_key(window, KEY_E) == PRESS):
            #for i in range(len(self.house.model.meshes)):
                #self.house.model.meshes[i].update_lighting(self.active_camera.position, self.active_camera.front, 0.0)
                #self.line.set_endpoints(self.p, self.active_camera.position)
            pass
        if (get_key(window, KEY_R) == PRESS):
            #for i in range(len(self.house.model.meshes)):
            pass

        if (get_key(window, KEY_Y) == PRESS):
            self.use_normal_map = True

        if (get_key(window, KEY_V) == PRESS):
            self.y_val += 1.0
        if (get_key(window, KEY_B) == PRESS):
            self.y_val -= 1.0

        if (get_key(window, KEY_N) == PRESS):
            self.x_val += 1.0
        if (get_key(window, KEY_M) == PRESS):
            self.x_val -= 1.0
    def on_mouse_moved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos
        self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)

    def on_mouse_clicked(self, button, action, mods):
        self.tmp_model.remove()
        self.val -= 3
    def on_window_resized(self, width, height):
        pass

    def on_key_pressed(self, key, scancode, action, mods):

        if (action == 1):
            if key == KEY_E:
                if (self.debug):
                    self.debug = False
                else:
                    self.debug = True
            if key == KEY_R:
                self._line.set_endpoints(self.active_camera.position, self.active_camera.position + self.active_camera.front * 15)

        #self.console.on_key_pressed(key, scancode, action, mods)
        if (action == 1):
            if (key == KEY_1):
                self.tmp_model = AnimatedObject("./data/astroboy.dae")
                self.tmp_model.shininess = 1.0
                self.tmp_model.color = vec3(random.randrange(0,2,1), random.randrange(0,2,1), random.randrange(0,2,1))
                self.tmp_transform = mat4(1.0)
                self.tmp_transform = rotate(translate(self.tmp_transform, vec3(self.val,3,0)), math.radians(-90.0), vec3(1,0,0))
                self.tmp_model.model_matrix = self.tmp_transform
                offset = random.uniform(0, 10)
                self.tmp_model.offset = offset
                self.tmp_model.set_frames(0.0, 1.0, self.tmp_model.offset)
                self.val += 3
                #self.tmp_model.draw_bounding_box = True
                self.all_models.append(self.tmp_model)

            elif (key == KEY_2):
                animations = [[2.85, 3.5],[3.65, 4.3],[5.32, 6],[4.5, 5.15],[0.2, 2.7]]
                animation = random.choice(animations)
                self.tmp_model = AnimatedObject("./data/player.fbx")
                self.tmp_transform = mat4(1.0)
                self.tmp_transform = scale(translate(mat4(1.0), vec3(self.val,3,0)), vec3(0.03, 0.03, 0.03))
                self.tmp_model.model_matrix = self.tmp_transform
                self.val += 3
                offset = random.uniform(0, 10)
                self.tmp_model.set_frames(animation[0], animation[1], offset)
                #self.all_models.append(self.tmp_model)

            elif (key == KEY_3):
                self.tmp_model = AnimatedObject("./data/steve.dae")
                self.tmp_transform = mat4(1.0)
                self.tmp_transform = rotate(scale(translate(mat4(1.0), vec3(self.val,7,0)), vec3(0.5, 0.5, 0.5)), math.radians(-90.0), vec3(1,0,0))
                self.tmp_model.model_matrix = self.tmp_transform
                self.val += 3
                offset = random.uniform(0, 10)
                self.tmp_model.set_frames(0.0, 1.0, offset)
                #self.all_models.append(self.tmp_model)


if __name__ == "__main__":
    app = App("example", WIDTH, HEIGHT, False)
    run(app)


