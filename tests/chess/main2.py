import sys
sys.path.append("/home/me/Documents/3d-graphics-project/engine/bin")
sys.path.append("/home/me/Documents/3d-graphics-project/engine/utils")
from console import *
from axis_3d import *
from engine.graphics import *
from collisions2 import *
from keys import *
from player import *
import random
import time
import math
from OpenGL.GL import *

WIDTH = 800
HEIGHT = 600

def myround(x, base=4):
    return base * round(x/base)

class ChessPiece(StaticObject):
    i = 0
    def __init__(self, file):
        StaticObject.__init__(self, file)
        ChessPiece.i += 1
        self.i = ChessPiece.i
        if (self.i > 16):
            self.model_matrix = translate(mat4(1.0), vec3(self.i*2 - 32,4,6))
        else:
            self.model_matrix = translate(mat4(1.0), vec3(self.i*2,4,0))

class App(Application):

    def initialize_board(self):

        counter = 0

        # draw pieces
        for i in range(8):
            tmp_model = ChessPiece("./data/pawn.obj")
            tmp_model.id = counter
            counter+=1
            tmp_transform = mat4(1.0)
            tmp_transform = translate(tmp_transform, vec3(-14.0 + 4.0 * i, 0.0, -10.0))
            tmp_model.model_matrix = tmp_transform
            self.all_blocks.append(tmp_model)
            self.all_blocks_transforms.append(tmp_transform)

        for i in range(8):
            tmp_model = ChessPiece("./data/pawn.obj")
            tmp_model.id = counter
            counter+=1
            tmp_transform = mat4(1.0)
            tmp_transform = translate(tmp_transform, vec3(-14.0 + 4.0 * i, 0.0, 10.0))
            tmp_model.model_matrix = tmp_transform
            self.all_blocks.append(tmp_model)
            self.all_blocks_transforms.append(tmp_transform)


        for i in range(2):
            for j in range(2):
                tmp_model = ChessPiece("./data/rook.obj")
                tmp_model.id = counter
                counter+=1
                tmp_transform = mat4(1.0)
                k = 0.0
                if (i == 1):
                    k = 14.0
                else:
                    k = -14.0
                tmp_transform = translate(tmp_transform, vec3(-14.0 + 4.0 * 7 * j, 0.0, k))
                tmp_model.model_matrix = tmp_transform
                self.all_blocks.append(tmp_model)
                self.all_blocks_transforms.append(tmp_transform)


        for i in range(2):
            for j in range(2):
                tmp_model = ChessPiece("./data/knight.obj")
                tmp_model.id = counter
                counter+=1
                tmp_transform = mat4(1.0)
                k = 0.0
                if (i == 1):
                    k = 14.0
                else:
                    k = -14.0
                tmp_transform = translate(tmp_transform, vec3(-14.0 + 4.0 + 4.0 * 5 * j, 0.0, k))
                tmp_model.model_matrix = tmp_transform
                self.all_blocks.append(tmp_model)
                self.all_blocks_transforms.append(tmp_transform)

        for i in range(2):
            for j in range(2):
                tmp_model = ChessPiece("./data/bishop.obj")
                tmp_model.id = counter
                counter+=1
                tmp_transform = mat4(1.0)
                k = 0.0
                if (i == 1):
                    k = 14.0
                else:
                    k = -14.0
                tmp_transform = translate(tmp_transform, vec3(-14.0 + 8.0 + 4.0 * 3 * j, 1.0, k))
                tmp_model.model_matrix = tmp_transform
                self.all_blocks.append(tmp_model)
                self.all_blocks_transforms.append(tmp_transform)

        for i in range(2):
            for j in range(1):
                tmp_model = ChessPiece("./data/queen.obj")
                tmp_model.id = counter
                counter+=1
                tmp_transform = mat4(1.0)
                k = 0.0
                if (i == 1):
                    k = 14.0
                else:
                    k = -14.0
                tmp_transform = translate(tmp_transform, vec3(-14.0 + 12.0, 1.0, k))
                tmp_model.model_matrix = tmp_transform
                self.all_blocks.append(tmp_model)
                self.all_blocks_transforms.append(tmp_transform)

        for i in range(2):
            for j in range(1):
                tmp_model = ChessPiece("./data/king.obj")
                tmp_model.color = vec3(0,1,0)
                tmp_model.id = counter
                counter+=1
                tmp_transform = mat4(1.0)
                k = 0.0
                if (i == 1):
                    k = 14.0
                else:
                    k = -14.0
                tmp_transform = translate(tmp_transform, vec3(-14.0 + 16.0, 0.0, k))
                tmp_model.model_matrix = tmp_transform
                self.all_blocks.append(tmp_model)
                self.all_blocks_transforms.append(tmp_transform)


        self.chess_board = StaticObject("./data/chess_board.obj")
        self.chess_board.model_matrix = translate(mat4(1.0), vec3(0,-3,0))


        self.selection_box = StaticObject("./data/square.obj")

        self.table_map = {}
        letters = ["a", "b", "c", "d", "e", "f", "g", "h"]
        for i in range(8):
            letter = letters[i]
            for j in range(8):
                pos = vec3(-14.0 + i * 4.0, 0.0, 14.0 - 4.0 * j)
                self.table_map[pos] = letter + str(j+1)

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        set_cursor_visible(self.window, False)

        self.console = Console(WIDTH, HEIGHT)

        self.show_shadows = True
        self.show_ssao = False

        self.light1 = Light(vec3(0, 4, 10), vec3(1,1,1))
        #self.light2 = Light(vec3(0, 4, 14), vec3(1,1,1))

        # point camera at chess board
        self.active_camera.position = vec3(16, 20, 0)
        self.active_camera.yaw = -180.0
        self.active_camera.pitch = -45.0

        self.start_time = time.time()

        self.all_blocks = []
        self.all_blocks_transforms = []

        self.initialize_board()
        self.active_camera.GetViewMatrix()
    def update(self):
        #print (self.active_camera.Position, self.active_camera.Yaw, self.active_camera)

        #self.light.position = vec3(5*math.sin(self.currentFrame % 60), 8, 5*math.cos(self.currentFrame % 60))

        self.processInput(self.window)

        self.end_time = time.time()
        self.elapsed_time = self.end_time - self.start_time

        self.console.update(self.currentFrame, self.deltaTime)


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

        if (get_key(window, KEY_E) == PRESS):
            self.show_ssao = False
            self.use_normal_map = False

        if (get_key(window, KEY_R) == PRESS):
            self.show_ssao = True


        if (get_key(window, KEY_Y) == PRESS):
            self.use_normal_map = True

    def onMouseMoved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos
        self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)


        x = (2.0 * self.lastX) / WIDTH - 1.0
        y = 1.0 - (2.0 * self.lastY) / HEIGHT
        z = 1.0
        ray_nds = vec3(x, y, z)
        ray_clip = vec4(ray_nds.x, ray_nds.y, -1.0, 1.0)

        # # eye space to clip we would multiply by projection so
        # # clip space to eye space is the inverse projection
        ray_eye = inverse(self.active_camera.projection_matrix) * ray_clip

        # # convert point to forwards
        ray_eye = vec4(ray_eye.x, ray_eye.y, -1.0, 0.0)
        # # world space to eye space is usually multiply by view so
        # # eye space to world space is inverse view
        #print (self.player.view)
        inv_ray_wor = (inverse(self.active_camera.view_matrix) * ray_eye)
        ray_wor = vec3(inv_ray_wor.x, inv_ray_wor.y, inv_ray_wor.z)
        ray_wor = normalize(ray_wor)
        new_pos = ray_plane_intersection(self.active_camera.position, ray_wor, vec3(0,1,0), vec3(0,-4,0))
        new_pos = vec3(myround(new_pos.x + 10.0) - 10.0, -1.9, myround(new_pos.z - 10.0) + 10.0)

        m = self.selection_box.model_matrix
        m = translate(m, new_pos - get_position(m))
        self.selection_box.model_matrix = m

    def onMouseClicked(self, button, action, mods):
        if (button == 2 and action == 0):
            self.lastX = WIDTH/2
            self.lastY = HEIGHT/2
            set_cursor_visible(self.window, True) 
        else:
            x = (2.0 * self.lastX) / WIDTH - 1.0
            y = 1.0 - (2.0 * self.lastY) / HEIGHT
            z = 1.0
            ray_nds = vec3(x, y, z)
            ray_clip = vec4(ray_nds.x, ray_nds.y, -1.0, 1.0)

            # # eye space to clip we would multiply by projection so
            # # clip space to eye space is the inverse projection
            ray_eye = inverse(self.active_camera.projection_matrix) * ray_clip

            # # convert point to forwards
            ray_eye = vec4(ray_eye.x, ray_eye.y, -1.0, 0.0)
            # # world space to eye space is usually multiply by view so
            # # eye space to world space is inverse view
            #print (self.player.view)
            inv_ray_wor = (inverse(self.active_camera.view_matrix) * ray_eye)
            ray_wor = vec3(inv_ray_wor.x, inv_ray_wor.y, inv_ray_wor.z)
            ray_wor = normalize(ray_wor)

            if button == 0:
                self.clicked_block = None
                self.clicked_block_index = None
                for i,block in enumerate(self.all_blocks):
                    if (block == None):
                        continue
                    if (ray_intersect_sphere(self.active_camera.position, ray_wor, get_position(self.all_blocks_transforms[i]), 5.0)):
                        self.clicked_block = block
                        self.clicked_block_index = i
                        new_pos = ray_plane_intersection(self.active_camera.position, ray_wor, vec3(0,1,0), vec3(0,-4,0))
                        new_pos = vec3(myround(new_pos.x + 10.0) - 10.0, 0.0, myround(new_pos.z - 10.0) + 10.0)
                        if (new_pos in list(self.table_map.keys())):
                            ind = list(self.table_map.keys()).index(new_pos)
                            self.start_coord = list(self.table_map.values())[ind]
                            print (True)
                            block.color = vec3(0,1,0)
                        else:
                            print (False)
                    #else:
                    #    block.color = vec3(-1,-1,-1)

    def onWindowResized(self, width, height):
        pass

    def onKeyPressed(self, key, scancode, action, mods):
        self.console.onKeyPressed(key, scancode, action, mods)
        
if __name__ == "__main__":
    app = App("chess", WIDTH, HEIGHT, False)
    run(app)
