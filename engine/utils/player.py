from engine.graphics import *
import math
#from entity import *

WIDTH = 800
HEIGHT = 600

class Player():
	# camera = Camera(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0), -90.0, 0.0)
	# entity = CharacterEntity()

	# model = mat4(1.0)
	# view = mat4(1.0)
	# projection = mat4(1.0)
	# zoom = 45.0

	# def __init__(self):
	# 	self.up = vec3(0.0, 1.0, 0.0)
	# 	self.up.y = 1.0
	# 	self.gravity = True

	# def setup(self, models, modelTransforms, radius):
	# 	self.entity.setup(models, modelTransforms, radius)

	# def ProcessKeyboard(self, direction, deltaTime):
	# 	speed = self.camera.MovementSpeed * deltaTime;
	# 	final_velocity = vec3(0,0,0)
	# 	if (direction == 0):
	# 		final_velocity += self.camera.Front * speed
	# 	if (direction == 1):
	# 		final_velocity += self.camera.Front * -speed
	# 	if (direction == 2):
	# 		final_velocity += self.camera.Right * -speed
	# 	if (direction == 3):
	# 		final_velocity += self.camera.Right * speed
	# 	self.entity.velocity += normalize(final_velocity)
	# def updateGravity(self, deltaTime):
	# 	self.entity.velocity.x *= 0.8
	# 	self.entity.velocity.z *= 0.8
	# 	self.entity.velocity.y *= 0.7
	# 	if (self.gravity):
	# 	    self.entity.velocity.y -=  2.0 * deltaTime
	# 	self.entity.update()

	# def jump(self, deltaTime):
	# 	self.entity.velocity.y = 1.0

	# def Update(self, deltaTime, currentFrame):
	# 	self.updateGravity(deltaTime)
	pass