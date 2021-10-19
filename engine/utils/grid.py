import sys
sys.path.append("../engine/bin")
sys.path.append("../engine/utils")
from engine.graphics import *

class Grid():
    def __init__(self):
        self.grid = []
        for i in range(-5,6):
            self.grid.append(Line3D(vec3(-5, 0, i), vec3(5,0,i)))
        for j in range(-5,6):
            self.grid.append(Line3D(vec3(j, 0, -5), vec3(j,0,5)))

