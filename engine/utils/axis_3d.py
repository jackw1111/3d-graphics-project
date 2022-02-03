import sys
sys.path.append("/home/me/Documents/3d-graphics-project/engine/bin")
sys.path.append("/home/me/Documents/3d-graphics-project/engine/utils")
import math
from engine.graphics import *

class Axis3D():
    def __init__(self):
        self.object = StaticObject("../../engine/utils/data/3d_axis.obj")
