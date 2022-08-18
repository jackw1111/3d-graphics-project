import sys
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from console import *
from axis_3d import *
from grid import *
from engine.graphics import *
from keys import *
import random
import time
import math
from OpenGL.GL import *

WIDTH = 800
HEIGHT = 600

import png

def create_mtl_file(filename):

    file_contents = """newmtl None
    Ns 0.000000
    Ka 1.000000 1.000000 1.000000
    Kd 0.800000 0.800000 0.800000
    Ks 0.800000 0.800000 0.800000
    Ke 0.000000 0.000000 0.000000
    Ni 1.450000
    d 1.000000
    illum 2
    map_Kd gradient.png"""

    with open(filename, "w") as f:
        f.write(file_contents)


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
    

def create_mesh(equation, color=vec3(-1,-1,-1), detail=2.5, xdomain=[-2,2], ydomain=[-2,2], reversed_normals=False, filename="mesh.obj"):
   
    # create a random color
    if (color == vec3(-1,-1,-1)):
        v1,v2,v3 = random.uniform(0,1),random.uniform(0,1),random.uniform(0,1)
        color = vec3(v1,v2,v3)

    print (color)
    create_png(color)
    create_mtl_file("./data/" + equation + ".mtl")


    mesh = mesh_method1(equation, detail, xdomain, ydomain, reversed_normals)
    indices = []
    if (mesh == []):
        mesh = mesh_method2(equation, detail, xdomain, ydomain, reversed_normals)

    create_obj_file(mesh, "./data/" + filename + ".obj", indices)
    mesh_model = StaticObject("./data/" + filename + ".obj")
    return mesh_model

def frange(start, end, increment):
    return [x * increment for x in range(int(start * 1.0/increment), int(end * 1.0/increment))]


# TO DO change color of labels text

class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        make_context_current(self.window)
        set_cursor_visible(self.window, False)
        self.sky_box.load_skybox = False
        #self.set_background_color(vec3(100,230,100))

        self.light = Light(vec3(0, 1, 4), vec3(1,1,1))

        #self.axis = Axis3D()

        self.grid = Grid()

        self.start_time = time.time()

        self.active_camera.position = vec3(0,5,15)
        self.console = Console(WIDTH, HEIGHT)
        self.console.new_command("newmesh", self.new_mesh)
        self.meshes = []
        self.graph_label = Label2D("no graph", vec2(100,20), "../minecraft/data/Minecraftia.ttf", 0.5)

    def new_mesh(self, equation):
        if (len(self.meshes)):
            for m in self.meshes:
                m.set_to_draw = False
        self.mesh_equations = ["(x**2+3*y**2)*math.e**(-x**2-y**2)", "0.125*(math.sin(4*x)-math.cos(5*y))", "0.25*math.sin(4*(x**2-y**2))", "0.25*math.sin(4*(x**2 + y**2))", "x*y**3-y*x**3", "math.cos(math.fabs(x) + math.fabs(y))", "math.cos(x**2+y**2-0.5)-0.5"]
        eq = random.choice(self.mesh_equations)
        self.graph_label.text = equation
        m = create_mesh(equation, xdomain=[-2,2], ydomain=[-2,2], detail=15, reversed_normals=False, filename=eq)
        self.meshes.append(m)
        self.console.show = False

    def update(self):

        self.process_input(self.window)

        self.end_time = time.time()
        self.elapsed_time = self.end_time - self.start_time
        self.console.update(self.currentFrame, self.deltaTime)
        self.set_background_color(vec3(0.7, 0.7, 0.7))

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

    def on_char_pressed(self, char):
        self.console.on_char_pressed(char)

    def on_key_pressed(self, key, scancode, action, mods):
        self.console.on_key_pressed(key, scancode, action, mods)

if __name__ == "__main__":
    app = App("grapher3D", WIDTH, HEIGHT, False, False)
    run(app)
