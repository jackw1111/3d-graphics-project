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

WIDTH = 1280
HEIGHT = 720

class Ball():
    def __init__(self, position, offset, position_offset, file):
        self.model = StaticObject(file)
        self.model.shininess = 1.0
        self.model.set_to_draw = True
        #self.model.draw_bounding_box = True
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

def set_cylinder_to_line(cylinder, start_point, end_point):
    cylinder.model_matrix = mat4(1.0)
    dir_vec = start_point - end_point
    cylinder.model_matrix = scale(cylinder.model_matrix * inverse(lookAt((start_point + end_point) * 0.5, end_point, vec3(0,1,0))), vec3(0.3, 0.3, 0.5*length(dir_vec))) 

class Cylinder():
    def __init__(self, start, end, file, draw=False):
        self.cylinder = StaticObject(file)
        self.cylinder.shininess = 1.0
        self.cylinder.set_to_draw = draw
        #self.cylinder.draw_bounding_box = True
        self.start = start
        self.end = end
    def set_endpoints(self, start, end):
        self.start = start
        self.end = end
        set_cylinder_to_line(self.cylinder, self.start, self.end)

class Pipeline():
    def __init__(self, file1, file2, offset=vec3(0,0,0)):
        self.file1 = file1
        self.file2 = file2
        self.pipes = []
        self.c = Cylinder(vec3(0.1, 0.0, 0.0)+offset,vec3(0.0, 1.0, 0.0)+offset, self.file2, True)
        self.c.color = vec3(0,1,0)
        self.c.set_endpoints(vec3(0.0, 0.0, 0.01)+offset, vec3(0.01, 1.0, 0.0)+offset)
        self.pipes.append(self.c)

        b1 = Ball(vec3(0.01, 1.0, 0.0)+offset, 0.0, vec3(0,0,0), self.file1)
        b2 = Ball(vec3(0.0, 0.0, 0.01)+offset, 0.0, vec3(0,0,0), self.file1)

        self.pipe_ends = []
        self.pipe_ends.append(b1)
        self.pipe_ends.append(b2)

        self.start_time = time.time()

    def update(self):

        self.end_time = time.time()
        self.elapsed_time = self.end_time - self.start_time

        if (self.elapsed_time > 0.1):
            self.start_time = time.time()
            self.end_time = time.time()

            last_pipe = self.pipes[len(self.pipes)-1]
            directions = [vec3(1,0.001,-0.001), vec3(-0.001,1,0.001), vec3(0.001,-0.001,1), vec3(-1,0.001,-0.001), vec3(-0.001,-1,0), vec3(-0.001,0.001,-1)]
            while True:
                d = random.choice(directions) * random.randrange(-4,4,1)
                s1 = last_pipe.start
                e1 = last_pipe.end
                s2 = last_pipe.end
                e2 = last_pipe.end + d
                if dot(normalize(e1 - s1), normalize(e2 - s2)) >= -0.95:
                    break
            p = Cylinder(vec3(0,0,0), vec3(0,0,0), self.file2, True)
            p.set_endpoints(s2, e2)
            self.pipes.append(p)
            b = Ball(e2, 0.0, vec3(0,0,0), self.file1)
            self.pipe_ends.append(b)


class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        make_context_current(self.window)
        set_cursor_visible(self.window, False)
        self.msaa = False
        self.sky_box.load_skybox = False
        self.active_camera.set_far_plane(50.0)
        self.active_camera.position = vec3(0,0,5)
        self.active_camera.MovementSpeed = 20.0
        self.paused = False

        self.light = Light(vec3(0.9, 1.7, 1.8), vec3(1,1,1))
        self.light.position = vec3(3.109959, 8.199944, 6.099957)

        self.pipeline1 = Pipeline("./data/red_ball.dae", "./data/red_cylinder.dae")
        self.pipeline2 = Pipeline("./data/blue_ball.dae", "./data/blue_cylinder.dae", vec3(3,4,1))
        self.pipeline3 = Pipeline("./data/ball.dae", "./data/cylinder.dae", vec3(-3,3,-2))
        self.pipeline4 = Pipeline("./data/green_ball.dae", "./data/green_cylinder.dae", vec3(-3,-4,-2))

        #self.axis = Axis3D()

    def update(self):

        self.process_input(self.window)

        self.pipeline1.update()
        self.pipeline2.update()
        self.pipeline3.update()
        self.pipeline4.update()

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
        pass

if __name__ == "__main__":
    app = App("pipes", WIDTH, HEIGHT, True, True)
    run(app)
