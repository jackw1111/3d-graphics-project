import sys
sys.path.append("/home/me/3d-graphics-project/engine/bin")

from engine.core3D.physics import *

def py_method(*args, **kwargs):
    for key in kwargs.keys():
        print ('Key: ', key, ', Value: ', kwargs[key])

kwargs = {'arg1': 1, 'arg2': 2}
args = ["1"]
cpp_method(py_method, args, kwargs)


# class s_Position():
#     def __init__(self):
#         self.x = 0
#         self.y = 0
#         self.z = 0

# class Vertex():
#     def __init__(self):
#         self.Position = s_Position()


# def someRepetitiveFunction(v, tex_position, radius, radius_2):
#     if (v.Position.x < tex_position.x - radius):
#         return
#     elif (v.Position.x > tex_position.x + radius):
#         return
#     elif (v.Position.y < tex_position.y - radius):
#         return
#     elif (v.Position.y > tex_position.y + radius):
#         return
#     elif ((tex_position.x - v.Position.x)**2 + (tex_position.z - v.Position.z)**2 < radius_2):
#         v.Position.y += 0.02

# def bulkApply(iterable, func, *args, **kwargs):
#     for item in iterable:
#         print (func(item, *args, **kwargs))

# verts = [Vertex(), Vertex(), Vertex()]

# tex_position = s_Position()
# radius = 2
# radius_2 = 4
# bulkApply(verts, someRepetitiveFunction, tex_position, radius, radius_2)
