import sys
sys.path.append("/home/me/Documents/3d-graphics-project/engine/bin")
sys.path.append("/home/me/Documents/3d-graphics-project/engine/utils")
from console import *
from axis_3d import *
from engine.graphics import *
#from engine.core.physics import collision_SAT
from keys import *
import random
import time
import math
from OpenGL.GL import *
from aa_bounding_box import *

WIDTH = 800
HEIGHT = 600

# circle drawing class
class Circle():

    def __init__(self, position=vec3(0,0,0), radius1=5, radius2=5):
        self.position = position
        self.radius1 = radius1
        self.radius2 = radius2
        self.points = []
        self.lines = []
        self.generate_circle()

    def generate_circle(self):
        # create points on circumference of circle
        for i in range(50):
            angle = math.radians(360.0/50.0 * i)
            x = self.radius1*math.cos(angle)
            y = self.radius2*math.sin(angle)
            point = self.position + vec3(x,0,y)
            self.points.append(point)

        self.end_points = []
        # join points with lines
        for i in range(50):
            start = i
            end = (i+1)%50 # loop back around at end of circle
            start_point = self.points[start]
            end_point = self.points[end]
            self.end_points.append([start_point, end_point])
            self.line = Line3D(start_point, end_point)
            self.line.color = vec3(0,0,1)
            self.lines.append(self.line)

    def set_position(self, position):
        x = position.x
        y = position.y
        z = position.z
        for i in range(50):
            line = self.lines[i]
            start_point = self.end_points[i][0] + vec3(x, 0, z)
            end_point = self.end_points[i][1] + vec3(x, 0, z)
            line.set_endpoints(start_point, end_point)

class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        set_cursor_visible(self.window, False)
        glLineWidth(1)
        self.show_ssao = False
        self.active_camera.MovementSpeed = 10.0
        self.start_time = time.time()
        self.light = Light(vec3(0,0,0), vec3(1,1,1))
        self.earth = StaticObject("./data/earth.obj")
        self.earth_orbit = Circle(vec3(0,0,0), radius1=24, radius2=16)
        self.moon = StaticObject("./data/moon.obj")
        self.moon.model_matrix = translate(mat4(1.0), vec3(0,0,6))
        self.moon_orbit = Circle(vec3(0,0,0), radius1=5, radius2=4)
        self.sun = StaticObject("./data/sun.obj")
        self.sun.model_matrix = scale(mat4(1.0), vec3(3,3,3))
        self.not_to_scale_label = Label("NOT TO SCALE", vec2(WIDTH-330, HEIGHT-50), "./data/default.ttf", 1)

    def update(self):
        
        line = Line3D(vec3(0,0,0), vec3(0,10,0))
        # get angle around circle given current frame
        scalar = 12
        angle = scalar * math.radians(self.currentFrame)

        # update position of earths orbit circle around the sun
        self.earth_pos = self.earth.model_matrix[3]
        x,y =24*math.cos(angle), 16*math.sin(angle)
        self.earth_pos = vec3(x,0,y)
        self.earth.model_matrix = translate(mat4(1.0), self.earth_pos)
        self.earth.model_matrix = rotate(self.earth.model_matrix, 50*angle, vec3(0,1,0))

        # update position of moons orbit circle around the earth
        self.moon_pos = self.moon.model_matrix[3]
        x,y =5*math.cos(12*angle), 4*math.sin(12*angle)
        self.moon_pos = self.earth_pos + vec3(x,0,y)
        self.moon.model_matrix = translate(mat4(1.0), self.moon_pos)
        self.moon_orbit.set_position(self.earth_pos)
        self.processInput(self.window)

        self.end_time = time.time()
        self.elapsed_time = self.end_time - self.start_time
        #print ("FPS =", 1.0/(self.deltaTime+0.0001))
 
    def processInput(self, window):
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
            self.show_ssao = False
            self.use_normal_map = False

        if (get_key(window, KEY_R) == PRESS):
            self.show_ssao = True


        if (get_key(window, KEY_Y) == PRESS):
            self.use_normal_map = True

    def onMouseMoved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos
        self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)

    def onMouseClicked(self, button, action, mods):
        pass

    def onWindowResized(self, width, height):
        pass

    def onKeyPressed(self, key, scancode, action, mods):
        #self.console.onKeyPressed(key, scancode, action, mods)
        pass
if __name__ == "__main__":
    app = App("solar-system", WIDTH, HEIGHT, False)
    run(app)
