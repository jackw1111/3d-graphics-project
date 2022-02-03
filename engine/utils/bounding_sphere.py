import sys
sys.path.append("../engine/bin")
sys.path.append("../engine/utils")
from engine.graphics import *
from OpenGL.GL import *
import math

# circle drawing class
class Circle():

    def __init__(self, position=vec3(0,0,0), radius=5, axis=vec3(0,1,0)):
        self.position = position
        self.radius = radius
        self.points = []
        self.lines = []
        self.axis = axis
        self.generate_circle()

    def generate_circle(self):
        # create points on circumference of circle
        for i in range(50):
            angle = math.radians(360.0/50.0 * i)
            x = self.radius*math.cos(angle)
            y = self.radius*math.sin(angle)
            if (self.axis == vec3(0,1,0)):
            	point = self.position + vec3(x,0,y)
            elif (self.axis == vec3(1,0,0)):
            	point = self.position + vec3(0,x,y)
            elif (self.axis == vec3(0,0,1)):
            	point = self.position + vec3(x,y,0)
            self.points.append(point)

        self.end_points = []
        # join points with lines
        for i in range(50):
            start = i
            end = (i+1)%50 # loop back around at end of circle
            start_point = self.points[start]
            end_point = self.points[end]
            self.end_points.append([start_point, end_point])
            self.line = Line3D(start_point, end_point)
            self.line.color = vec3(0,1,0)
            self.lines.append(self.line)

    def set_position(self, position):
        x = position.x
        y = position.y
        z = position.z
        for i in range(50):
            line = self.lines[i]
            if (self.axis == vec3(0,1,0)):
	            start_point = self.end_points[i][0] + vec3(x, 0, z)
	            end_point = self.end_points[i][1] + vec3(x, 0, z)
            elif (self.axis == vec3(1,0,0)):
	            start_point = self.end_points[i][0] + vec3(0,x,y)
	            end_point = self.end_points[i][1] + vec3(0,x,y)
            elif (self.axis == vec3(0,0,1)):
	            start_point = self.end_points[i][0] + vec3(x,y,0)
	            end_point = self.end_points[i][1] + vec3(x,y,0)

            line.set_endpoints(start_point, end_point)

class BoundingSphere():
	def __init__(self, model):
		aabb = model.model.getAABB()
		point1 = vec3(aabb[0], aabb[1], aabb[2])
		point2 = vec3(aabb[3], aabb[4], aabb[5])
		if (length(point1) < length(point2)):
			self.radius = length(point2)
		else:
			self.radius = length(point1)
		model_pos = get_position(model.model_matrix)
		self.c1 = Circle(model_pos, self.radius, vec3(1,0,0))
		self.c2 = Circle(model_pos, self.radius, vec3(0,1,0))
		self.c3 = Circle(model_pos, self.radius, vec3(0,0,1))

class Ellipsoid():
    def __init__(self, pos, scale):
        self.c1 = Circle(pos, scale.x, vec3(1,0,0))
        self.c2 = Circle(pos, scale.y, vec3(0,1,0))
        self.c3 = Circle(pos, scale.z, vec3(0,0,1))
