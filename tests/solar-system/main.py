import sys
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
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
from line_circle import *

WIDTH = 800
HEIGHT = 600

class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        set_cursor_visible(self.window, False)
        glLineWidth(1)
        self.active_camera.MovementSpeed = 10.0
        self.light = Light(vec3(0,0,0), vec3(1,1,1))
        self.earth = StaticObject("./data/earth.obj")
        self.earth_orbit = Circle(vec3(0,0,0), radius1=24, radius2=16)
        for l in self.earth_orbit.lines:
            l.color = vec3(0.5,0.5,0.5)
        self.earth_orbit2 = Circle(vec3(0,0,0), radius1=24, radius2=16)

        self.earth_orbit2_rotation = mat4(1.0)
        self.earth_orbit2_rotation = translate(self.earth_orbit2_rotation, vec3(10,0,0))

        for i in range(len(self.earth_orbit2.lines)):
            l = self.earth_orbit2.lines[i]

            p1 = l.start_point
            p2 = l.end_point
            p1 = self.earth_orbit2_rotation * vec4(p1.x, p1.y, p1.z, 1.0)
            p1 = vec3(p1.x, p1.y, p1.z)
            l.start_point = p1

            p2 = self.earth_orbit2_rotation * vec4(p2.x, p2.y, p2.z, 1.0)
            p2 = vec3(p2.x, p2.y, p2.z)
            l.end_point = p2

        self.moon = StaticObject("./data/moon.obj")
        self.moon.model_matrix = translate(mat4(1.0), vec3(0,0,6))
        self.moon_orbit = Circle(vec3(0,0,0), up=vec3(0,1,0), radius1=5, radius2=4)
        self.sun = StaticObject("./data/sun.obj")
        self.sun_pos = vec3(0,0,0)
        self.sun.model_matrix = scale(mat4(1.0), vec3(3,3,3))

        self.axis_3d = Axis3D()
        self.centre = vec3(24,0,0)
        self.axis_3d.object.model_matrix = translate(mat4(1.0), self.centre)

        self.axis_3d2 = Axis3D()
        self.axis_3d2.object.model_matrix = scale(self.axis_3d2.object.model_matrix, vec3(3,3,3))
        self.axis_3d2.object.model_matrix = translate(self.axis_3d2.object.model_matrix, self.centre)

        self.active_camera.position = vec3(4,5,0)
        self.rot = 0.0
        self.rotation = translate(mat4(1.0),vec3(0,0,0))
        self.paused = False
        self.currentTime = 0
        self.year = 3114
        self.label = Label2D("21/12/" + str(self.year) + " BC", vec2(WIDTH-300,50), "../minecraft/data/Minecraftia.ttf", 0.6)
        self.epoch = "BC"

    def update(self):
        self.process_input(self.window)
        if (not self.paused):
            line = Line3D(vec3(0,0,0), vec3(0,10,0))
            # get angle around circle given current frame
            scalar = 120
            angle = scalar * math.radians(self.currentFrame)

            # update position of earths orbit circle around the sun
            self.earth_pos = self.earth.model_matrix[3]
            x,y =24*math.cos(angle), 16*math.sin(angle)
            self.earth_pos = vec3(x,0,y)
            self.earth.model_matrix = mat4(1.0)
            self.earth.model_matrix = translate(self.earth.model_matrix, self.earth_pos)
            self.earth.model_matrix = rotate(self.earth.model_matrix, 50*angle, vec3(0,1,0))
            self.earth.model_matrix =  self.rotation * self.earth.model_matrix
            self.earth_pos = self.earth.model_matrix[3]
            self.earth_pos = vec3(self.earth_pos.x,self.earth_pos.y,self.earth_pos.z)

            # update position of moons orbit circle around the earth
            self.moon_pos = self.moon.model_matrix[3]
            x,y =5*math.cos(12*angle), 4*math.sin(12*angle)
            self.moon_pos = self.earth_pos + vec3(x,0,y)
            self.moon.model_matrix = translate(mat4(1.0), self.moon_pos)
            self.moon_orbit.update(self.earth_pos, self.moon_orbit.up, self.moon_orbit.radius1, self.moon_orbit.radius2)

            #l.set_endpoints()
            alignment = dot(normalize(self.centre - self.sun_pos), normalize(self.earth_pos - self.sun_pos))
            alignment2 = dot(normalize(self.centre - self.sun_pos), normalize(self.moon_pos - self.sun_pos))
            
            if  alignment > 0.999:
                self.rot += 1.0
                if (self.year < 0 and self.epoch == "BC"):
                    self.year = 0
                    self.epoch = "AD"

                if self.epoch == "BC":
                    self.year -= 10
                    self.label.text = "21/12/" + str(self.year) + " BC"
                else:
                    self.year += 10
                    self.label.text = "21/12/" + str(self.year) + " AD"

                self.earth.color = vec4(0,1,0,1)
                self.moon.color = vec4(0,1,0,1)
                self.sun.color = vec4(0,1,0,1)

                # reverse prevous rotation
                for i in range(len(self.earth_orbit.lines)):
                    l = self.earth_orbit.lines[i]
                    p1 = l.start_point
                    p2 = l.end_point
                    p1 = inverse(self.rotation) * vec4(p1.x, p1.y, p1.z, 1.0)
                    p1 = vec3(p1.x, p1.y, p1.z)
                    l.start_point = p1

                    p2 = inverse(self.rotation) * vec4(p2.x, p2.y, p2.z, 1.0)
                    p2 = vec3(p2.x, p2.y, p2.z)
                    l.end_point = p2

                self.centre = inverse(self.rotation) * vec4(self.centre.x, self.centre.y, self.centre.z, 1.0)
                self.centre = vec3(self.centre.x, self.centre.y, self.centre.z)                

                self.rotation = mat4(1.0)
                self.rotation = rotate(self.rotation, math.radians(self.rot), vec3(0,1,0))
                self.rotation = translate(self.rotation, vec3(10,0,0))

                self.centre = self.rotation * vec4(self.centre.x, self.centre.y, self.centre.z, 1.0)
                self.centre = vec3(self.centre.x, self.centre.y, self.centre.z)

                self.axis_3d.object.model_matrix = translate(mat4(1.0), self.centre)


                # apply new rotation
                for i in range(len(self.earth_orbit.lines)):
                    l = self.earth_orbit.lines[i]

                    p1 = l.start_point
                    p2 = l.end_point
                    #rotation = rotate(mat4(1.0), math.radians(self.rot), vec3(0,1,0))
                    p1 = self.rotation * vec4(p1.x, p1.y, p1.z, 1.0)
                    p1 = vec3(p1.x, p1.y, p1.z)
                    l.start_point = p1

                    p2 = self.rotation * vec4(p2.x, p2.y, p2.z, 1.0)
                    p2 = vec3(p2.x, p2.y, p2.z)
                    l.end_point = p2

                    # p2 = vec4(p2.x, p2.y, p2.z, 1.0) * rotate(mat4(1.0), math.radians(self.rot), l.end_point)
                    # p1 = vec3(p1.x, p1.y, p1.z)
                    # p2 = vec3(p2.x, p2.y, p2.z)
                    #if (i == len(self.earth_orbit.lines)-1):
                        #print (l.start_point, l.end_point)
                    # l.set_endpoints(p1, p2)
                    # l.start_point = p1
                    # l.end_point = p2
                    #self.earth_orbit.lines[i].start_point = p1
                    #self.earth_orbit.lines[i].end_point = p2

            else:
                self.earth.color = vec4(0,0,0,1)
                self.moon.color = vec4(0,0,0,1)
                self.sun.color = vec4(0,0,0,1)
        #print ("FPS =", 1.0/(self.deltaTime+0.0001))
 
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
            self.show_ssao = False
            self.use_normal_map = False

        if (get_key(window, KEY_R) == PRESS):
            self.show_ssao = True

        if (get_key(window, KEY_P) == PRESS):
            self.paused = True

        if (get_key(window, KEY_Y) == PRESS):
            self.use_normal_map = True

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
    app = App("solar-system", WIDTH, HEIGHT, False, False)
    run(app)
