import sys
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from engine.graphics import *
from keys import *
import random
import time
import math
from OpenGL.GL import *
from sunfish import *
from threading import Thread

WIDTH = 800
HEIGHT = 600

seed = 42
random.seed(seed)

def myround(x, base=4):
    return base * round(x/base)

def get_position(mat1):
    v4 = mat1[3]
    return vec3(v4.x, v4.y, v4.z)

# TO DO AI is currently broken
# TO DO select piece with ray-plane intersection instead of ray-sphere

class App(Application):

    def initialize_board(self):

        counter = 0

        # draw pieces
        for i in range(8):
            tmp_model = AnimatedObject("./data/pawn2.dae")
            tmp_model.id = counter
            counter+=1
            tmp_transform = translate(mat4(1.0), vec3(-14.0 + 4.0 * i, 0.0, -10.0))
            tmp_transform = rotate(tmp_transform, math.radians(-90.0), vec3(1,0,0))

            tmp_model.model_matrix = tmp_transform
            self.all_blocks.append(tmp_model)
            self.all_blocks_transforms.append(tmp_transform)
            self.all_blocks[len(self.all_blocks)-1].outlined = False
            self.all_blocks[len(self.all_blocks)-1].piece_color = 0

        for i in range(8):
            tmp_model = AnimatedObject("./data/pawn1.dae")
            tmp_model.id = counter
            counter+=1
            tmp_transform = translate(mat4(1.0), vec3(-14.0 + 4.0 * i, 0.0, 10.0))
            tmp_transform = rotate(tmp_transform, math.radians(-90.0), vec3(1,0,0))

            tmp_model.model_matrix = tmp_transform
            self.all_blocks.append(tmp_model)
            self.all_blocks_transforms.append(tmp_transform)
            self.all_blocks[len(self.all_blocks)-1].outlined = False
            self.all_blocks[len(self.all_blocks)-1].piece_color = 1


        for i in range(2):
            for j in range(2):
                tmp_model = AnimatedObject("./data/rook" + str(2-i) + ".dae")
                tmp_model.id = counter
                counter+=1
                tmp_transform = mat4(1.0)
                k = 0.0
                if (i == 1):
                    k = 14.0
                else:
                    k = -14.0
                tmp_transform = translate(tmp_transform, vec3(-14.0 + 4.0 * 7 * j, 0.0, k))
                tmp_transform = rotate(tmp_transform, math.radians(-90.0), vec3(1,0,0))

                tmp_model.model_matrix = tmp_transform
                self.all_blocks.append(tmp_model)
                self.all_blocks_transforms.append(tmp_transform)
                self.all_blocks[len(self.all_blocks)-1].outlined = False
                self.all_blocks[len(self.all_blocks)-1].piece_color = 2-i


        for i in range(2):
            for j in range(2):
                tmp_model = AnimatedObject("./data/knight" + str(2-i) + ".dae")
                tmp_model.id = counter
                counter+=1
                tmp_transform = mat4(1.0)
                k = 0.0
                if (i == 1):
                    k = 14.0
                else:
                    k = -14.0
                tmp_transform = translate(tmp_transform, vec3(-14.0 + 4.0 + 4.0 * 5 * j, 0.0, k))
                tmp_transform = rotate(tmp_transform, math.radians(-90.0), vec3(1,0,0))
                tmp_model.model_matrix = tmp_transform
                self.all_blocks.append(tmp_model)
                self.all_blocks_transforms.append(tmp_transform)
                self.all_blocks[len(self.all_blocks)-1].outlined = False
                self.all_blocks[len(self.all_blocks)-1].piece_color = 2-i

        for i in range(2):
            for j in range(2):
                tmp_model = AnimatedObject("./data/bishop" + str(2-i) + ".dae")
                tmp_model.id = counter
                counter+=1
                tmp_transform = mat4(1.0)
                k = 0.0
                if (i == 1):
                    k = 14.0
                else:
                    k = -14.0
                tmp_transform = translate(tmp_transform, vec3(-14.0 + 8.0 + 4.0 * 3 * j, 1.0, k))
                tmp_transform = rotate(tmp_transform, math.radians(-90.0), vec3(1,0,0))
                tmp_model.model_matrix = tmp_transform
                self.all_blocks.append(tmp_model)
                self.all_blocks_transforms.append(tmp_transform)
                self.all_blocks[len(self.all_blocks)-1].outlined = False
                self.all_blocks[len(self.all_blocks)-1].piece_color = 2-i

        for i in range(2):
            for j in range(1):
                tmp_model = AnimatedObject("./data/queen" + str(2-i) + ".dae")
                tmp_model.id = counter
                counter+=1
                tmp_transform = mat4(1.0)
                k = 0.0
                if (i == 1):
                    k = 14.0
                else:
                    k = -14.0
                tmp_transform = translate(tmp_transform, vec3(-14.0 + 12.0, 1.0, k))
                tmp_transform = rotate(tmp_transform, math.radians(-90.0), vec3(1,0,0))
                tmp_model.model_matrix = tmp_transform
                self.all_blocks.append(tmp_model)
                self.all_blocks_transforms.append(tmp_transform)
                self.all_blocks[len(self.all_blocks)-1].outlined = False
                self.all_blocks[len(self.all_blocks)-1].piece_color = 2-i

        for i in range(2):
            for j in range(1):
                tmp_model = AnimatedObject("./data/king" + str(2-i) + ".dae")
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
                tmp_transform = rotate(tmp_transform, math.radians(-90.0), vec3(1,0,0))
                tmp_model.model_matrix = tmp_transform
                self.all_blocks.append(tmp_model)
                self.all_blocks_transforms.append(tmp_transform)
                self.all_blocks[len(self.all_blocks)-1].outlined = False
                self.all_blocks[len(self.all_blocks)-1].piece_color = 2-i


        self.chess_board = StaticObject("./data/chess_board.obj")
        self.chess_board.model_matrix = translate(mat4(1.0), vec3(0,-3,0))


        self.red_box = StaticObject("./data/red_square.obj")
        self.green_box = StaticObject("./data/green_square.obj")
        self.grey_box = StaticObject("./data/grey_square.obj")
        self.grey_box.set_to_draw = False
        self.green_box.set_to_draw = False
        self.red_box.set_to_draw = False
        self.hover_box = self.red_box
        self.hover_box.set_to_draw = True
        self.hover_box_color = 0
        self.selection_box =  StaticObject("./data/green_square.obj")
        self.selection_box.set_to_draw = False

        self.table_map = {}
        letters = ["a", "b", "c", "d", "e", "f", "g", "h"]
        for i in range(8):
            letter = letters[i]
            for j in range(8):
                pos = vec3(-14.0 + i * 4.0, 0.0, 14.0 - 4.0 * j)
                self.table_map[pos] = letter + str(j+1)

    def run_sunfish(self):
        hist = [Position(initial, 0, (True,True), (True,True), 0, 0)]
        searcher = Searcher()
        while True:
            print_pos(hist[-1])
            valid_move = False

            if hist[-1].score <= -MATE_LOWER:
                print("You lost")
                break

            # We query the user until she enters a (pseudo) legal move.
            move = None
            while move not in hist[-1].gen_moves():
                match = None
                if (self.my_move != None):
                    match = re.match('([a-h][1-8])'*2, self.my_move)
                if match:
                    print ("My move: ", self.my_move)
                    self.my_move = None

                    move = parse(match.group(1)), parse(match.group(2))
                    valid_move = True
                    if (self.my_move != None):
                        for i,block in enumerate(self.all_blocks):
                            block_pos = get_position(self.all_blocks_transforms[i])
                            block_taken = self.table_map[self.my_move[2:]]
                            print ("blocks: ", block_pos, block_taken)
                            if block_pos == block_taken:
                                block.set_to_draw = False
                    
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
            print("CPU move:", _move)
            if (valid_move):
                self.ai_clicked_block = None
                self.ai_clicked_block_index = None
                self.make_move(_move)
            hist.append(hist[-1].move(move))

    def check_possible_move(self):
        hist = [Position(initial, 0, (True,True), (True,True), 0, 0)]
        searcher = Searcher()
        while True:
            print_pos(hist[-1])
            valid_move = False

            if hist[-1].score <= -MATE_LOWER:
                print("You lost")
                break
            if (self.start_coord):
                # We query the user until she enters a (pseudo) legal move.
                _move = None
                while _move not in hist[-1].gen_moves():
                    new_pos = self.get_board_position()
                    possible_move = self.get_move(self.start_coord, new_pos)

                    match = None
                    if (possible_move != None):
                        match = re.match('([a-h][1-8])'*2, possible_move)
                    if match:
                        print ("My possible move: ", possible_move)

                        _move = parse(match.group(1)), parse(match.group(2))
                    else:
                        # Inform the user when invalid input (e.g. "help") is entered
                        #print("Please enter a move like g8f6")
                        self.hover_box_color = 0

                self.hover_box_color = 1



            # We query the user until she enters a (pseudo) legal move.
            # move = None
            # while move not in hist[-1].gen_moves():
            #     match = None
            #     if (self.my_move != None):
            #         match = re.match('([a-h][1-8])'*2, self.my_move)
            #     if match:
            #         print ("My move: ", self.my_move)
            #         self.my_move = None

            #         move = parse(match.group(1)), parse(match.group(2))
            #         valid_move = True
            #         if (self.my_move != None):
            #             for i,block in enumerate(self.all_blocks):
            #                 block_pos = get_position(self.all_blocks_transforms[i])
            #                 block_taken = self.table_map[self.my_move[2:]]
            #                 print ("blocks: ", block_pos, block_taken)
            #                 if block_pos == block_taken:
            #                     block.set_to_draw = False
                    
            #     else:
            #         # Inform the user when invalid input (e.g. "help") is entered
            #         #print("Please enter a move like g8f6")
            #         pass
            # hist.append(hist[-1].move(move))

            # # After our move we rotate the board and print it again.
            # # This allows us to see the effect of our move.
            # print_pos(hist[-1].rotate())

            # if hist[-1].score <= -MATE_LOWER:
            #     print("You won")
            #     break

            # # Fire up the engine to look for a move.
            # start = time.time()
            # for _depth, move, score in searcher.search(hist[-1], hist):
            #     if time.time() - start > 1:
            #         break

            # if score == MATE_UPPER:
            #     print("Checkmate!")

            # # The black player moves from a rotated position, so we have to
            # # 'back rotate' the move before printing it.
            # _move = render(119-move[0]) + render(119-move[1])
            # print("CPU move:", _move)
            # if (valid_move):
            #     self.ai_clicked_block = None
            #     self.ai_clicked_block_index = None
            #     self.make_move(_move)
            # hist.append(hist[-1].move(move))

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
                self.ai_clicked_block = block
                self.ai_clicked_block_index = i

        for i,block in enumerate(self.all_blocks):
            if (get_position(self.all_blocks_transforms[i]) == end_pos):
                if (block != None and block != self.ai_clicked_block):
                    block.set_to_draw = False
                print ("block " , block)        

        m = mat4(1.0)
        m = translate(m, end_pos)
        m = rotate(m, math.radians(-90.0), vec3(1,0,0))

        self.all_blocks_transforms[self.ai_clicked_block_index] = m

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        self.active_camera.set_far_plane(50.0)

        self.show_shadows = True
        self.show_ssao = False

        self.light1 = Light(vec3(0.9,1,-0.4), vec3(1,1,1))
        #self.light2 = Light(vec3(0, 4, 14), vec3(1,1,1))

        # point camera at chess board
        self.active_camera.position = vec3(16, 20, 0)
        self.active_camera.yaw = -180.0
        self.active_camera.pitch = -45.0

        self.start_time = time.time()

        self.all_blocks = []
        self.all_blocks_transforms = []

        self.start_coord = ""
        self.end_coord = ""
        self.my_move = None

        self.initialize_board()

        self.table_map = {}
        letters = ["a", "b", "c", "d", "e", "f", "g", "h"]
        for i in range(8):
            letter = letters[i]
            for j in range(8):
                pos = vec3(-14.0 + i * 4.0, 0.0, 14.0 - 4.0 * j)
                self.table_map[pos] = letter + str(j+1)
        self.outlined = False

        self.clicked_block = None
        self.clicked_block_index = None

        self.chess_engine_thread = Thread(target=self.run_sunfish)
        self.chess_engine_thread.start()
        self.possible_move_thread = Thread(target=self.check_possible_move)
        self.possible_move_thread.start()

        self.axes = FloatVector()

    def update(self):

        self.process_input(self.window)

        self.end_time = time.time()
        self.elapsed_time = self.end_time - self.start_time

        for i,block in enumerate(self.all_blocks):
            if block == None:
                continue
            block.model_matrix = self.all_blocks_transforms[i]
            if block.outlined:
                block.color = vec3(0,1,0)
            else:
                block.color = vec3(-1,-1,-1)
                #block.model_matrix = scale(block.model_matrix, vec3(1.5,1.5,1.5))

        if (self.hover_box_color == 0):
            self.hover_box.set_to_draw = False
            self.hover_box = self.red_box
            self.hover_box.set_to_draw = True

        elif (self.hover_box_color == 1):
            self.hover_box.set_to_draw = False
            self.hover_box = self.green_box
            self.hover_box.set_to_draw = True

    def process_input(self, window):
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

    def get_board_position(self):
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
        new_pos = vec3(myround(new_pos.x + 10.0) - 10.0, 0.0, myround(new_pos.z - 10.0) + 10.0)
        return new_pos

    def get_block(self, new_pos):
        if (new_pos in list(self.table_map.keys())):
            ind = list(self.table_map.keys()).index(new_pos)
            start_coord = list(self.table_map.values())[ind]
            pos = list(self.table_map.keys())[ind]
            c = 0
            for i,transform in enumerate(self.all_blocks_transforms):
                if get_position(transform) == pos:
                    return self.all_blocks[i]

    def get_move(self, start_coord, new_pos):
        my_move = None
        if (new_pos in list(self.table_map.keys())):
            ind = list(self.table_map.keys()).index(new_pos)
            end_coord = list(self.table_map.values())[ind]
            my_move = start_coord + end_coord
        return my_move
          
    def on_mouse_moved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos
        self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)

        new_pos = self.get_board_position()
        self.block = self.get_block(new_pos)

        if self.block != None and self.block.piece_color == 1 and not self.start_coord:
            self.hover_box_color = 1
        else:
            self.hover_box_color = 0

        # update hover box
        new_pos2 = new_pos + vec3(0,-1.9,0)
        m = self.hover_box.model_matrix
        m = translate(m, new_pos2 - get_position(m))
        self.hover_box.model_matrix = m

    def on_mouse_clicked(self, button, action, mods):
        self.selection_box.set_to_draw = False

        print ()
        if (button == 2 and action == 1):
            set_cursor_visible(self.window, False)
        if (button == 2 and action == 0):
            self.lastX = WIDTH/2
            self.lastY = HEIGHT/2
            set_cursor_visible(self.window, True) 
        else:
            new_pos = self.get_board_position()

            # LMB click to select piece
            if button == 0:

                if (action == 1):
                    self.clicked_block = None
                    self.clicked_block_index = None
                    for i,block in enumerate(self.all_blocks):
                        if (block == None):
                            continue
                        if get_position(self.hover_box.model_matrix) + vec3(0,1.9,0) == get_position(block.model_matrix) and block.piece_color == 1:
                            block.outlined = True
                            self.selection_box.model_matrix = self.hover_box.model_matrix
                            self.selection_box.set_to_draw = True
                            self.clicked_block = block
                            self.clicked_block_index = i
                            if (new_pos in list(self.table_map.keys())):
                                ind = list(self.table_map.keys()).index(new_pos)
                                self.start_coord = list(self.table_map.values())[ind]
                            else:
                                print (False)
                            break
                        else:
                            block.outlined = False
                            self.selection_box.set_to_draw = False

                elif (action == 0):

                    self.my_move = None
                    self.selection_box.set_to_draw = False

                    if (self.hover_box_color == 1):
                        if (self.clicked_block != None):
                            self.m = mat4(1.0)
                            pos_matrix = self.hover_box.model_matrix.copy()
                            new_pos = get_position(pos_matrix)
                            new_pos.y = 0.0
                            self.m = translate(self.m, new_pos)
                            self.m = rotate(self.m, math.radians(-90.0), vec3(1,0,0))
                            self.all_blocks_transforms[self.clicked_block_index] = self.m
                            for i,block in enumerate(self.all_blocks):
                                if block == None or block == self.clicked_block:
                                    continue
                            self.my_move = self.get_move(self.start_coord, new_pos)

                            for i,block in enumerate(self.all_blocks):
                                if (get_position(self.all_blocks_transforms[i]) == new_pos):
                                    if (block != None and block != self.clicked_block):
                                        block.set_to_draw = False
                                    print ("block " , block)
                        self.start_coord = None
                        self.end_coord = None
if __name__ == "__main__":
    app = App("chess", WIDTH, HEIGHT, False, False)
    run(app)
