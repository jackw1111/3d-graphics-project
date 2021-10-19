import sys
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from engine.graphics import *
import math
import time

WIDTH = 800
HEIGHT = 600

class Player():
	camera = Camera(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0), -90.0, 0.0)
	entity = CharacterEntity()

	model = mat4(1.0)
	view = mat4(1.0)
	projection = mat4(1.0)
	
	cameraType = "FirstPersonCamera"
	renderScope = False
	gunType = "AWP"
	scope = Rect()
	zoom = 45.0

	def __init__(self):
		self.projection = mat4(1.0)
		self.projection = perspective(math.radians(self.zoom), float(WIDTH)/float(HEIGHT), 0.1, 100.0)
		self.up = vec3(0.0, 1.0, 0.0)
		self.up.y = 1.0

	def setup(self, models, modelTransforms, radius):
		self.entity.setup(models, modelTransforms, radius)
		self.player_shader = AnimatedShader()
		self.player_model = AnimatedModel()
		self.awp_model = AnimatedModel()
		self.ak47_model = AnimatedModel()		

		self.model = translate(mat4(1.0), vec3(1.0, 0.0, -5.0))
		self.model = rotate(self.model, math.radians(195.0), vec3(0.0, 1.0, 0.0))
		self.model = rotate(self.model, math.radians(-90.0), vec3(1.0, 0.0, 0.0))
		self.model = scale(self.model, vec3(0.1, 0.1, 0.1))

		self.player_shader.setup("./data/skinning.vs","./data/skinning.fs")
		self.awp_model.loadModel("./data/scene.gltf")
		self.ak47_model.loadModel("./data/scene.gltf")
		self.player_model = self.awp_model
		self.gunType = "AWP"
		self.scope.setup_with_image("./data/scope.png")

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

	def draw(self, shader, currentFrame):
		self.view = lookAt(self.camera.Position, self.camera.Position + self.camera.Front, self.up)
		self._model = mat4(1.0)
		#self._model = rotate(mat4(1.0), time.time() % 60, vec3(0.0, 1.0, 0.0))
		self.player_shader.setMat4("MVP", self.projection * self.model)
		self.player_model.getFrame(self.player_shader, currentFrame * 3)

		if (self.renderScope and self.gunType == "AWP"):
			self.projection = perspective(math.radians(15.0), float(WIDTH)/float(HEIGHT), 0.1, 100.0)
			self.scope.render(vec2(WIDTH/2, HEIGHT/2), vec2(WIDTH, HEIGHT), vec3(1.0, 0.0, 0.0))
		else:
			self.projection = perspective(math.radians(45.0), float(WIDTH)/float(HEIGHT), 0.1, 100.0)
			clear_depth_buffer()
			self.player_model.Draw(self.player_shader)

	def Update(self, deltaTime, currentFrame):
		#print (currentFrame)
		self.draw(self.player_shader, currentFrame % 60)
		self.updateGravity(deltaTime)
