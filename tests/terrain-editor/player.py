from engine.graphics import *
import math
WIDTH = 800
HEIGHT = 600

class Player():
	camera = Camera(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0), -90.0, 0.0)

	cameraType = "ThirdPersonCamera"

	model = mat4(1.0)
	view = mat4(1.0)
	projection = mat4(1.0)
	gravity = 2.5

	def __init__(self):
		self.projection = mat4(1.0)
		self.projection = perspective(45.0, float(WIDTH)/float(HEIGHT), 0.1, 100.0)
		self.up = vec3(0.0, 1.0, 0.0)
		self.up.y = 1.0

	def setup(self, models, modelTransforms, radius):
		self.entity = CharacterEntity(models, modelTransforms, radius)
		self.playerShader = StaticShader()
		self.instanced_tree_shader = StaticShader()
		self.playerShader.setup("./data/shader.vs","./data/shader.fs")
		self.instanced_tree_shader.setup("./data/instanced_shader.vs","./data/instanced_shader.fs")
		self.player_model = StaticObject("./data/palm_trees.obj")
		self.model_transforms = []
		self.model_height = 0.0
		self.model_rotation = 0.0
		
	def ProcessKeyboard(self, direction, deltaTime):
		speed = self.camera.MovementSpeed * deltaTime;
		if (self.cameraType == "FirstPersonCamera"):
			if (direction == 0):
				self.entity.velocity += self.camera.front * speed
			if (direction == 1):
				self.entity.velocity += self.camera.front * -speed
			if (direction == 2):
				self.entity.velocity += self.camera.right * -speed
			if (direction == 3):
				self.entity.velocity += self.camera.right * speed
		if (self.cameraType == "ThirdPersonCamera"):
			if (direction == 0):
				self.camera.position += self.camera.front * speed
			if (direction == 1):
				self.camera.position += self.camera.front * -speed
			if (direction == 2):
				self.camera.position += self.camera.right * -speed
			if (direction == 3):
				self.camera.position += self.camera.right * speed
	def updateGravity(self, deltaTime):
		self.entity.velocity *= 0.7
		self.entity.velocity.y -=  self.gravity * deltaTime
		self.entity.update()

	def jump(self, deltaTime):
		self.entity.velocity.y += 1.5

	def animate(self, shader, currentFrame):
		if (self.cameraType == "ThirdPersonCamera"):
			self.view = lookAt(self.camera.position, self.camera.position + self.camera.front, self.up)

			self.model = mat4(1.0)
			t = intersectPlane(vec3(0,1,0), vec3(0,0,0), self.camera.position, self.camera.front)
			self.intersect_pos = self.camera.position + self.camera.front * t 
			self.texture_coords = self.intersect_pos
			if (t > 0):
				self.model = translate(self.model, self.intersect_pos)
			self.model = scale(self.model, vec3(0.01, 0.01, 0.01))

			#self.model = translate(self.model, self.entity.position + vec3(0.0, 1.0, 0.0))
			# self.model = rotate(self.model, math.radians(-90.0), vec3(1,0,0))
			# self.model = rotate(self.model, math.radians(90.0), vec3(0,0,1))
			# self.model = rotate(self.model, math.radians(-self.camera.Yaw), vec3(0,0,1))


		if (self.cameraType == "FirstPersonCamera"):
			self.view = lookAt(self.camera.position, self.camera.position + self.camera.front, self.up)
		
	def Update(self, deltaTime, currentFrame):

		self.animate(self.playerShader, currentFrame)
		

		if (self.cameraType == "ThirdPersonCamera"):
			# player
			self.playerShader.use()
			self.playerShader.setMat4("projection", self.projection)
			self.playerShader.setMat4("view", self.view)
			m = translate(self.model, vec3(0, self.model_height, 0))
			m = rotate(m, math.radians(self.model_rotation), vec3(0, 1, 0))
			self.playerShader.setMat4("model", m)
			#self.playerShader.setMat4("MVP", self.projection * self.view * self.model)
			#self.player_model.getFrame(self.playerShader, currentFrame)
			#self.player_model.model.Draw(self.playerShader)

			self.instanced_tree_shader.use()
			self.instanced_tree_shader.setMat4("projection", self.projection)
			self.instanced_tree_shader.setMat4("view", self.view)
			self.instanced_tree_shader.setMat4("model", self.model)
			#self.player_model.model.DrawInstanced(self.instanced_tree_shader, self.model_transforms)
