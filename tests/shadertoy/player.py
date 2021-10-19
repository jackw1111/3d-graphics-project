from engine.graphics import *
import math
#from entity import *

WIDTH = 800
HEIGHT = 600

class Player():
	camera = Camera(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0), -90.0, 0.0)
	entity = CharacterEntity()
	playerShader = StaticShader()

	model = mat4(1.0)
	view = mat4(1.0)
	projection = mat4(1.0)
	zoom = 45.0

	def __init__(self):
		self.projection = mat4(1.0)
		self.projection = perspective(math.radians(self.zoom), float(WIDTH)/float(HEIGHT), 0.1, 100.0)
		self.up = vec3(0.0, 1.0, 0.0)
		self.up.y = 1.0

	def setup(self, models, modelTransforms, radius):
		self.entity.setup(models, modelTransforms, radius)

	def ProcessKeyboard(self, direction, deltaTime):
		speed = self.camera.MovementSpeed * deltaTime;
		if (direction == 0):
			self.entity.velocity += self.camera.Front * speed
		if (direction == 1):
			self.entity.velocity += self.camera.Front * -speed
		if (direction == 2):
			self.entity.velocity += self.camera.Right * -speed
		if (direction == 3):
			self.entity.velocity += self.camera.Right * speed

	def updateGravity(self, deltaTime):
		self.entity.velocity.x *= 0.8
		self.entity.velocity.z *= 0.8
		self.entity.velocity.y *= 0.7
		self.entity.velocity.y -=  2.0 * deltaTime
		self.entity.update()

	def jump(self, deltaTime):
		self.entity.velocity.y = 1.0

	def animate(self, shader, currentFrame):
		self.view = lookAt(self.camera.Position, self.camera.Position + self.camera.Front, self.up)
		
	def Update(self, deltaTime, currentFrame):
		self.animate(self.playerShader, currentFrame)
		self.updateGravity(deltaTime)