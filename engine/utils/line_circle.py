import sys
sys.path.append("../engine/bin")
sys.path.append("../engine/utils")
from engine.graphics import *
import math

# circle drawing class # BUG only works for circles/ellipses aligned to each x,y,z axes
class Circle():

    def __init__(self, position=vec3(0,0,0), up=vec3(0,1,0), radius1=5, radius2=5):
        self.points = []
        self.lines = []
        self.end_points = []

        self.position = position
        self.up = up
        self.radius1 = radius1
        self.radius2 = radius2
        self.color = vec3(0,1,0)
        
        self.update(self.position, self.up, self.radius1, self.radius2)

    def update(self, position, up, radius1, radius2):

        self.position = position
        self.up = up
        self.radius1 = radius1
        self.radius2 = radius2

        # create points on circumference of circle
        for i in range(50):
            angle = math.radians(360.0/50.0 * i)
            x = self.radius1*math.cos(angle)
            y = self.radius2*math.sin(angle)
            point = vec3(x,0,y)
            #print ("point before: ", point)
            p4 = vec4(point.x, point.y, point.z, 1.0)
            point = vec3(p4.x, p4.y, p4.z)
            #print ("point after: ", point)
            self.points.append(point)

        if (len(self.end_points) == 0):
            # join points with lines
            for i in range(50):
                start = i
                end = (i+1)%50 # loop back around at end of circle
                start_point = self.points[start]
                end_point = self.points[end]
                self.end_points.append([start_point, end_point])
                self.line = Line3D(start_point, end_point)
                self.line.color = self.color
                self.lines.append(self.line)
        else:
            for i in range(50):
                m = mat4(1.0)
                m = translate(m, self.position)
                if (self.up != vec3(0,1,0)):
                    #m = rotate(m, math.radians(90.0), cross(self.up, vec3(0,1,0)))
                    m = m * mat4_cast(angle_axis(math.radians(90.0), cross(self.up, vec3(0,1,0))))

                line = self.lines[i]

                start_point = self.end_points[i][0]
                p4 = m * vec4(start_point.x, start_point.y, start_point.z, 1.0)
                start_point = vec3(p4.x, p4.y, p4.z)

                end_point = self.end_points[i][1]
                p4 = m * vec4(end_point.x, end_point.y, end_point.z, 1.0)
                end_point = vec3(p4.x, p4.y, p4.z)

                line.set_endpoints(start_point, end_point)            


