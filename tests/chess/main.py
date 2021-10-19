import sys
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from engine.graphics import *
from keys import *
from player import *
import random
import time
import math
import queue
from OpenGL.GL import *
from sunfish import *
from threading import Thread

WIDTH = 800
HEIGHT = 600

import random
import time

SOLVED_CUBE_STR = "WWWWWWWWWRRRBBBOOOGGGRRRBBBOOOGGGRRRBBBOOOGGGYYYYYYYYY"
MOVES = ["Li", "Ri", "Ui", "Di", "Fi", "Bi", "Mi", "Ei", "Si", "L", "R", "U", "D", "F", "B", "M", "E", "S"]

seed = 42 #54071 #random.randrange(0, 100, 1)
print (seed)
random.seed(seed)

def myround(x, base=4):
    return base * round(x/base)

def get_position(mat1):
    v4 = mat1[3]
    return vec3(v4.x, v4.y, v4.z)

class Piece():

    def __init__(self):
        self.outlined = False
        self.colour = "silver"
        self.model = StaticModel()
    def loadModel(self, file):
        self.model.loadModel(file)

    def Draw(self, shader):
        self.model.Draw(shader)

class App(Application):
    player = Player()
    camera = Camera(vec3(0.0, 0.0, 10.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0), -90.0, 0.0)
    model = mat4(1.0)
    view = lookAt(vec3(0.0, 0.0, 10.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0))

    projection = mat4(1.0)
    deltaTime = 0.0
    lastFrame = 0.0
    currentFrame = 0.0

    debug_shader = StaticShader()
    debug_line = StaticModel()

    sky_box = Skybox()
    sky_box_shader = StaticShader()


    selection_box = StaticModel()
    selection_box_shader = StaticShader()
    angle = camera.Yaw
    scamble_moves = ''

    def run_sunfish(self):
        hist = [Position(initial, 0, (True,True), (True,True), 0, 0)]
        searcher = Searcher()
        while True:
            print_pos(hist[-1])

            if hist[-1].score <= -MATE_LOWER:
                print("You lost")
                break

            # We query the user until she enters a (pseudo) legal move.
            move = None
            while move not in hist[-1].gen_moves():
                match = None
                if (self.my_move != None):
                    match = re.match('([a-h][1-8])'*2, self.my_move)
                    self.my_move = None
                if match:
                    move = parse(match.group(1)), parse(match.group(2))
                else:
                    # Inform the user when invalid input (e.g. "help") is entered
                    #print("Please enter a move like g8f6")
                    pass
            hist.append(hist[-1].move(move))

            # After our move we rotate the board and print it again.
            # This allows us to see the effect of our move.
            print_pos(hist[-1].rotate())

            if hist[-1].score <= -MATE_LOWER:
                print("You won")
                break

            # Fire up the engine to look for a move.
            start = time.time()
            for _depth, move, score in searcher.search(hist[-1], hist):
                if time.time() - start > 1:
                    break

            if score == MATE_UPPER:
                print("Checkmate!")

            # The black player moves from a rotated position, so we have to
            # 'back rotate' the move before printing it.
            _move = render(119-move[0]) + render(119-move[1])
            print("My move:", _move)
            self.make_move(_move)
            hist.append(hist[-1].move(move))

    def make_move(self, move):
        start,end = move[:2], move[2:]

        start_ind = list(self.table_map.values()).index(start)
        end_ind = list(self.table_map.values()).index(end)

        start_pos = list(self.table_map.keys())[start_ind]
        start_pos.y = 0.0

        end_pos = list(self.table_map.keys())[end_ind]
        end_pos.y = 0.0
        for i,block in enumerate(self.all_blocks):
            if block == None:
                continue
            print (start_pos, get_position(self.all_blocks_transforms[i]))
            if (start_pos == get_position(self.all_blocks_transforms[i])):
                block.outlined = True
                self.clicked_block = block
                self.clicked_block_index = i

        for i,block in enumerate(self.all_blocks):
            if block == None:
                continue
            print (end_pos, get_position(self.all_blocks_transforms[i]))
            if (end_pos == get_position(self.all_blocks_transforms[i])):
                self.all_blocks[i] = None
                #self.all_blocks_transforms = None

        m = mat4(1.0)
        m = translate(m, end_pos)
        self.all_blocks_transforms[self.clicked_block_index] = m

    def setup(self):
        self.projection = perspective(45.0, float(WIDTH)/float(HEIGHT), 0.1, 100.0)
        self.map_shader = StaticShader()
        self.test_shader = StaticShader()
        self.testModel = StaticModel()
        self.outline_shader = StaticShader()
        self.outline_shader.setup("./data/outline_shader.vs","./data/outline_shader.fs")
        self.selection_box_shader.setup("./data/selection_box_shader.vs", "./data/selection_box_shader.fs")
        self.test_shader.setup("./data/basic_shader.vs","./data/basic_shader.fs")
        self.map_shader.setup("./data/shader.vs","./data/shader.fs")
        self.testModel.loadModel("./data/chess_board.obj")
        self.selection_box.loadModel("./data/square.obj")
        self.sky_box_shader.setup("./data/skybox_shader.vs","./data/skybox_shader.fs")
        self.all_blocks = []
        self.all_blocks_transforms = []
        self.selection_box_model = mat4(1.0)
        self.start_coord = ""
        self.end_coord = ""
        self.my_move = None
        self.rlastX = 0
        self.rlastY = 0
        #print (self.scramble_moves)
        faces = [
            "./data/skybox/right.jpg",
            "./data/skybox/left.jpg",
            "./data/skybox/top.jpg",
            "./data/skybox/bottom.jpg",
            "./data/skybox/front.jpg",
            "./data/skybox/back.jpg"
        ]

        self.sky_box.load(faces)

        # draw pieces
        counter = 0
        for i in range(8):
            tmp_model = Piece()
            tmp_model.id = counter
            counter+=1
            tmp_model.loadModel("./data/pawn.obj")
            self.all_blocks.append(tmp_model)
            tmp_transform = mat4(1.0)
            tmp_transform = translate(tmp_transform, vec3(-14.0 + 4.0 * i, 0.0, -10.0))
            self.all_blocks_transforms.append(tmp_transform)

        for i in range(2):
            for j in range(2):
                rook = Piece()
                rook.loadModel("./data/rook.obj")
                k = 0.0
                if (i == 1):
                    rook.colour = "gold"
                    k = 14.0
                else:
                    k = -14.0
                self.all_blocks.append(rook)
                rook_transform = mat4(1.0)
                rook_transform = translate(rook_transform, vec3(-14.0 + 4.0 * 7 * j, 0.0, k))
                self.all_blocks_transforms.append(rook_transform)

        for i in range(2):
            for j in range(2):
                knight = Piece()
                knight.loadModel("./data/knight.obj")
                k = 0.0
                if (i == 1):
                    knight.colour = "gold"
                    k = 14.0
                else:
                    k = -14.0
                self.all_blocks.append(knight)
                knight_transform = mat4(1.0)
                knight_transform = translate(knight_transform, vec3(-14.0 + 4.0 + 4.0 * 5 * j, 0.0, k))
                self.all_blocks_transforms.append(knight_transform)

        for i in range(2):
            for j in range(2):
                bishop = Piece()
                bishop.loadModel("./data/bishop.obj")
                k = 0.0
                if (i == 1):
                    bishop.colour = "gold"
                    k = 14.0
                else:
                    k = -14.0
                self.all_blocks.append(bishop)
                bishop_transform = mat4(1.0)
                bishop_transform = translate(bishop_transform, vec3(-14.0 + 8.0 + 4.0 * 3 * j, 0.0, k))
                self.all_blocks_transforms.append(bishop_transform)

        for i in range(2):
            for j in range(1):
                queen = Piece()
                queen.loadModel("./data/queen.obj")
                k = 0.0
                if (i == 1):
                    queen.colour = "gold"
                    k = 14.0
                else:
                    k = -14.0
                self.all_blocks.append(queen)
                queen_transform = mat4(1.0)
                queen_transform = translate(queen_transform, vec3(-14.0 + 12.0, 0.0, k))
                self.all_blocks_transforms.append(queen_transform)

        for i in range(2):
            for j in range(1):
                queen = Piece()
                queen.loadModel("./data/king.obj")
                k = 0.0
                if (i == 1):
                    queen.colour = "gold"
                    k = 14.0
                else:
                    k = -14.0
                self.all_blocks.append(queen)
                queen_transform = mat4(1.0)
                queen_transform = translate(queen_transform, vec3(-14.0 + 16.0, 0.0, k))
                self.all_blocks_transforms.append(queen_transform)


        for i in range(8):
            tmp_model = Piece()
            tmp_model.id = counter
            counter+=1
            tmp_model.loadModel("./data/pawn.obj")
            tmp_model.colour = "gold"
            self.all_blocks.append(tmp_model)
            tmp_transform = mat4(1.0)
            tmp_transform = translate(tmp_transform, vec3(-14.0 + 4.0 * i, 0.0, 10.0))
            self.all_blocks_transforms.append(tmp_transform)

        self.table_map = {}
        letters = ["a", "b", "c", "d", "e", "f", "g", "h"]
        for i in range(8):
            letter = letters[i]
            for j in range(8):
                pos = vec3(-14.0 + i * 4.0, 0.0, 14.0 - 4.0 * j)
                self.table_map[pos] = letter + str(j+1)

        self.lightPos = vec3(1,0,1)

        self.debug_shader.setup("../../GUI/debug_line_shader.vs", "../../GUI/debug_line_shader.fs")
        self.debug_line.loadModel("../../GUI/line.obj")
        glClearColor(0,0,0,1)
        glEnable(GL_BLEND);  
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
        self.outlined = False

        models = [self.testModel] 
        model_positions = [mat4(1.0)]
        self.player.setup(models, model_positions, vec3(0.7, 1.2, 0.7))
        self.player.gravity = False


        self.other_player_position = vec3(-10.0, 0.0, 10.0)

        self.clicked_block = None
        self.clicked_block_index = None

        self.chess_engine_thread = Thread(target=self.run_sunfish)
        self.chess_engine_thread.start()

        self.player.camera.Pitch = -90.0

        self.axes = FloatVector()

    def set_view(self, mat):
        self.map_shader.setMat4("view", mat)


    def draw(self):
        self.currentFrame = time.time()
        self.deltaTime = (self.currentFrame - self.lastFrame)
        self.lastFrame = self.currentFrame
        #print ("position:", self.camera.Position)
        #self.view = lookAt(self.camera.Position, self.camera.Position + self.camera.Front, vec3(0,1,0))
        #self.view = lookAt( self.camera.Position,  self.camera.Position + self.camera.Front,  vec3(0.0, 1.0,0.0)) 

        glEnable(GL_STENCIL_TEST);
        glEnable(GL_DEPTH_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); 

        glEnable(GL_DEPTH_CLAMP)

        self.sky_box_shader.use();
        #self.sky_box_shader.setInt("skybox", 0);

        #draw skybox as last

        sky_view = self.player.view.copy(); # remove translation from the view matrix
        sky_view[3] = vec4(0,0,0,0)

        self.sky_box_shader.setMat4("MVP", self.player.projection * sky_view);

        self.sky_box.Draw(self.sky_box_shader)


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); 
        glStencilMask(0x00); # make sure we don't update the stencil buffer while drawing the floor
        
        self.test_shader.use()
        self.test_shader.setMat4("MVP", self.player.projection * self.player.view * translate(mat4(1.0), vec3(0.0, -4.0, 0.0)))
        self.testModel.Draw(self.test_shader)          

        glStencilFunc(GL_ALWAYS, 1, 0xFF); 
        glStencilMask(0xFF);
        self.selection_box_shader.use()
        self.selection_box_shader.setMat4("MVP", self.player.projection * self.player.view * self.selection_box_model)

        self.selection_box.Draw(self.selection_box_shader)

        self.map_shader.use()
        self.view_projection = self.player.projection * self.player.view
        self.map_shader.setVec3("cameraPos",self.player.camera.Position)
        self.map_shader.setMat4("MVP", self.player.projection * self.player.view)
        self.sky_box.bindTexture()
        #self.map_shader.setInt("skybox", 0)
        #self.map_shader.use()

        for i,block in enumerate(self.all_blocks):
            if block == None:
                continue
            self.map_shader.setMat4("MVP", self.player.projection * self.player.view * self.all_blocks_transforms[i])     
            self.map_shader.setMat4("model", self.all_blocks_transforms[i])
            if (block.colour == "gold"):
                self.map_shader.setVec3("colour", vec3(0.2, 0.1, 0.0))
            else:
                self.map_shader.setVec3("colour", vec3(0.0, 0.0, 0.0))

            block.Draw(self.map_shader)


        glStencilFunc(GL_ALWAYS, 1, 0xFF); 
        glStencilMask(0xFF);

        # to here

    
        other_player_mat = translate(mat4(1.0), self.other_player_position )
        self.map_shader.setMat4("model", other_player_mat)
        #print (self.other_player_position)

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00); 
        glDisable(GL_DEPTH_TEST);
        self.outline_shader.use()
        self.outline_shader.setMat4("model", other_player_mat)
        self.outline_shader.setMat4("view", self.player.view)
        self.outline_shader.setMat4("projection", self.player.projection)       
        for i,block in enumerate(self.all_blocks):
            if block == None:
                continue
            if (block.outlined):
                self.map_shader.setMat4("model", scale(self.all_blocks_transforms[i], vec3(1.1, 1.1, 1.1)))
                block.Draw(self.map_shader)
                break
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);   
        glEnable(GL_DEPTH_TEST);  

        clear_depth_buffer()

        self.debug_shader.use()
        debug_view_projection = self.player.projection * self.player.view
        self.debug_shader.setMat4("model", scale(mat4(1.0), vec3(1.0, 1.0, 1.0)))
        self.debug_shader.setMat4("MVP", debug_view_projection * scale(mat4(1.0), vec3(1.0, 1.0, 1.0)))
        self.debug_shader.setVec3("colour", vec3(1.0, 0.0, 0.0))
        self.debug_line.Draw(self.debug_shader)
        z_axis = rotate(mat4(1.0), math.radians(-90.0), vec3(0.0, 1.0, 0.0))
        self.debug_shader.setMat4("MVP", debug_view_projection * scale(z_axis, vec3(1.0, 1.0, 1.0)))
        self.debug_shader.setMat4("model", scale(z_axis, vec3(1.0, 1.0, 1.0)))
        self.debug_shader.setVec3("colour", vec3(0.0, 0.0, 1.0))
        self.debug_line.Draw(self.debug_shader)
        y_axis = rotate(mat4(1.0), math.radians(90.0), vec3(0.0, 0.0, 1.0))
        self.debug_shader.setMat4("MVP", debug_view_projection * scale(y_axis, vec3(1.0, 1.0, 1.0)))
        self.debug_shader.setMat4("model",  scale(y_axis, vec3(1.0, 1.0, 1.0)))
        self.debug_shader.setVec3("colour", vec3(0.0, 1.0, 0.0))
        #self.debug_line.Draw(self.debug_shader)


        self.player.Update(self.deltaTime, self.currentFrame)
        self.player.camera.Position = self.player.entity.position

    def update(self): 

        self.processInput(self.window)
        self.map_shader.use()
        self.set_view(self.player.view)
        self.draw()

    def processInput(self, window):
        speed = 4.0

        if (get_key(window, KEY_ESCAPE) == PRESS):
            set_window_should_close(self.window, True);
        if (get_key(window, KEY_W) == PRESS):
            self.player.ProcessKeyboard(0, self.deltaTime*speed)
        if (get_key(window, KEY_S) == PRESS):
            self.player.ProcessKeyboard(1, self.deltaTime*speed)
        if (get_key(window, KEY_A) == PRESS):
            self.player.ProcessKeyboard(2, self.deltaTime*speed)
        if (get_key(window, KEY_D) == PRESS):
            self.player.ProcessKeyboard(3, self.deltaTime*speed)
        if (get_key(window, KEY_R) == PRESS):
            self.player.entity.position.y = 40.0
            self.player.camera.Position.y = 40.0
    def onJoystickMoved(self, jid, event):
        pass
    def onMouseMoved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos

        self.player.camera.ProcessMouseMovement(xoffset, yoffset, True)
        
        x = (2.0 * self.lastX) / WIDTH - 1.0
        y = 1.0 - (2.0 * self.lastY) / HEIGHT
        z = 1.0
        ray_nds = vec3(x, y, z)
        ray_clip = vec4(ray_nds.x, ray_nds.y, -1.0, 1.0)

        # # eye space to clip we would multiply by projection so
        # # clip space to eye space is the inverse projection
        ray_eye = inverse(self.player.projection) * ray_clip

        # # convert point to forwards
        ray_eye = vec4(ray_eye.x, ray_eye.y, -1.0, 0.0)
        # # world space to eye space is usually multiply by view so
        # # eye space to world space is inverse view
        #print (self.player.view)
        inv_ray_wor = (inverse(self.player.view) * ray_eye)
        ray_wor = vec3(inv_ray_wor.x, inv_ray_wor.y, inv_ray_wor.z)
        ray_wor = normalize(ray_wor)
        new_pos = ray_plane_intersection(self.player.entity.position, ray_wor, vec3(0,1,0), vec3(0,-4,0))
        new_pos = vec3(myround(new_pos.x + 10.0) - 10.0, 0.0, myround(new_pos.z - 10.0) + 10.0)


        m = self.selection_box_model
        m = translate(m, new_pos - get_position(m))
        self.selection_box_model = m
    def onMouseClicked(self, button, action, mods):

        if (button == 2 and action == 1):
            set_cursor_visible(self.window, False)
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
            ray_eye = inverse(self.player.projection) * ray_clip

            # # convert point to forwards
            ray_eye = vec4(ray_eye.x, ray_eye.y, -1.0, 0.0)
            # # world space to eye space is usually multiply by view so
            # # eye space to world space is inverse view
            #print (self.player.view)
            inv_ray_wor = (inverse(self.player.view) * ray_eye)
            ray_wor = vec3(inv_ray_wor.x, inv_ray_wor.y, inv_ray_wor.z)
            ray_wor = normalize(ray_wor)

            if button == 0:
                self.clicked_block = None
                self.clicked_block_index = None
                for i,block in enumerate(self.all_blocks):
                    if (block == None):
                        continue
                    if (intersect(self.player.entity.position, ray_wor, get_position(self.all_blocks_transforms[i]), 5.0)):
                        block.outlined = True
                        self.clicked_block = block
                        self.clicked_block_index = i
                        new_pos = ray_plane_intersection(self.player.entity.position, ray_wor, vec3(0,1,0), vec3(0,-4,0))
                        new_pos = vec3(myround(new_pos.x + 10.0) - 10.0, 0.0, myround(new_pos.z - 10.0) + 10.0)
                        if (new_pos in list(self.table_map.keys())):
                            ind = list(self.table_map.keys()).index(new_pos)
                            self.start_coord = list(self.table_map.values())[ind]
                        else:
                            print (False)
                    else:
                        block.outlined = False

            if self.clicked_block != None and button == 1:
                m = mat4(1.0)
                pos_matrix = self.selection_box_model.copy()
                new_pos = get_position(pos_matrix)
                new_pos.y = 0.0
                m = translate(m, new_pos)
                print (new_pos)
                self.all_blocks_transforms[self.clicked_block_index] = m
                new_pos = ray_plane_intersection(self.player.entity.position, ray_wor, vec3(0,1,0), vec3(0,-4,0))
                new_pos = vec3(myround(new_pos.x + 10.0) - 10.0, 0.0, myround(new_pos.z - 10.0) + 10.0)
                for i,block in enumerate(self.all_blocks):
                    if block == None or block == self.clicked_block:
                        continue
                    if (new_pos == get_position(self.all_blocks_transforms[i])):
                        self.all_blocks[i] = None
                if (new_pos in list(self.table_map.keys())):
                    ind = list(self.table_map.keys()).index(new_pos)
                    self.end_coord = list(self.table_map.values())[ind]
                    self.my_move = self.start_coord + self.end_coord
                else:
                    print (False)     

    def __init__(self, *args, **kwargs):
        pass

    def init(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        make_context_current(self.window)
        set_cursor_visible(self.window, True)
        self.setup()
        #self.camera.Position = vec3(0.0, 0.0, 10.0)
        self.view = lookAt(vec3(0.0, 0.0, 10.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0))



if __name__ == "__main__":
    app = App("chess", WIDTH, HEIGHT, False)
    app.init("chess", WIDTH, HEIGHT, False)
    run(app)
