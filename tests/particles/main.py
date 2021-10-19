import sys
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from engine.graphics import *
from keys import *
from player import *
from particle_emitter import *
import random
import time
import math
from OpenGL.GL import *

WIDTH = 800
HEIGHT = 600


class App(Application):


	def __init__(self, *args, **kwargs):
		Application.__init__(self, *args, **kwargs)
		make_context_current(self.window)
		set_cursor_visible(self.window, False)
		self.active_camera.position = vec3(0.0, 0.0, 10.0)
		self.light = Light(vec3(0, 100, 0), vec3(1,1,1))


		self.wood_fire = StaticObject("./data/wood.obj")
		self.wood_fire.model_matrix = translate(mat4(1.0), vec3(0.0, -0.5, 0.0))
		
		self.fire_particles = ParticleEmitter("./data/sparkle.obj")

		glEnable(GL_BLEND)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO)

	def update(self):
		self.processInput(self.window)

		self.fire_particles.update(self.active_camera, self.currentFrame)  

	def processInput(self, window):
		if (get_key(window, KEY_ESCAPE) == PRESS):
			set_window_should_close(self.window, True);
		if (get_key(window, KEY_W) == PRESS):
			self.active_camera.ProcessKeyboard(0, self.deltaTime)
		if (get_key(window, KEY_S) == PRESS):
			self.active_camera.ProcessKeyboard(1, self.deltaTime)
		if (get_key(window, KEY_A) == PRESS):
			self.active_camera.ProcessKeyboard(2, self.deltaTime)
		if (get_key(window, KEY_D) == PRESS):
			self.active_camera.ProcessKeyboard(3, self.deltaTime)

	def onMouseMoved(self, xpos, ypos):
		xoffset = xpos - self.lastX
		yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

		self.lastX = xpos
		self.lastY = ypos
		self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)

	def onMouseClicked(self, button, action, mods):
		pass

	def onWindowResized(self, width, height):
		pass

	def onKeyPressed(self, key, scancode, action, mods):
		pass

if __name__ == "__main__":
	app = App("test", WIDTH, HEIGHT, False)
	run(app)
