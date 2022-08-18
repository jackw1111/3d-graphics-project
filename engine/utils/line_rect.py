import sys
sys.path.append("../engine/bin")
sys.path.append("../engine/utils")
from engine.graphics import *

# A 2D Rectangle of 4 lines used as a GUI for object selection
class LineRect():
	def __init__(self, WIDTH, HEIGHT):

		self.line1 = Line2D(vec2(0,0), vec2(5,0), WIDTH, HEIGHT)
		self.line1.color = vec3(0,1,0)

		self.line2 = Line2D(vec2(0,0), vec2(0,5), WIDTH, HEIGHT)
		self.line2.color = vec3(0,1,0)

		self.line3 = Line2D(vec2(0,0), vec2(0,0), WIDTH, HEIGHT)
		self.line3.color = vec3(0,1,0)

		self.line4 = Line2D(vec2(0,0), vec2(0,0), WIDTH, HEIGHT)
		self.line4.color = vec3(0,1,0)

		self.start_coord = vec3(0,0,0)
		self.end_coord = vec3(0,0,0)
		self.centre_coord = vec3(0,0,0)

		self.points = []

	def update(self, start, end):
		_start = vec2(min(start.x, end.x), min(start.y, end.y))
		_end = vec2(max(start.x, end.x), max(start.y, end.y))
		self.start_coord = _start
		self.end_coord = _end
		self.centre_coord = (self.start_coord + self.end_coord) * 0.5
		self.points = []

		s = vec2(self.start_coord.x, self.start_coord.y)
		e = vec2(self.end_coord.x, self.start_coord.y)
		self.points.append(s)
		self.line1.set_endpoints(s, e)

		s = vec2(self.end_coord.x, self.start_coord.y)
		e = vec2(self.end_coord.x, self.end_coord.y)
		self.points.append(s)
		self.line2.set_endpoints(s, e)

		s = vec2(self.end_coord.x, self.end_coord.y)
		e = vec2(self.start_coord.x, self.end_coord.y)
		self.points.append(s)
		self.line3.set_endpoints(s, e)

		s = vec2(self.start_coord.x, self.end_coord.y)
		e = vec2(self.start_coord.x, self.start_coord.y)
		self.points.append(s)
		self.line4.set_endpoints(s, e)