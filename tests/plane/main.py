import sys
sys.path.append("/home/me/Documents/3d-graphics-project/engine/bin")
sys.path.append("/home/me/Documents/3d-graphics-project/engine/utils")

from engine.graphics import *
from keys import *
import random
import math

WIDTH = 800
HEIGHT = 600

from noise import pnoise1, snoise2

import png

def create_png(color):
    pass


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

def mesh_method1(detail=2.5, xdomain=[-2,2], ydomain=[-2,2], reversed_normals=False):
    mesh = []
    detail = 1.0/detail
    for i in frange(xdomain[0], xdomain[1], detail):
        for j in frange(ydomain[0], ydomain[1], detail):

                x = i
                y = j
                try:
                    p1 = vec3(x, snoise2(x,y, 3), y)
                except ValueError:
                    return []
                x = i+detail
                y = j
                try:
                    p2 = vec3(x, snoise2(x,y, 3), y)
                except ValueError:
                    return []
                x = i
                y = j+detail
                try:
                    p3 = vec3(x, snoise2(x,y, 3), y)
                except ValueError:
                    return []
                x = i+detail
                y = j+detail
                try:
                    p4 = vec3(x, snoise2(x,y, 3), y)
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

mesh_counter = 0

def create_mesh(detail=2.5, xdomain=[-2,2], ydomain=[-2,2], reversed_normals=False):
    global mesh_counter
    mesh_counter+=1
    mesh = mesh_method1(detail, xdomain, ydomain, reversed_normals)
    indices = []
    create_obj_file(mesh, "data/mesh" + str(mesh_counter) + ".obj", indices)
    mesh_model = StaticObject("data/mesh" + str(mesh_counter) + ".obj")

    return mesh_model

def frange(start, end, increment):
    return [x * increment for x in range(int(start * 1.0/increment), int(end * 1.0/increment))]

class PlaneDemo(Application):
    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        set_cursor_visible(self.window, False)
        self.set_background_color(vec3(1.0, 1.0, 1.0))
        #self.mesh = create_mesh(xdomain=[-2,2], ydomain=[-2,2], detail=100)
        #self.mesh1 = create_mesh(xdomain=[2,6], ydomain=[-2,2], detail=1)
        self.plane = StaticObject("data/plane.obj")
        #self.plane.set_to_draw = False
        self.audio_window = AudioWindow()
        self.audio_window.set_volume(0.4)
        self.audio_window.play_audio("./data/plane.wav")

        self.axis = StaticObject("data/3d_axis.obj")

        self.active_camera.yaw = -90

        self.plane.front = self.active_camera.front
        self.plane.yaw = 0.0
        self.plane.pitch = 0.0
        self.plane.right = self.active_camera.right
        self.plane.up = self.active_camera.up
        self.plane.position = vec3(0,5,0)
        self.plane.speed = 3.0

        self.meshes = []

        for i in range(-5,5,1):
            for j in range(-5,5,1):
                self.mesh0 = StaticObject("data/mesh.obj")
                self.mesh0.model_matrix = translate(self.mesh0.model_matrix, vec3(i*17,0,j*17))
                self.mesh0.model_matrix = scale(self.mesh0.model_matrix, vec3(5,1,5))
            self.meshes.append(self.mesh0)
        self.light = Light(vec3(0, 100, 0), vec3(1,1,1))
        self.active_camera.position = vec3(0,5,0)
        self.yaw_multiplier = 0
        self.turning_tilt = 0

    def update(self):
        self.processInput(self.window)
        self.plane.yaw = self.yaw_multiplier
        if (self.turning_tilt > 0):
            self.turning_tilt-=0.07
        if (self.turning_tilt < 0):
            self.turning_tilt+=0.07
        self.turning_tilt = min(45.0, max(-45.0, self.turning_tilt))

        front = vec3(0,0,0)
        front.x = math.cos(math.radians(self.plane.yaw)) * math.cos(math.radians(self.plane.pitch))
        front.y = math.sin(math.radians(self.plane.pitch))
        front.z = math.sin(math.radians(self.plane.yaw)) * math.cos(math.radians(self.plane.pitch))
        Front = normalize(front)
        Right = normalize(cross(Front, vec3(0,1,0))); 
        Up    = normalize(cross(Right, Front))

        #self.l = Line3D(self.plane.position, self.plane.position + Front * 5.0)
        #self.l.color = vec3(1,0,0)

        self.plane.position += Front * self.plane.speed * self.deltaTime
        self.plane.model_matrix = translate(mat4(1.0), self.plane.position)
        self.plane.model_matrix = rotate(self.plane.model_matrix, math.radians(-self.turning_tilt), Front)

        self.plane.model_matrix = rotate(self.plane.model_matrix, math.radians(-self.yaw_multiplier), Up)
        self.plane.model_matrix = rotate(self.plane.model_matrix, math.radians(self.plane.pitch), Right)

        #y = min(45.0, max(-45.0, self.yaw_multiplier))

        modelHeight = 20.0
        ytheta = -self.active_camera.pitch
        # algorithm from ThinMatrix video on third person cameras
        horizDist = modelHeight * math.cos(math.radians(ytheta))
        vertDist = modelHeight * math.sin(math.radians(ytheta))
        xtheta = self.active_camera.yaw - 90.0
        offsetx = horizDist * math.sin(math.radians(-xtheta))
        offsetz = horizDist * math.cos(math.radians(xtheta))
        self.active_camera.position = self.plane.position + vec3(-offsetx, vertDist, -offsetz)

    def processInput(self, window):
        if (get_key(window, KEY_ESCAPE) == PRESS):
            set_window_should_close(self.window, True);

        if (get_key(window, KEY_W) == PRESS):
            #if (self.plane.pitch < 45.0):
            self.plane.pitch += 0.1
            self.active_camera.pitch += 0.1

        if (get_key(window, KEY_S) == PRESS):
            #if (self.plane.pitch > -45.0):
            self.plane.pitch -= 0.1
            self.active_camera.pitch -= 0.1

        if (get_key(window, KEY_A) == PRESS):
            #if (self.plane.yaw < 45.0):
            self.yaw_multiplier -= 0.1
            self.turning_tilt += 0.15
            self.active_camera.yaw -= 0.1

        if (get_key(window, KEY_D) == PRESS):
            #if (self.plane.yaw > -45.0):
            self.yaw_multiplier +=0.1
            self.turning_tilt -= 0.15
            self.active_camera.yaw += 0.1

    def onMouseMoved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos
        self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)

if __name__ == "__main__":
    app = PlaneDemo("PlaneDemo", WIDTH, HEIGHT, False)
    run(app)

