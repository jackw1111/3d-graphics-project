import sys
sys.path.append("../engine/bin")
sys.path.append("../engine/utils")
from engine.graphics import *

class Grid():
    def __init__(self, sz=10):
        self.size = sz
        self.grid = []
        for i in range(-int(self.size/2),int(self.size/2) + 1):
            l = Line3D(vec3(-int(self.size/2), 0, i), vec3(int(self.size/2),0,i))
            if (i == 0):
                l.color = vec3(0,1,0)
            self.grid.append(l)
        for j in range(-int(self.size/2),int(self.size/2) + 1):
            l = Line3D(vec3(j, 0, -int(self.size/2)), vec3(j,0,int(self.size/2)))
            if (j == 0):
                l.color = vec3(0,1,0)
            self.grid.append(l)

