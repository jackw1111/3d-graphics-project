import sys
sys.path.append("/home/me/Documents/3d-graphics-project/engine/bin")
sys.path.append("/home/me/Documents/3d-graphics-project/engine/utils")

from engine.graphics import *
from keys import *
import random
import time
import math
from OpenGL.GL import *
from noise import snoise2

WIDTH = 600
HEIGHT = 600

class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        set_cursor_visible(self.window, False)
        self.set_background_color(vec3(0.8, 0.9, 1.0))

        self.active_camera.position = vec3(0,5,0)

        # create a light for the scene
        self.light = Light(vec3(-4, 8,-2), vec3(1,1,1))

        # store a list of grass blocks
        self.all_blocks = []

        # store a list of grass block transforms for collision detection
        self.all_blocks_transforms = []

        octaves = 1
        freq = 16.0 * octaves

        for i in range(50):
            for j in range(50):
                for k in range(1):
                    n =  snoise2( i / freq , j / freq, octaves)*3
                    self.tmp_object = StaticObject("data/grass_block.obj")
                    self.tmp_object.model_matrix = translate(mat4(1.0), vec3(-i*2, int(n)*2, -j*2))
                    self.all_blocks.append(self.tmp_object)
                    self.all_blocks_transforms.append(self.tmp_object.model_matrix)
                    dice = random.randrange(1,70,1)
                    if (dice == 1):
                        x,y,z = -1*random.randrange(1,70,1), int(n)*2, -1*random.randrange(1,70,1)
                        for i in range(3):
                            for j in range(3):
                                for k in range(3):
                                    self.tmp_object = StaticObject("data/leaves_block.obj")
                                    self.tmp_object.model_matrix = translate(mat4(1.0), vec3(x-i*2, y + j*2 + 8, z-k*2))
                                    self.all_blocks.append(self.tmp_object)
                                    self.all_blocks_transforms.append(self.tmp_object.model_matrix)

                        for i in range(3):
                            self.tmp_object = StaticObject("data/tree_block.obj")
                            self.tmp_object.model_matrix = translate(mat4(1.0), vec3(x-2, y+i*2 + 2, z-2))
                            self.all_blocks.append(self.tmp_object)
                            self.all_blocks_transforms.append(self.tmp_object.model_matrix)

        for i in range(50):
            for j in range(4,6):
                for k in range(50):
                    self.tmp_object = StaticObject("data/gravel_block.obj")
                    self.tmp_object.model_matrix = translate(mat4(1.0), vec3(-i*2, -j*2, -k*2))
                    self.all_blocks.append(self.tmp_object)
                    self.all_blocks_transforms.append(self.tmp_object.model_matrix)


        # for i in range(3):
        #     for j in range(8,10):
        #         for k in range(10):
        #             self.tmp_object = StaticObject("data/sand_block.obj")
        #             self.tmp_object.model_matrix = translate(mat4(1.0), vec3(-i*2, -j*2, -k*2))
        #             self.all_blocks.append(self.tmp_object)
        #             self.all_blocks_transforms.append(self.tmp_object.model_matrix)



        v = [self.all_blocks[0].model]
        self.entity = CharacterEntity(v, self.all_blocks_transforms, vec3(0.7, 1.2, 0.7))
        self.active_camera.MovementSpeed = 50.0

        # set third person mode as default
        self.third_person = True

        # create our player
        self.player = AnimatedObject("data/steve.dae")
        self.gravity = vec3(0,-10,0)
        self.y = 0
        self.jump = vec3(0,0,0)

    def update(self):

        self.processInput(self.window)
        #print (self.shadow_map_center)
        #self.shadow_map_center = self.entity.position
        # update for collision detection / resolution
        self.entity.velocity.x *= 0.1
        self.entity.velocity.z *= 0.1
        self.entity.velocity.y *= 0.1
        self.entity.velocity  += self.gravity * self.deltaTime
        self.entity.velocity += self.jump * self.deltaTime
        self.entity.update()

        if (self.entity.grounded):
            self.jump.y = 0

        if (self.jump.y > 0):
            self.jump.y -= self.deltaTime * 20
        else:
            self.jump.y = 0

        if (self.third_person):
            modelHeight = 10.0
            ytheta = -self.active_camera.pitch
            # algorithm from ThinMatrix video on third person cameras
            horizDist = modelHeight * math.cos(math.radians(ytheta))
            vertDist = modelHeight * math.sin(math.radians(ytheta))
            xtheta = self.active_camera.yaw - 90.0
            offsetx = horizDist * math.sin(math.radians(-xtheta))
            offsetz = horizDist * math.cos(math.radians(xtheta))

            self.active_camera.position = self.entity.position + vec3(-offsetx, vertDist, -offsetz)

        self.correct_orientation = translate(mat4(1.0), self.entity.position)
        self.correct_orientation = translate(self.correct_orientation, vec3(0,1,0))
        self.correct_orientation = scale(self.correct_orientation, vec3(0.3, 0.3, 0.3))
        #self.player.model_matrix = rotate(self.correct_orientation, math.radians(90.0), vec3(0,1,0))
        self.player.model_matrix = rotate(self.correct_orientation, math.radians(-90.0), vec3(1,0,0))

    def processInput(self, window):

        # exit handler
        if (get_key(window, KEY_ESCAPE) == PRESS):
            set_window_should_close(self.window, True);

        # basic keyboard controls for entity
        speed = self.active_camera.MovementSpeed * self.deltaTime;
        if (get_key(window, KEY_W) == PRESS):
            self.entity.velocity += self.active_camera.front * speed
        if (get_key(window, KEY_S) == PRESS):
            self.entity.velocity += self.active_camera.front * -speed
        if (get_key(window, KEY_A) == PRESS):
            self.entity.velocity += self.active_camera.right * -speed
        if (get_key(window, KEY_D) == PRESS):
            self.entity.velocity += self.active_camera.right * speed

        if (get_key(window, KEY_R) == PRESS):
            self.entity.position = vec3(-2,2.5,-2)
            self.player.color = vec3(random.randrange(0,2,1), random.randrange(0,2,1), random.randrange(0,2,1))
            
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
        if (key == KEY_V and action == 1):
            self.debug = True
        elif (key == KEY_V and action == 0):
            self.debug = False
        if (key == KEY_SPACE and action == 1):
            if (self.jump == vec3(0,0,0)):
                self.jump = vec3(0,20,0)

app = App("example", WIDTH, HEIGHT, False)
run(app)
