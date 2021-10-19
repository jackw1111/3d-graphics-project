#!/usr/bin/python

import sys,os
sys.path.append("/home/me/Documents/3d-graphics-project/engine/bin")
sys.path.append("/home/me/Documents/3d-graphics-project/engine/utils")
from engine.graphics import *
from OpenGL.GL import *
from keys import *
from engine.graphics import *
import random
import time
import math
from OpenGL.GL import *

WIDTH = 600
HEIGHT = 600

import png

def create_png(color):

    width = 255
    height = 255
    img = []
    for y in range(height):
        row = ()
        for x in range(width):
            row = row + (int(color.x * 255), int(color.y * 255), int(color.z * 255), 255)
        img.append(row)
    with open('./data/gradient.png', 'wb') as f:
        w = png.Writer(width, height, greyscale=False, alpha='RGBA')
        w.write(f, img)



def convert_vertices(vertices):
    converted = []
    for v in vertices:
        s = "v " + str(v.x) + " " + str(v.y) + " " + str(v.z) + "\n"
        converted += s
    return "".join(converted)

def convert_faces(vertices, indices=[]):

    converted = []
    i = 0
    for f in range(int(len(vertices)/3)):
        print (int(len(vertices)/3))
        s = "f "
        if (len(indices)):
            for j in range(3):
                i+=1
                s += str(indices[i]) + "/1/" + str(indices[i])
                if (j == 2):
                    s += '\n'
                else:
                    s += ' '
        else:      
            for j in range(3):
                i += 1
                s += str(i) + "/1/" + str(i)
                if (j == 2):
                    s += "\n"
                else:
                    s += " "

        converted += s

    return "".join(converted)



def create_obj_file(vertices, filename, indices=[]):
    contents = """
# Blender v2.79 (sub 0) OBJ File: ''
# www.blender.org
mtllib mesh.mtl
""" + convert_vertices(vertices) + """
vt 0.999900 0.000100
""" + convert_normals(vertices) + """
usemtl None
s off
""" + convert_faces(vertices, indices)


    with open(filename, 'w') as f:
        f.write(contents)

def convert_normals(vertices, reversed_normals=False):
    normals = []
    for f in range(0, int(len(vertices)/3), 3):
        for j in range(3):
            n = "vn "
            p1, p2, p3 = vertices[f*3], vertices[f*3 + 1], vertices[f*3 + 2]
            norm = cross(p2 - p1, p3 - p1)
            n += str(norm.x)
            n += " "
            n += str(norm.y)
            n += " "
            n += str(norm.z)
            n += '\n'
            normals.append(n)
    return "".join(normals)

def mesh_method1(equation, detail=2.5, xdomain=[-2,2], ydomain=[-2,2], reversed_normals=False):
    mesh = []
    detail = 1.0/detail
    for i in frange(xdomain[0], xdomain[1], detail):
        for j in frange(ydomain[0], ydomain[1], detail):

                x = i
                y = j
                try:
                    p1 = vec3(x, eval(equation), y)
                except ValueError:
                    return []
                x = i+detail
                y = j
                try:
                    p2 = vec3(x, eval(equation), y)
                except ValueError:
                    return []
                x = i
                y = j+detail
                try:
                    p3 = vec3(x, eval(equation), y)
                except ValueError:
                    return []
                x = i+detail
                y = j+detail
                try:
                    p4 = vec3(x, eval(equation), y)
                except ValueError:
                    return []

                if (reversed_normals):
                    mesh.append(p1)
                    mesh.append(p2)
                    mesh.append(p3)
                    mesh.append(p2)
                    mesh.append(p4)
                    mesh.append(p3)
                else:
                    mesh.append(p1)
                    mesh.append(p3)
                    mesh.append(p2)
                    mesh.append(p2)
                    mesh.append(p3)
                    mesh.append(p4)
    return mesh

def map(value, in_start, in_end, out_start, out_end):
    m = (out_end - out_start) / (in_end - in_start)
    return out_start + m * value

def mesh_method2(equation, detail=2.5, xdomain=[-2,2], ydomain=[-2,2], reversed_normals=False):
    layers             = 100
    circumferenceTiles = 100
    sphereVertices = []
    mesh = []

    for i in range(layers+1):
        for j in range(circumferenceTiles+1):

            x = i
            y = j

            lon     = map(x, 0, layers, -math.pi, math.pi)
            lon_sin = math.sin( lon )
            lon_cos = math.cos( lon )

            lat     = map(y, 0, circumferenceTiles, -math.pi/2, math.pi/2)
            lat_sin = math.sin( lat)
            lat_cos = math.cos( lat)
            p1 = vec3(lon_cos * lat_cos, lon_sin, lon_cos * lat_sin )

            x = i+1
            y = j    

            lon     = map(x, 0, layers, -math.pi, math.pi)
            lon_sin = math.sin( lon )
            lon_cos = math.cos( lon )

            lat     = map(y, 0, circumferenceTiles, -math.pi/2, math.pi/2)
            lat_sin = math.sin( lat)
            lat_cos = math.cos( lat)
            p2 = vec3(lon_cos * lat_cos, lon_sin, lon_cos * lat_sin )

            x = i
            y = j+1

            lon     = map(x, 0, layers, -math.pi, math.pi)
            lon_sin = math.sin( lon )
            lon_cos = math.cos( lon )

            lat     = map(y, 0, circumferenceTiles, -math.pi/2, math.pi/2)
            lat_sin = math.sin( lat)
            lat_cos = math.cos( lat)
            p3 = vec3(lon_cos * lat_cos, lon_sin, lon_cos * lat_sin )

            x = i+1
            y = j+1

            lon     = map(x, 0, layers, -math.pi, math.pi)
            lon_sin = math.sin( lon )
            lon_cos = math.cos( lon )

            lat     = map(y, 0, circumferenceTiles, -math.pi/2, math.pi/2)
            lat_sin = math.sin( lat)
            lat_cos = math.cos( lat)
            p4 = vec3(lon_cos * lat_cos, lon_sin, lon_cos * lat_sin )


            mesh.append(p1)
            mesh.append(p3)
            mesh.append(p2)
            mesh.append(p2)
            mesh.append(p3)
            mesh.append(p4)

    return mesh
    

def create_mesh(equation, color=vec3(-1,-1,-1), detail=2.5, xdomain=[-2,2], ydomain=[-2,2], reversed_normals=False):
   
    # create a random color
    if (color == vec3(-1,-1,-1)):
        v1,v2,v3 = random.uniform(0,1),random.uniform(0,1),random.uniform(0,1)
        color = vec3(v1,v2,v3)

    create_png(color)


    mesh = mesh_method1(equation, detail, xdomain, ydomain, reversed_normals)
    indices = []
    if (mesh == []):
        mesh = mesh_method2(equation, detail, xdomain, ydomain, reversed_normals)

    create_obj_file(mesh, "./data/mesh.obj", indices)
    mesh_model = StaticObject("./data/mesh.obj")
    return mesh_model

def frange(start, end, increment):
    return [x * increment for x in range(int(start * 1.0/increment), int(end * 1.0/increment))]


class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        set_cursor_visible(self.window, True)

        self.mesh = StaticObject("./data/mesh.obj")
        self.mesh.set_to_draw = False
        self.earth = StaticObject("../solar-system/data/earth.obj")
        self.light = Light(vec3(0, 1, 4), vec3(1,1,1))
        self.mouse_down = False
        self.lines = []
        self.rot_axis = vec3(0,0,0)
        self.active_camera.position = vec3(0,0,5)
        self.a = 0.0
    def update(self):
        self.processInput(self.window)

        rot_matrix = mat4(1.0)
        rot_matrix = rotate(rot_matrix, math.radians(90.0), vec3(1.0, 0.0, 0.0))
        rot_matrix = rotate(rot_matrix, math.radians(90.0+self.active_camera.yaw), vec3(0.0, 0.0, 1.0))
        #rot_matrix = rotate(rot_matrix, math.radians(self.active_camera.pitch), vec3(1.0, 0.0, 0.0))

        self.mesh.model_matrix = rot_matrix
        self.earth.model_matrix = angle_axis(math.radians(self.a) ,self.rot_axis) * self.earth.model_matrix

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

    def onMouseMoved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos
        if (self.mouse_down):
            t = ray_intersect_object(self.active_camera.position, self.active_camera.front, self.mesh)
            self.line2 = Line3D(self.active_camera.position, self.active_camera.position + self.active_camera.front * t)
            self.line2.color = vec3(0,1,0)
            self.lines.append(self.line2)
            if (len(self.lines) > 2):
                self.lines = self.lines[len(self.lines)-2: len(self.lines)]
            l2 = self.lines[len(self.lines) - 2]
            l1 = self.lines[len(self.lines) - 1]
            v1 = normalize(l1.end_point - l1.start_point)
            v2 = normalize(l2.end_point - l2.start_point)
            # angle between vectors
            try:
                self.a = (1.0-dot(v1, v2))*1000000
            except:
                self.a = 0.01
            max_angle = 5
            if self.a > max_angle:
                self.a = max_angle
            #self.line3 = Line3D(l1, l2 + normalize(l2 - l1))
            #self.line3.color = vec3(1,0,0)
            self.rot_axis = cross(self.active_camera.front, l2.end_point + normalize(l2.end_point - l1.end_point))
            #self.line4 = Line3D(vec3(0,0,0), rot_axis * 5.0)
            #self.line4.color = vec3(0,0,1)
            self.earth.model_matrix = angle_axis(math.radians(self.a) ,self.rot_axis) * self.earth.model_matrix

        self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)

    def onMouseClicked(self, button, action, mods):
        if (button == 2 and action == 1):
            set_cursor_visible(self.window, False)
        if (button == 2 and action == 0):
            self.lastX = WIDTH/2
            self.lastY = HEIGHT/2


        if (button == 0 and action == 1):
            self.mouse_down = True
        if (button == 0 and action == 0):
            self.mouse_down = False

    def onWindowResized(self, width, height):
        pass

    def onKeyPressed(self, key, scancode, action, mods):
        pass

app = App("arcball", WIDTH, HEIGHT, False)
run(app)
