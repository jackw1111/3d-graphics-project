import sys,os
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from engine.graphics import *
from sat import *

from keys import *
from math_funcs import *
from line_rect import *
import math

WIDTH = 800
HEIGHT = 600


def get_axis(mat, i):
    v = mat[i]
    v = vec3(v.x, v.y, v.z)
    return v

def matrix_multiply(mat, vec):
    v = mat * vec4(vec.x,vec.y, vec.z, 1.0)
    return vec3(v.x, v.y, v.z)


class Player(AnimatedObject):
    def __init__(self, *args, **kwargs):
        self.f = args[0]
        AnimatedObject.__init__(self, self.f)
        #self.draw_bounding_box = True
        self.position = args[1]
        self.orientation_x = 0
        self.orientation_y = 0
        self.orientation_z = 0
        self.scale_x = 1
        self.scale_y = 1
        self.scale_z = 1

    def update(self):
        self.model_matrix = mat4(1.0)
        self.model_matrix = translate(self.model_matrix, self.position)
        self.model_matrix = rotate(self.model_matrix, math.radians(self.orientation_x), vec3(1,0,0))
        self.model_matrix = rotate(self.model_matrix, math.radians(self.orientation_y), vec3(0,1,0))
        self.model_matrix = rotate(self.model_matrix, math.radians(self.orientation_z), vec3(0,0,1))
        self.model_matrix = scale(self.model_matrix, vec3(self.scale_x, self.scale_y, self.scale_z))


    def get_mesh_data(self, i):

        _min = self.model.meshes[i].bounding_box.min
        _max = self.model.meshes[i].bounding_box.max

        point1 = vec3(_min.x, _min.y, _min.z)
        point2 = vec3(_max.x, _max.y, _max.z)
        point3 = vec3(point1.x, point2.y, point1.z)
        point4 = vec3(point2.x, point1.y, point1.z)
        point5 = vec3(point2.x, point2.y, point1.z)
        point6 = vec3(point2.x, point1.y, point2.z)
        point7 = vec3(point1.x, point2.y, point2.z)
        point8 = vec3(point1.x, point1.y, point2.z)
        v = [point8, point6, point2, point7, point1, point4, point5, point3]

        _verts = []
        for _vert in v:
            _verts.append(matrix_multiply(self.model_matrix, _vert))

        front_centre = _verts[0] + (_verts[2] - _verts[0]) * 0.5
        back_centre = _verts[6] + (_verts[4] - _verts[6]) * 0.5
        #right_centre = _verts[1] + (_verts[6] - _verts[1]) * 0.5
        #left_centre = _verts[7] + (_verts[0] - _verts[7]) * 0.5

        #bottom_centre = _verts[5] + (_verts[0] - _verts[5]) * 0.5
        #top_centre = _verts[6] + (_verts[3] - _verts[6]) * 0.5

        front_normal = front_centre - back_centre
        #back_normal = back_centre - front_centre
        right_normal = cross(_verts[5] - _verts[1], _verts[2] - _verts[1])
        #left_normal = cross(_verts[3] - _verts[0], _verts[4] - _verts[0])
        #bottom_normal = cross(_verts[4] - _verts[0], _verts[1] - _verts[0])
        top_normal = cross(_verts[6] - _verts[2], _verts[3] - _verts[2])

        normals = [normalize(front_normal), normalize(right_normal), normalize(top_normal)]
        return [_verts, normals]

class Cuboid():
    def __init__(self, verts, model_matrix=mat4(1.0)):
        self.verts = verts
        self.model_matrix = model_matrix
        self.position = vec3(0,0,0)
        self.rx = 0
        self.ry = 0
        self.rz = 0
        self.sx = 1
        self.sy = 1
        self.sz = 1
        self.l1 = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.l2 = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.l3 = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.l4 = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.l5 = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.l6 = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.l7 = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.l8 = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.l9 = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.l10 = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.l11 = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.l12 = Line3D(vec3(0,0,0), vec3(0,0,0))

        self.l13 = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.l14 = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.l15 = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.l16 = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.l17 = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.l18 = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.l13.color = vec3(0,1,0)
        self.l14.color = vec3(0,1,0)
        self.l15.color = vec3(0,1,0)
        self.l16.color = vec3(0,1,0)
        self.l17.color = vec3(0,1,0)
        self.l18.color = vec3(0,1,0)

        self.lines = [self.l1, self.l2, self.l3, self.l4, self.l5, self.l6, self.l7, self.l8, self.l9, self.l10, self.l11, self.l12]
        for l in self.lines:
            l.color = vec3(0,0,1)

    def update(self):
        self.model_matrix = mat4(1.0)
        self.model_matrix = translate(self.model_matrix, self.position)
        self.model_matrix = scale(self.model_matrix, vec3(self.sx, self.sy, self.sz))
        self.model_matrix = rotate(self.model_matrix, math.radians(self.rx), vec3(1,0,0))
        self.model_matrix = rotate(self.model_matrix, math.radians(self.ry), vec3(0,1,0))
        self.model_matrix = rotate(self.model_matrix, math.radians(self.rz), vec3(0,0,1))

        _verts = self.verts[:]
        for i,v in enumerate(_verts):
            _v = vec4(v.x, v.y, v.z, 1.0) * self.model_matrix
            pos_v4 = self.model_matrix[3]
            _verts[i] = vec3(_v.x, _v.y, _v.z)


        front_centre = _verts[0] + (_verts[2] - _verts[0]) * 0.5
        back_centre = _verts[6] + (_verts[4] - _verts[6]) * 0.5
        right_centre = _verts[1] + (_verts[6] - _verts[1]) * 0.5
        left_centre = _verts[7] + (_verts[0] - _verts[7]) * 0.5

        bottom_centre = _verts[5] + (_verts[0] - _verts[5]) * 0.5
        top_centre = _verts[6] + (_verts[3] - _verts[6]) * 0.5

        front_normal = front_centre - back_centre
        back_normal = back_centre - front_centre
        right_normal = cross(_verts[5] - _verts[1], _verts[2] - _verts[1])
        left_normal = cross(_verts[3] - _verts[0], _verts[4] - _verts[0])
        bottom_normal = cross(_verts[4] - _verts[0], _verts[1] - _verts[0])
        top_normal = cross(_verts[6] - _verts[2], _verts[3] - _verts[2])

        self.position = (front_centre + back_centre) * 0.5

        normals = [normalize(front_normal), normalize(back_normal), normalize(right_normal), normalize(left_normal), normalize(bottom_normal), normalize(top_normal)]
        return [_verts, normals]

class App(Application):
    
    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        set_cursor_visible(self.window, True)
        self.sky_box.load_skybox = False
        self.active_camera.set_far_plane(50.0)
        self.active_camera.set_near_plane(0.1)
        self.mouse_down = False
        self.line_rect = LineRect(WIDTH, HEIGHT)
        self.start_coord = vec3(0,0,0)
        self.end_coord = vec3(0,0,0)
        self.current_coord = vec3(0,0,0)

        self.control_camera = False
        self.light = Light(vec3(0, 1, 4), vec3(1,1,1))
        self.active_camera.position = vec3(0,0,20)


        self.l1 = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.l2 = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.l3 = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.l4 = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.l5 = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.l6 = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.l7 = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.l8 = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.l9 = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.l10 = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.l11 = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.l12 = Line3D(vec3(0,0,0), vec3(0,0,0))


        self.lines = [self.l1, self.l2, self.l3, self.l4, self.l5, self.l6, self.l7, self.l8, self.l9, self.l10, self.l11, self.l12]
        for l in self.lines:
            l.color = vec3(0,0,1)

        self.players = []

        for i in range(10):
            self.player = Player("../example/data/astroboy.dae", vec3(i*5,0,0))
            self.players.append(self.player)
        #self.player.draw_bounding_box = True
        #self.player.orientation_x = 10.0
        #self.player.orientation_y = -25.0
        self.frustum_verts = [vec3(-1,-1,1), vec3(1,-1,1), vec3(1,1,1), vec3(-1,1,1), vec3(-1,-1,-1), vec3(1,-1,-1), vec3(1,1,-1), vec3(-1,1,-1)]
        self._frustum = Cuboid(self.frustum_verts)

        self.i = 0
        self.first_mouse = True
        self.paused = False

    def update_line_rect(self):

        if (self.mouse_down):
            if (self.start_coord == vec3(0,0,0)):
                self.start_coord = self.current_coord
            self.end_coord = self.current_coord
            self.line_rect.update(self.start_coord, self.end_coord)


            frustum_verts = []
            for p in self.line_rect.points:
                ray_wor = ray_cast(p.x, HEIGHT-p.y, self.active_camera.view_matrix, self.active_camera.projection_matrix, WIDTH, HEIGHT)
                coord1 = self.active_camera.position + ray_wor * 0.1
                frustum_verts.append(coord1)

            for p in self.line_rect.points:
                ray_wor = ray_cast(p.x, HEIGHT-p.y, self.active_camera.view_matrix, self.active_camera.projection_matrix, WIDTH, HEIGHT)
                coord2 = self.active_camera.position + ray_wor * 50.0
                frustum_verts.append(coord2)

            self._frustum.verts = frustum_verts

        else:
            self.start_coord = vec3(0,0,0)
            self.end_coord = vec3(0,0,0)
            self.line_rect.update(self.start_coord, self.end_coord)


    def update(self):

        self.process_input(self.window)

        self.player.orientation_x += 0.1
        self.player.orientation_y += 0.1

        self.update_line_rect()

        for player in self.players:
            player.update()

        box2_verts, box2_normals = self._frustum.update()

        for player in self.players:
            # only check against head bounding box for more fine grain selection and cheaper
            for i in range(len(player.model.meshes)-1, len(player.model.meshes)): # len(player.model.meshes)
                box_verts, box_normals = player.get_mesh_data(i)

                colliding = int(boxFrustumCollision(box_verts, box_normals, self._frustum.verts, box2_normals))
                if colliding:
                    player.color = vec3(1,1,1)
                    self.paused = True
                    break
                else:
                    player.color = vec3(-1,-1,-1)


    def process_input(self, window):
        if (get_key(window, KEY_ESCAPE) == PRESS):
            set_window_should_close(self.window, True);

        if (get_key(window, KEY_W) == PRESS):
            self.active_camera.position += self.active_camera.front * self.deltaTime * self.active_camera.MovementSpeed
        if (get_key(window, KEY_S) == PRESS):
            self.active_camera.position -= self.active_camera.front * self.deltaTime * self.active_camera.MovementSpeed
        if (get_key(window, KEY_A) == PRESS):
            self.active_camera.position -= self.active_camera.right * self.deltaTime * self.active_camera.MovementSpeed
        if (get_key(window, KEY_D) == PRESS):
            self.active_camera.position += self.active_camera.right * self.deltaTime * self.active_camera.MovementSpeed


        if (get_key(window, KEY_I) == PRESS):
            self._frustum.position += self.active_camera.front * self.deltaTime * self.active_camera.MovementSpeed
        if (get_key(window, KEY_K) == PRESS):
            self._frustum.position -= self.active_camera.front * self.deltaTime * self.active_camera.MovementSpeed
        if (get_key(window, KEY_J) == PRESS):
            self._frustum.position -= self.active_camera.right * self.deltaTime * self.active_camera.MovementSpeed
        if (get_key(window, KEY_L) == PRESS):
            self._frustum.position += self.active_camera.right * self.deltaTime * self.active_camera.MovementSpeed

        if (get_key(window, KEY_LEFT_SHIFT) == PRESS):
            self.active_camera.MovementSpeed = 5.0
        else:
            self.active_camera.MovementSpeed = 1.0

        if get_key(window, KEY_P) == PRESS:
            self.paused = True
        if get_key(window, KEY_U) == PRESS:
            self.paused = False

    def on_mouse_moved(self, xpos, ypos):

        if self.first_mouse:
            self.lastX = xpos
            self.lastY = ypos
            self.first_mouse = False
    

        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos

        if not self.mouse_down:
            self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)

        self.current_coord = vec3(self.lastX, HEIGHT-self.lastY, 0.0)
    def on_mouse_clicked(self, button, action, mods):

        if (button == 2 and action == 1):
            self.control_camera = True
            set_cursor_visible(self.window, False)
        if (button == 2 and action == 0):
            self.control_camera = False
            set_cursor_visible(self.window, True)
            self.first_mouse = True

        if (button == 0 and action == 1):
            self.mouse_down = True
        if (button == 0 and action == 0):
            self.mouse_down = False

        if (button == 1 and action == 1):

            _verts = self.frustum.verts
            for i in range(4):
                s = _verts[i]
                e = _verts[(i+1)%4]
                self.lines[i].set_endpoints(s,e)

            for i in range(4):
                s = _verts[i+4]
                e = _verts[(i+1)%4+4]
                self.lines[i+4].set_endpoints(s,e)

            #print (verts)
            self.lines[8].start_point = _verts[0]
            self.lines[8].end_point = _verts[4]

            self.lines[9].start_point = _verts[1]
            self.lines[9].end_point = _verts[5]

            self.lines[10].start_point = _verts[2]
            self.lines[10].end_point = _verts[6]

            self.lines[11].start_point = _verts[3]
            self.lines[11].end_point = _verts[7]


    def on_key_pressed(self, key, scancode, action, mods):
        pass

if __name__ == "__main__":
    app = App("aom", WIDTH, HEIGHT, False, True)
    run(app)

