import sys
sys.path.append("../engine/bin")
sys.path.append("../engine/utils")
from engine.graphics import *
from line_rect import *
import math
from collisionSAT import *

def matrix_multiply(mat, vec):
    v = mat * vec4(vec.x,vec.y, vec.z, 1.0)
    return vec3(v.x, v.y, v.z)

class _Frustum():
    def __init__(self):
        self.top_left_near = vec3(0,0,0)
        self.bottom_left_near = vec3(0,0,0)
        self.bottom_right_near = vec3(0,0,0)
        self.top_right_near = vec3(0,0,0)
        self.top_left_far = vec3(0,0,0)
        self.bottom_left_far = vec3(0,0,0)
        self.bottom_right_far = vec3(0,0,0)
        self.top_right_far = vec3(0,0,0)   

def update_frustum(frustum_body, frustum):
    v = [frustum.top_left_near,
    frustum.bottom_left_near,
    frustum.bottom_right_near,
    frustum.top_right_near,
    frustum.top_left_far,
    frustum.bottom_left_far,
    frustum.bottom_right_far,
    frustum.top_right_far]

    frustum_body.update(v)
    frustum_body.cm_position = (frustum.bottom_left_near + frustum.top_right_far ) * 0.5

def update_box(box, box_object):
    _min = box_object.model.meshes[0].bounding_box.min
    _max = box_object.model.meshes[0].bounding_box.max
    v5 = vec3(_min.x, _min.y, _min.z)
    v3 = vec3(_max.x, _max.y, _max.z)
    v4 = vec3(_min.x, _max.y, _min.z)
    v6 = vec3(_max.x, _min.y, _min.z)
    v7 = vec3(_max.x, _max.y, _min.z)
    v2 = vec3(_max.x, _min.y, _max.z)
    v0 = vec3(_min.x, _max.y, _max.z)
    v1 = vec3(_min.x, _min.y, _max.z)
    _v = [v0, v1, v2, v3, v4, v5, v6, v7]

    v = []
    for vert in _v:
        v.append(matrix_multiply(box_object.model_matrix, vert))

    box.update(v)  

    cm_position = vec3(0,0,0)
    for vert in v:
        cm_position += vert
    box.cm_position = cm_position * (1.0 / len(v))

class SelectionBox():
    def __init__(self, WIDTH, HEIGHT):
        self._WIDTH = WIDTH
        self._HEIGHT = HEIGHT
        self.line_rect = LineRect(self._WIDTH, self._HEIGHT)
        self.start_coord = vec3(0,0,0)
        self.end_coord = vec3(0,0,0)
        self.current_coord = vec3(0,0,0)

        self.frustum_body = Body()
        self.frustum = _Frustum()

    def update_line_rect(self, camera, mouse_down):

        if (mouse_down):
            if (self.start_coord == vec3(0,0,0)):
                self.start_coord = self.current_coord
            self.end_coord = self.current_coord
            self.line_rect.update(self.start_coord, self.end_coord)
            frustum_vertices = []
            for p in self.line_rect.points:
                ray_wor = ray_cast(p.x, self._HEIGHT-p.y, camera.view_matrix, camera.projection_matrix, self._WIDTH, self._HEIGHT)
                coord1 = camera.position + ray_wor * 0.1
                frustum_vertices.append(coord1)

            for p in self.line_rect.points:
                ray_wor = ray_cast(p.x, self._HEIGHT-p.y, camera.view_matrix, camera.projection_matrix, self._WIDTH, self._HEIGHT)
                coord2 = camera.position + ray_wor * 50.0
                frustum_vertices.append(coord2)

            self.frustum.bottom_left_near = frustum_vertices[0]
            self.frustum.bottom_right_near = frustum_vertices[1]
            self.frustum.top_right_near = frustum_vertices[2]
            self.frustum.top_left_near = frustum_vertices[3]

            self.frustum.bottom_left_far = frustum_vertices[4]
            self.frustum.bottom_right_far = frustum_vertices[5]
            self.frustum.top_right_far = frustum_vertices[6]
            self.frustum.top_left_far = frustum_vertices[7]

        else:
            self.start_coord = vec3(0,0,0)
            self.end_coord = vec3(0,0,0)
            self.line_rect.update(self.start_coord, self.end_coord)

    def update(self, lastX, lastY, camera, mouse_down, players):
        self.current_coord = vec3(lastX, self._HEIGHT-lastY, 0.0)
        self.update_line_rect(camera, mouse_down)
        update_frustum(self.frustum_body, self.frustum)

        for player in players:
            # only check against head bounding box for more fine grain selection and cheaper # TO DO
            for i in range(1):
                self.box_body = Body()
                update_box(self.box_body, player)

                queries = SAT(self.box_body, self.frustum_body)
                max_seperation = -sys.float_info.max
                for i in range(len(queries)):
                    query = queries[i]
                    if query.max_seperation > max_seperation:
                        max_seperation = query.max_seperation

                if max_seperation < 0.0:
                    player.color = vec4(1,1,1,1)
                    #player.set_to_draw = True
                    break
                else:
                    player.color = vec4(-1,-1,-1,-1)
                    #player.set_to_draw = False

