from engine.graphics import *
import math
import random

WIDTH = 800
HEIGHT = 600

def get_position(mat1):
    v4 = mat1[3]
    return vec3(v4.x, v4.y, v4.z)

class ParticleEmitter():
    view = lookAt(vec3(0.0, 0.0, 10.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0))
    projection = mat4(1.0)
    _model = mat4(1.0)
    id = 0
    def __init__(self):
        ParticleEmitter.id+=1
        self.testModel = StaticModel()
        self.map_shader = StaticShader()

        self.map_shader.setup("./data/instanced_shader.vs","./data/instanced_shader.fs")
        self.testModel.loadModel("./data/bullet.obj")

        self.projection = perspective(45.0, float(WIDTH)/float(HEIGHT), 0.1, 1000.0)
        self.t = mat4(1.0)
        self.t = translate(self.t, vec3(0, 3, 0))
        self.modelTransforms = []
        for i in range(1):
            m = mat4(1.0)

            m = translate(m, vec3(1,0.5,0))
            self.modelTransforms.append(m)
        self.x = 0.0

        self.bulletTransforms = []
        for i in range(1):
            self.bulletTransforms.append(mat4(1.0))


    def get_current_model_transforms(self):

        return self.modelTransforms

    def draw(self, camera, currentFrame, change_of_angle, player_position):
        #print ("change of angle: ", change_of_angle)

        self.map_shader.use()

        self._model = mat4(1.0)
        self.x += 0.01
        self._model = translate(self._model, player_position + vec3(self.x, 0, self.x))
        self._model = rotate(self._model, math.radians(change_of_angle), camera.Front)

        self._model = rotate(self._model, math.radians(-90.0-camera.Yaw), vec3(0.0, 1.0, 0.0))
        self._model = rotate(self._model, math.radians(camera.Pitch), vec3(1.0, 0.0, 0.0))
        v = 0.06
        self._model = scale(self._model, vec3(v, 3*v, v))
        self.view = lookAt(camera.Position, camera.Position + camera.Front, vec3(0,1,0))

        mvp = self.projection * self.view

        self.map_shader.setMat4("MVP", mvp)
        self.map_shader.setMat4("model", self._model)

        self.testModel.DrawInstanced(self.map_shader, self.get_current_model_transforms()) 

    def onMouseClicked(self, button, action, mods):
        self.x = 0
