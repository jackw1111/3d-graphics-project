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

WIDTH = 800
HEIGHT = 600

class Ball():
    def __init__(self, position, offset, position_offset):
        self.model = AnimatedObject("./data/ball.dae")
        self.model.shininess = 1.0
        self.model.color = vec4(1,1,1,1)
        self.model.set_to_draw = True
        self.model.position = position
        self.model.model_matrix = scale(translate(mat4(1.0), self.model.position), vec3(0.3, 0.3, 0.3))
        self.offset = offset
        self.animated_position = position
        self.position_offset = position_offset

    def animate(self, t):
        y = 2*math.cos(t+self.offset+math.pi)
        z = 2*math.cos(t+self.offset)
        x = 5*math.sin(t+self.offset)/math.sqrt(1+0.2*z)
        p = self.position_offset + vec3(x, y, z)
        self.animated_position = p
        self.model.model_matrix = scale(translate(mat4(1.0), p), vec3(0.3, 0.3, 0.3))

    def update_model_matrix(self, model):
        self.model.model_matrix = model * self.model.model_matrix

def set_cylinder_to_line(cylinder, start_point, end_point):
    cylinder.model_matrix = mat4(1.0)
    dir_vec = start_point - end_point
    cylinder.model_matrix = scale(cylinder.model_matrix * inverse(lookAt((start_point + end_point) * 0.5, end_point, vec3(0,1,0))), vec3(0.3, 0.3, 0.5*length(dir_vec))) 

class Cylinder():
    def __init__(self, start, end, draw=False):
        self.cylinder = AnimatedObject("./data/cylinder.dae")
        self.cylinder.shininess = 1.0
        self.cylinder.set_to_draw = draw
        self.cylinder.color = vec4(1,1,1,1)

    def set_endpoints(self, start, end):
        set_cylinder_to_line(self.cylinder, start, end)

    def update_model_matrix(self, model):
        self.cylinder.model_matrix = model * self.cylinder.model_matrix

class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        make_context_current(self.window)
        set_cursor_visible(self.window, False)
        self.shadow_resolution = 2048
        self.msaa = False
        #self.sky_box.load_skybox = False
        self.start_time = time.time()
        self.active_camera.set_far_plane(100.0)
        self.active_camera.position = vec3(0,0,5)
        self.active_camera.MovementSpeed = 20.0
        self.paused = False

        self.plane = StaticObject("./data/plane.obj")
        self.plane.model_matrix = scale(translate(mat4(1.0), vec3(0,-10,0)), vec3(10,10,10))
        self.plane.model_matrix = rotate(self.plane.model_matrix, math.radians(45.0),vec3(1,0,-1))

        self.plane.normal_matrix = transpose(inverse(mat3_cast(self.plane.model_matrix)))
        self.plane.normal = vec3(0,1,0)
        self.plane.normal = normalize(self.plane.normal_matrix * self.plane.normal)

        self.light = Light(vec3(0.9, 1.7, 1.8), vec3(1,1,1))
        self.light.position = vec3(3.109959, 8.199944, 6.099957)

        #self.axis_3d = Axis3D()

        self.pt0 = vec3(-1,-1, 1)
        self.pt1 = vec3(-1,1, 1)
        self.pt2 = vec3(1,1, 1)
        self.pt3 = vec3(1,-1, 1)

        self.v = 4.72
        self.i1 = 1
        self.i2 = 6

        self.points = [self.pt0, self.pt1, self.pt2, self.pt3]

        self.balls = []

        for i,p in enumerate(self.points):
            self.balls.append(Ball(p, i*math.pi/2, vec3(-1,-2,-1)))

        for i,p in enumerate(self.points):
            self.balls.append(Ball(p, i*math.pi/2, vec3(-1,9,-1)))

        for i,p in enumerate(self.points):
            self.balls.append(Ball(p, i*math.pi/2, vec3(-1,-2,7)))

        for i,p in enumerate(self.points):
            self.balls.append(Ball(p, i*math.pi/2, vec3(-1,9,7)))

        self.lines = []

        for i in range(32):
            c = Cylinder(self.balls[0].model.position, self.balls[1].model.position, True)
            c.color = vec3(0,1,0)
            self.lines.append(c)

        self.x = 0

    def update(self):

        self.process_input(self.window)
        self.end_time = time.time()
        self.elapsed_time = self.end_time - self.start_time

        # if not self.paused:
        #     self.v += 0.01

        # draw ball corners
        for b in self.balls:
            b.animate(self.v)

        # draw connecting lines
        offset = vec3(0.01, 0, 0)
        self.lines[0].set_endpoints(self.balls[0].animated_position, self.balls[1].animated_position)
        self.lines[1].set_endpoints(self.balls[1].animated_position, self.balls[2].animated_position)
        self.lines[2].set_endpoints(self.balls[2].animated_position, self.balls[3].animated_position)
        self.lines[3].set_endpoints(self.balls[3].animated_position, self.balls[0].animated_position)

        self.lines[4].set_endpoints(self.balls[4].animated_position, self.balls[5].animated_position)
        self.lines[5].set_endpoints(self.balls[5].animated_position, self.balls[6].animated_position)
        self.lines[6].set_endpoints(self.balls[6].animated_position, self.balls[7].animated_position)
        self.lines[7].set_endpoints(self.balls[7].animated_position, self.balls[4].animated_position)

        self.lines[8].set_endpoints(self.balls[8].animated_position, self.balls[9].animated_position)
        self.lines[9].set_endpoints(self.balls[9].animated_position, self.balls[10].animated_position)
        self.lines[10].set_endpoints(self.balls[10].animated_position, self.balls[11].animated_position)
        self.lines[11].set_endpoints(self.balls[11].animated_position, self.balls[8].animated_position)

        self.lines[12].set_endpoints(self.balls[12].animated_position, self.balls[13].animated_position)
        self.lines[13].set_endpoints(self.balls[13].animated_position, self.balls[14].animated_position)
        self.lines[14].set_endpoints(self.balls[14].animated_position, self.balls[15].animated_position)
        self.lines[15].set_endpoints(self.balls[15].animated_position, self.balls[12].animated_position)

        self.lines[16].set_endpoints(self.balls[0].animated_position, self.balls[8].animated_position)
        self.lines[17].set_endpoints(self.balls[1].animated_position, self.balls[9].animated_position)
        self.lines[18].set_endpoints(self.balls[2].animated_position, self.balls[10].animated_position)
        self.lines[19].set_endpoints(self.balls[3].animated_position, self.balls[11].animated_position)

        self.lines[20].set_endpoints(self.balls[7].animated_position, self.balls[15].animated_position)
        self.lines[21].set_endpoints(self.balls[6].animated_position, self.balls[14].animated_position)
        self.lines[22].set_endpoints(self.balls[5].animated_position, self.balls[13].animated_position)
        self.lines[23].set_endpoints(self.balls[4].animated_position, self.balls[12].animated_position)

        self.lines[24].set_endpoints(self.balls[0].animated_position+offset, self.balls[4].animated_position)
        self.lines[25].set_endpoints(self.balls[9].animated_position+offset, self.balls[13].animated_position)
        self.lines[26].set_endpoints(self.balls[2].animated_position+offset, self.balls[6].animated_position)
        self.lines[27].set_endpoints(self.balls[8].animated_position+offset, self.balls[12].animated_position)
        self.lines[28].set_endpoints(self.balls[14].animated_position-offset, self.balls[10].animated_position)
        self.lines[29].set_endpoints(self.balls[1].animated_position+offset, self.balls[5].animated_position)
        self.lines[30].set_endpoints(self.balls[11].animated_position+offset, self.balls[15].animated_position)
        self.lines[31].set_endpoints(self.balls[3].animated_position+offset, self.balls[7].animated_position)

        self.tesseract_rotation_matrix = rotate(mat4(1.0), math.radians(self.x), normalize(self.light.position))
        self.x += 1
        for l in self.lines:
            l.update_model_matrix(self.tesseract_rotation_matrix)
        for b in self.balls:
            b.update_model_matrix(self.tesseract_rotation_matrix)

        #print (self.v)
        # print (self.light.position)
        # print (self.i1, self.i2)

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
        #self.console.on_key_pressed(key, scancode, action, mods)
        if (get_key(self.window, KEY_I) == PRESS):
            self.light.position += vec3(0,0.1, 0)
        if (get_key(self.window, KEY_K) == PRESS):
            self.light.position -= vec3(0,0.1, 0)     
        if (get_key(self.window, KEY_J) == PRESS):
            self.light.position += vec3(0.1,0, 0) 
        if (get_key(self.window, KEY_L) == PRESS):
            self.light.position -= vec3(0.1,0, 0)   
        if (get_key(self.window, KEY_0) == PRESS):
            self.paused = True
        if (get_key(self.window, KEY_1) == PRESS):
            self.paused = False
        if (get_key(self.window, KEY_2) == PRESS):
            self.i1 -= 1
        if (get_key(self.window, KEY_3) == PRESS):
            self.i1 += 1
        if (get_key(self.window, KEY_4) == PRESS):
            self.i2 -= 1
        if (get_key(self.window, KEY_5) == PRESS):
            self.i2 += 1     

        if (get_key(self.window, KEY_U) == PRESS):
            self.light.position -= vec3(0, 0, 0.1) 
        if (get_key(self.window, KEY_O) == PRESS):
            self.light.position += vec3(0, 0, 0.1) 

        if (get_key(self.window, KEY_R) == PRESS):
            self.v -= 0.01
        if (get_key(self.window, KEY_T) == PRESS):
            self.v += 0.01   

if __name__ == "__main__":
    app = App("tesseract", WIDTH, HEIGHT, False, True)
    run(app)
