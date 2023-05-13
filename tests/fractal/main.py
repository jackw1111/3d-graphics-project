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
    def __init__(self, position, offset, position_offset, diameter):
        self.model = AnimatedObject("./data/ball.dae")
        self.model.shininess = 1.0
        self.model.color = vec4(1,1,1,1)
        self.model.set_to_draw = True
        self.model.position = position
        self.diameter = diameter
        self.model.model_matrix = scale(translate(mat4(1.0), self.model.position), vec3(self.diameter, self.diameter, self.diameter))
        self.offset = offset
        self.animated_position = position
        self.position_offset = position_offset

    def update_model_matrix(self, model):
        self.model.model_matrix = model * scale(translate(mat4(1.0), self.model.position), vec3(self.diameter, self.diameter, self.diameter))

def set_cylinder_to_line(cylinder, start_point, end_point):
    cylinder.model_matrix = mat4(1.0)
    dir_vec = start_point - end_point
    cylinder.model_matrix = scale(cylinder.model_matrix * inverse(lookAt((start_point + end_point) * 0.5, end_point, vec3(0,1,0))), vec3(cylinder.diameter, cylinder.diameter, 0.5*length(dir_vec))) 

class Cylinder():
    def __init__(self, start, end, draw=False):
        self.cylinder = AnimatedObject("./data/cylinder.dae")
        self.cylinder.shininess = 1.0
        self.cylinder.set_to_draw = draw
        self.cylinder.color = vec4(1,1,1,1)
        self.cylinder.model_matrix = mat4(1.0)
        self.cylinder.diameter = 0.3
    def set_endpoints(self, start, end):
        set_cylinder_to_line(self.cylinder, start, end)

    def update_model_matrix(self, model):
        self.cylinder.model_matrix = model * self.cylinder.model_matrix

def rotate_point(pt, angle):
    x,y = pt.x, pt.y
    #print (pt, pt.x, pt.y)

    _x = pt.x * math.cos(math.radians(angle)) - pt.y * math.sin(math.radians(angle))
    _y = pt.x * math.sin(math.radians(angle)) + pt.y * math.cos(math.radians(angle))
    return vec2(_x, _y)

class Root():
    allroots = []
    def __init__(self, start, end):
        self.startpoint = start
        self.currentpoint = start
        self.endpoint = end
        self.line = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.line.color = vec3(0,1,0)

    def found_duplicate(self, currentstartpoint, currentendpoint):
        for r in Root.allroots:
            if distance(currentstartpoint, r.startpoint) < 0.1 and distance(currentendpoint, r.endpoint) < 0.1:
                return True
        return False

    def update(self):
        #print ("a:",self.endpoint*(2.0/3.0), distance(self.currentpoint, self.endpoint*(2.0/3.0)))
        if distance(self.currentpoint, self.startpoint + (self.endpoint-self.startpoint) * (2.0 / 3.0)) < 0.1:
            if self.found_duplicate(self.currentpoint, self.endpoint):
                return

            m = mat4(1.0)
            m = translate(m, self.endpoint)
            m = rotate(m, math.radians(-60.0), vec3(1,0,0))

            point = m * vec4(self.endpoint.x, self.endpoint.y, self.endpoint.z, 1.0)
            point = vec3(point.x, point.y, point.z)
            #point = rotate_point(point, -60.0)
            #point = vec3(0.0, point.y, point.x) + self.startpoint + (self.endpoint-self.startpoint) * (2.0 / 3.0)
            self.r = Root(self.startpoint + (self.endpoint-self.startpoint) * (2.0 / 3.0), point)
            Root.allroots.append(self.r)

        if distance(self.currentpoint, self.endpoint) > 0.1:
            v = (self.endpoint-self.startpoint) * 0.005
            self.currentpoint += v
            self.line.set_endpoints(self.startpoint, self.currentpoint)

class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        make_context_current(self.window)
        set_cursor_visible(self.window, False)
        self.shadow_resolution = 2048
        #self.sky_box.load_skybox = False
        self.start_time = time.time()
        self.active_camera.set_far_plane(100.0)
        self.active_camera.position = vec3(0,0,5)
        self.active_camera.MovementSpeed = 20.0
        self.paused = False

        self.light = Light(vec3(0.9, 1.7, 1.8), vec3(1,1,1))
        self.light.position = vec3(3.109959, 8.199944, 6.099957)
        self.axis_3d = Axis3D()

        # self.pt0 = vec3(0,0,0)
        # self.pt1 = vec3(9,0,0)
        # self.pt2 = vec3(6,0,0)
        # self.pt3 = vec3(7,2,0)

        # self.points = [self.pt0, self.pt1, self.pt2, self.pt3]

        # self.balls = []

        # self.balls.append(Ball(self.pt0, 0.0, vec3(0,0,0), 0.3))
        # self.balls.append(Ball(self.pt1, 0.0, vec3(0,0,0), 0.3))
        # self.balls.append(Ball(self.pt2, 0.0, vec3(0,0,0), 0.3))
        # self.balls.append(Ball(self.pt3, 0.0, vec3(0,0,0), 0.3))

        # self.lines = []

        # c1 = Cylinder(self.balls[0].model.position, self.balls[1].model.position, True)
        # c2 = Cylinder(self.balls[2].model.position, self.balls[3].model.position, True)
        # c2.cylinder.diameter = 0.3
        # self.lines.append(c1)
        # self.lines.append(c2)

        glLineWidth(10)
        root = Root(vec3(0,0,0), vec3(0,0,10))
        Root.allroots.append(root)

        self.l4 = Line3D(vec3(0,5,0), vec3(0,5,0))
        self.l4.color = vec3(1,0,0)

        self.l5 = Line3D(vec3(0,5,0), vec3(0,5,0))
        self.l5.color = vec3(1,0,0)

    def update(self):
        self.process_input(self.window)
        for root in Root.allroots:
            root.update()


        # self.balls[1].model.position += vec3(0.1, 0.0, 0.0)
        # self.balls[1].update_model_matrix(mat4(1.0))
        # self.balls[3].model.position += vec3(0.1, 0.2, 0.0)
        # self.balls[3].update_model_matrix(mat4(1.0))
        # self.lines[0].set_endpoints(self.balls[0].model.position, self.balls[1].model.position)
        # self.lines[0].update_model_matrix(mat4(1.0))
        # self.lines[1].set_endpoints(self.balls[2].model.position, self.balls[3].model.position)
        # self.lines[1].update_model_matrix(mat4(1.0))

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
    app = App("fractal", WIDTH, HEIGHT, False, True)
    run(app)