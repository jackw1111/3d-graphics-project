from engine.graphics import *
import math
import random

class ParticleEmitter():

	def __init__(self, particleImgPath):

		self.all_models = []
		for i in range(100):
			self.tmp_model = StaticObject(particleImgPath)
			self.tmp_model.position = vec3(0,0,0)
			self.tmp_model.direction = vec3(random.uniform(-0.1, 0.1), random.uniform(0, 1), random.uniform(-0.1, 0.1))
			self.all_models.append(self.tmp_model)
		self.x = 3.0

	def update(self, camera, currentFrame):
		self.x-=0.01
		if (self.x < 0.0):
			self.x = 0.0
		v = 1.5
		for i in range(100):
			self.tmp_model = self.all_models[i]
			self.tmp_model.position += self.tmp_model.direction * currentFrame * 0.01
			self.tmp_model.model_matrix = translate(mat4(1.0), self.tmp_model.position)
			rot_matrix = mat4(1.0)
			rot_matrix = rotate(rot_matrix, math.radians(-90.0-camera.yaw), vec3(0.0, 1.0, 0.0))
			rot_matrix = rotate(rot_matrix, math.radians(camera.pitch), vec3(1.0, 0.0, 0.0))
			self.tmp_model.model_matrix = self.tmp_model.model_matrix * rot_matrix
			#self.tmp_model.model_matrix = scale(self.tmp_model.model_matrix, vec3(self.x, self.x, self.x))
			if (length(self.tmp_model.position) > 3.0):
				self.tmp_model.position = vec3(0,0,0)