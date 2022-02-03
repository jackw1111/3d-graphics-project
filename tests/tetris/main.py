import sys
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from engine.graphics import *
from keys import *
from player import *
import random
import time
import math


BLOCK_SIZE = 20
BLOCKS_PER_COLUMN = 20
BLOCKS_PER_ROW = 20

WIDTH =  BLOCK_SIZE * BLOCK_SIZE
HEIGHT = BLOCK_SIZE * BLOCK_SIZE

red    = vec3(1.0, 0.0, 0.0);
green  = vec3(0.0, 1.0, 0.0);
blue   = vec3(0.0, 0.0, 1.0);
purple = vec3(1.0, 0.0, 1.0);
yellow = vec3(1.0, 1.0, 0.0);

tBrick  = [[1, 1, 1, 0], 
            [0, 1, 0, 0],
            [0, 0, 0, 0],
            [0, 0, 0, 0]];

tBrick2 = [[1, 1, 1, 0], 
            [0, 1, 0, 0],
            [0, 0, 0, 0],
            [0, 0, 0, 0]];

lBrick  = [[1, 0, 0, 0], 
            [1, 0, 0, 0],
            [1, 1, 0, 0],
            [0, 0, 0, 0]];


lBrick2 = [[0, 1, 0, 0], 
            [0, 1, 0, 0],
            [1, 1, 0, 0],
            [0, 0, 0, 0]];

longBrick=[[1, 0, 0, 0], 
            [1, 0, 0, 0],
            [1, 0, 0, 0],
            [1, 0, 0, 0]];

sqrBrick= [[1, 1, 0, 0], 
            [1, 1, 0, 0],
            [0, 0, 0, 0],
            [0, 0, 0, 0]];

zBrick =  [[0, 1, 1, 0], 
            [1, 1, 0, 0],
            [0, 0, 0, 0],
            [0, 0, 0, 0]];

zBrick2 = [[1, 1, 0, 0], 
            [0, 1, 1, 0],
            [0, 0, 0, 0],
            [0, 0, 0, 0]];

def to_ndc_space(x1, y1, w, h):
    x1 = 2*x1 / w - 1;
    y1 = 2*y1 / h - 1;

    return vec3(x1, y1, 0)

class Block():
    size = vec2(WIDTH/BLOCK_SIZE, HEIGHT/BLOCK_SIZE);

    def __init__(self, pos, offset, col):
        self.offset = offset
        self.rect = Rect2D(pos, vec2(BLOCK_SIZE, BLOCK_SIZE), "./data/block_atlas.png", 5, 1)
        print (self.rect.ordering)
        self.rect.ordering = 1
        self.rect.frame = col
        self.i = 0
        self.j = 0

    @property
    def position(self):
        return self.rect.position

    @position.setter
    def position(self, position):
        self.rect.position = position

    # simple 2D AABB collision test
    def check_collision(self, other):
        if ((self.position.x + BLOCK_SIZE/2) > (other.position.x - BLOCK_SIZE/2)):
          if ((self.position.x - BLOCK_SIZE/2) < (other.position.x + BLOCK_SIZE/2)):
            if ((self.position.y + BLOCK_SIZE/2) > (other.position.y - BLOCK_SIZE/2)):
              if ((self.position.y - BLOCK_SIZE/2) < (other.position.y + BLOCK_SIZE/2)):
                return True;

class Brick():
    choices = [ sqrBrick, longBrick, tBrick, tBrick2, lBrick, lBrick2, longBrick, zBrick, zBrick2];

    def __init__(self):
        self.brickArr = random.choice(Brick.choices)
        self.sz = vec2(WIDTH/BLOCK_SIZE, HEIGHT/BLOCK_SIZE)
        self.blocks = []
        self._position = vec2(WIDTH/2+BLOCK_SIZE/2, HEIGHT/2)
        self.color = random.randrange(5,10,1) # TO DO : bug: setting frames is offset by an extra row
        self.position = self._position
        self.set_brick_array()


    def set_brick_array(self):
        # set block array
        self.pos = vec2(0,0);
        for i in range(4):
            self.pos.x = 0
            for j in range(4):
                if (self.brickArr[i][j]):
                    self.blocks.append(Block(self._position, self.pos.copy(), self.color));
                
                self.pos.x += self.sz.x;
            
            self.pos.y += self.sz.y;

        self.pos = vec2(0,0)

    @property
    def position(self):
        return self._position
    
    @position.setter
    def position(self, position):
        self._position = position
        for block in self.blocks:
            block.rect.position = self._position + block.offset

    def on_the_floor(self):

        # get lowest block
        lowest = self.blocks[0]
        for block in self.blocks:
            if block.position.y < lowest.position.y:
                lowest = block

        # check if lowest is below the floor
        if lowest.position.y < BLOCK_SIZE/2:
            # find amount below floor
            offset = vec2(0, lowest.position.y - BLOCK_SIZE/2)
            # lift out of floor
            self.position += offset
            return True

        return False

    def get_furthest_block(self):
        furthestRow = 0;
        furthestCol = 0;
        for i in range(4):
            for j in range(4):
                if (self.brickArr[i][j] and j > furthestRow):
                    furthestRow = j;
                if (self.brickArr[i][j] and i > furthestCol):
                    furthestCol = i;

        return vec2(furthestRow, furthestCol);

    # anti clockwise
    def rotate(self):
        # TO DO rotate items without deleting and recreating them
        furth = self.get_furthest_block();
        a = self.brickArr[0][0];
        b = self.brickArr[0][1];
        c = self.brickArr[0][2];
        d = self.brickArr[0][3];
        e = self.brickArr[1][0];            
        f = self.brickArr[1][1];
        g = self.brickArr[1][2];
        h = self.brickArr[1][3];
        i = self.brickArr[2][0];            
        j = self.brickArr[2][1];
        k = self.brickArr[2][2];
        l = self.brickArr[2][3];
        m = self.brickArr[3][0];            
        n = self.brickArr[3][1];
        o = self.brickArr[3][2];
        p = self.brickArr[3][3];

        if (furth == vec2(0, 3)):
            self.brickArr = [[a, e, i, m], [d, h, l, p], [c, g, k, o], [0,0,0,0]];  
        elif (furth == vec2(3, 0)):
            self.brickArr = [[b, f, j, n], [a, e, i, m], [d, h, l, p], [c, g, k, o]];   
        elif (furth == vec2(2, 1)):
            self.brickArr = [[c, g, k, o], [b, f, j, n], [a, e, i, m], [0,0,0,0]];              
        elif (furth == vec2(1, 2)):
            self.brickArr = [[b, f, j, n], [a, e, i, m], [0,0,0,0], [0,0,0,0]];    
        #self.remove() 
        #self.setBrickArray();
        block_index = 0
        # set block array
        self.pos = vec2(0,0);
        for i in range(4):
            self.pos.x = 0
            for j in range(4):
                if (self.brickArr[i][j]):
                    block = self.blocks[block_index]
                    block_index += 1
                    block.offset = self.pos.copy()
                    block.position = self._position
                    block.color = self.color
                
                self.pos.x += self.sz.x;
            
            self.pos.y += self.sz.y;

        self.pos = vec2(0,0)



    def remove(self):
        for block in self.blocks:
            block.rect.remove()
        self.blocks = []

class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        set_cursor_visible(self.window, False)
        self.sky_box.load_skybox = False
        self.set_background_color(vec3(1.0, 1.0, 1.0))
        self.current_brick = Brick()
        self.all_blocks = []
        self.rect = Rect2D(vec2(WIDTH/2, HEIGHT/2), vec2(WIDTH, HEIGHT), "./data/bg.png",1,1)
        #self.rect.alpha = 0.2
        self.rect.ordering = 1
        self.score = Label("score: 0", vec2(WIDTH - 150,HEIGHT - 50), "../minecraft/data/Minecraftia.ttf", 0.5)
        self.score.color = vec3(0,0,0)
        self.score_value = 0
        start_pos = vec2(0,0)
        end_pos = vec2(50,50)
        start_pos = to_ndc_space(end_pos.x, end_pos.y, WIDTH, HEIGHT)
        end_pos = to_ndc_space(end_pos.x, end_pos.y, WIDTH, HEIGHT)
        print (start_pos, end_pos)


    def check_completed_row(self):
        rows = [];
        values = [0 for x in range(BLOCKS_PER_COLUMN)]

        for i in range(len(self.all_blocks)):
            #print (self.all_blocks[i].j)
            values[self.all_blocks[i].j]+=1;

        for i in range(BLOCKS_PER_COLUMN):
            if (values[i] == BLOCKS_PER_ROW):
                rows.append(i);

        return rows;

    def delete_row(self, row):

        for block in self.all_blocks[:]:
            if (block.j == row):
                self.all_blocks.remove(block)

        # move all blocks down if larger than row
        for block in self.all_blocks:
            if (block.j > row):
                block.position -= vec2(0, BLOCK_SIZE);
                block.j-=1;

    def update(self):
        self.process_input(self.window)
        start_pos = vec3(random.randrange(0,5,1), random.randrange(0,100,1), -5)
        end_pos = vec3(random.randrange(0,5,1), random.randrange(0,100,1),-5)
        start_pos = to_ndc_space(end_pos.x, end_pos.y, WIDTH, HEIGHT)
        end_pos = to_ndc_space(end_pos.x, end_pos.y, WIDTH, HEIGHT)
        self.line = Line3D(start_pos, end_pos)
        
        #print (self.line.MVP)

        self.current_brick.position += vec2(0,-10 * self.deltaTime)

        for placed_block in self.all_blocks:
            for current_block in self.current_brick.blocks:
                if current_block.check_collision(placed_block):
                    # place block
                    self.all_blocks += self.current_brick.blocks
                    self.current_brick = Brick()

        if (self.current_brick.on_the_floor() is True):
            #self.current_brick.remove()
            # place block
            self.all_blocks += self.current_brick.blocks
            self.current_brick = Brick()

        # update block array
        for block in self.all_blocks:

            modelSpacePos = block.position;
            # store grid value as a vec2 between (0,0)->(20, 20)
            block.i = int(int(modelSpacePos.x) / int(BLOCK_SIZE));
            block.j = int(int(modelSpacePos.y) / int(BLOCK_SIZE));

        # update blocks array
        rowCheck2 = self.check_completed_row();

        if (len(rowCheck2)):
            for i in range(len(rowCheck2)):
                self.delete_row(rowCheck2[i]);
                self.score_value += 10
                self.score.text = "score: " + str(self.score_value)


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

    def on_mouse_moved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos

        self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)


    def on_mouse_clicked(self, button, action, mods):
        pass

    def on_window_resized(self, width, height):
        pass

    def on_key_pressed(self, key, scancode, action, mods):

        if (key == KEY_R and action == 1):
            self.current_brick.rotate()

        if (key == KEY_W and action == 1):
            self.current_brick.position += vec2(0,20)

        if (key == KEY_A and action == 1):
            self.current_brick.position += vec2(-20,0)

        if (key == KEY_S and action == 2):
            self.current_brick.position += vec2(0,-2)

        if (key == KEY_D and action == 1):
            self.current_brick.position += vec2(20,0)

if __name__ == "__main__":
    app = App("tetris", WIDTH, HEIGHT, False)
    run(app)
