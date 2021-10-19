import sys
sys.path.append("../engine/bin")
sys.path.append("../engine/utils")
from engine.graphics import *
from OpenGL.GL import *

class AABoundingBox:
	def __init__(self, model, color=vec3(1,1,1)):
		glLineWidth(5)
		self.model = model
		self.lines = []
		aabb = self.model.getAABB()
		self.point1 = vec3(aabb[0], aabb[1], aabb[2])
		self.point2 = vec3(aabb[3], aabb[4], aabb[5])
		self.point3 = vec3(self.point1.x, self.point2.y, self.point1.z)
		self.point4 = vec3(self.point2.x, self.point1.y, self.point1.z)
		self.point5 = vec3(self.point2.x, self.point2.y, self.point1.z)
		self.point6 = vec3(self.point2.x, self.point1.y, self.point2.z)
		self.point7 = vec3(self.point1.x, self.point2.y, self.point2.z)
		self.point8 = vec3(self.point1.x, self.point1.y, self.point2.z)
		self.line1 =Line3D(self.point1, self.point4)
		self.line1.color = color
		self.line2 =Line3D(self.point4, self.point5)
		self.line2.color = color
		self.line3 =Line3D(self.point5, self.point3)
		self.line3.color = color
		self.line4 =Line3D(self.point3, self.point1)
		self.line4.color = color
		self.line5 =Line3D(self.point6, self.point8)
		self.line5.color = color
		self.line6 =Line3D(self.point8, self.point7)
		self.line6.color = color
		self.line7 =Line3D(self.point2, self.point7)
		self.line7.color = color
		self.line8 =Line3D(self.point2, self.point6)
		self.line8.color = color
		self.line9 = Line3D(self.point2, self.point5)
		self.line9.color = color
		self.line10 =Line3D(self.point5, self.point3)
		self.line10.color = color
		self.line11 =Line3D(self.point3, self.point7)
		self.line11.color = color
		self.line12 =Line3D(self.point2, self.point7)
		self.line12.color = color
		self.line13 =Line3D(self.point6, self.point4)
		self.line13.color = color
		self.line14 =Line3D(self.point1, self.point4)
		self.line14.color = color
		self.line15 =Line3D(self.point1, self.point8)
		self.line15.color = color
		self.line16 =Line3D(self.point6, self.point8)
		self.line16.color = color
		self.lines.append(self.line1)
		self.lines.append(self.line2)
		self.lines.append(self.line3)
		self.lines.append(self.line4)
		self.lines.append(self.line5)
		self.lines.append(self.line6)
		self.lines.append(self.line7)
		self.lines.append(self.line8)
		self.lines.append(self.line9)
		self.lines.append(self.line10)
		self.lines.append(self.line11)
		self.lines.append(self.line12)
		self.lines.append(self.line13)
		self.lines.append(self.line14)
		self.lines.append(self.line15)
		self.lines.append(self.line16)

	def update(self):
		for line in self.lines:
			line.model_matrix = self.model.model_matrix