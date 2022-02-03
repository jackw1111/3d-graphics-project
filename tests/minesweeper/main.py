import sys
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from engine.graphics import *
from keys import *
import random
import time
import math
from enum import IntEnum

BLOCK_SIZE = 50
BLOCKS_PER_COLUMN = 8
BLOCKS_PER_ROW = 8

WIDTH =  BLOCKS_PER_ROW * BLOCK_SIZE
HEIGHT = BLOCKS_PER_COLUMN * BLOCK_SIZE

def get_row_col(frame, rows, cols):
    row = 0
    while (frame > rows):
        frame -= rows
        row += 1

    return (row, frame)

assert(get_row_col(5, 4, 2)==(1,1))

class Button():
    def __init__(self, pos):
        self.icon_rect = Rect2D(pos, vec2(BLOCK_SIZE, BLOCK_SIZE), "./data/minesweeper_texture_atlas.png", 8, 1)
        self.icon_rect.ordering = 1
        self.icon_rect.frame = 0
        # self.icon_rect.row = int(self.icon_rect.frame_number / 8)
        # self.icon_rect.col = self.icon_rect.frame_number - 8 * self.icon_rect.row

    @property
    def position(self):
        return self.icon_rect.position

    @position.setter
    def position(self, position):
        self.icon_rect.position = position

    def cursor_in_bounds(self, x:float, y:float) -> bool:
        # convert cursor position to NDC
        x = 2*x / float(WIDTH) - 1
        y = 2*y / float(HEIGHT)- 1

        hw = self.icon_rect.size.x / float(WIDTH)
        hh = self.icon_rect.size.y / float(HEIGHT)

        # convert button position to NDC
        self.ndc_position = vec2(2*self.icon_rect.position.x / WIDTH -1, 2*self.icon_rect.position.y / HEIGHT -1);

        topRight = vec2(self.ndc_position.x + hw,  self.ndc_position.y + hh);
        bottomRight = vec2(self.ndc_position.x + hw,  self.ndc_position.y - hh);
        bottomLeft = vec2(self.ndc_position.x - hw,  self.ndc_position.y - hh);
        topLeft = vec2(self.ndc_position.x - hw,  self.ndc_position.y + hh);

        # check cursor is within the button boundary
        if (x > topLeft.x and x < topRight.x and y > bottomLeft.y and y < topLeft.y):
            return True

        return False

# BUG shouldn't be += rows + 1, should be just += 1
SMALL_OFFSET = 9
MINE = SMALL_OFFSET*3
OPEN = 0
EMPTY = SMALL_OFFSET*9
FLAG = SMALL_OFFSET*10
ONE = SMALL_OFFSET*12
TWO = SMALL_OFFSET*13
THREE = SMALL_OFFSET*14
FOUR = SMALL_OFFSET*15
OFFSET = SMALL_OFFSET*3


class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        make_context_current(self.window)
        set_cursor_visible(self.window, True)
        self.sky_box.load_skybox = False
        self.set_background_color(vec3(1,1,1))

        self.grid = []

        for i in range(BLOCKS_PER_ROW):
            row = []
            for j in range(BLOCKS_PER_COLUMN):
                b = Button(vec2(BLOCK_SIZE/2 + i*BLOCK_SIZE, BLOCK_SIZE/2 + j*BLOCK_SIZE))
                b.icon_rect.frame_value = EMPTY
                row.append(b)
            self.grid.append(row)
        self.running = False


    def valid_space(self, i, j):
        return i >= 0 and i < BLOCKS_PER_ROW and j >= 0 and j < BLOCKS_PER_COLUMN and self.grid[i][j].icon_rect.frame != MINE

    def open_board(self, i, j):
        # top left
        if (self.valid_space(i-1,j+1)):
            icon = self.grid[i-1][j+1].icon_rect
            if (icon.frame == OPEN):
                if icon.frame_value == EMPTY:
                    icon.frame = icon.frame_value
                    self.open_board(i-1, j+1)
                elif icon.frame_value != MINE:
                    icon.frame = icon.frame_value

        # top middle
        if (self.valid_space(i-1,j)):
            icon = self.grid[i-1][j].icon_rect
            if (icon.frame == OPEN):
                if icon.frame_value == EMPTY:
                    icon.frame = icon.frame_value
                    self.open_board(i-1, j)
                elif icon.frame_value != MINE:
                    icon.frame = icon.frame_value
               
        # top right
        if (self.valid_space(i-1,j-1)):
            icon = self.grid[i-1][j-1].icon_rect
            if (icon.frame == OPEN):
                if icon.frame_value == EMPTY:
                    icon.frame = icon.frame_value
                    self.open_board(i, j+1)
                elif icon.frame_value != MINE:
                    icon.frame = icon.frame_value       

        # middle left
        if (self.valid_space(i,j+1)):
            icon = self.grid[i][j+1].icon_rect
            if (icon.frame == OPEN):
                if icon.frame_value == EMPTY:
                    icon.frame = icon.frame_value
                    self.open_board(i, j+1)
                elif icon.frame_value != MINE:
                    icon.frame = icon.frame_value
              
        # middle right
        if (self.valid_space(i,j-1)):
            icon = self.grid[i][j-1].icon_rect
            if (icon.frame == OPEN):
                if icon.frame_value == EMPTY:
                    icon.frame = icon.frame_value
                    self.open_board(i, j-1)
                elif icon.frame_value != MINE:
                    icon.frame = icon.frame_value
               
        # bottom left
        if (self.valid_space(i+1,j+1)):
            icon = self.grid[i+1][j+1].icon_rect
            if (icon.frame == OPEN):
                if icon.frame_value == EMPTY:
                    icon.frame = icon.frame_value
                    self.open_board(i+1, j+1)
                elif icon.frame_value != MINE:
                    icon.frame = icon.frame_value
              
        # bottom middle
        if (self.valid_space(i+1,j)):
            icon = self.grid[i+1][j].icon_rect
            if (icon.frame == OPEN):
                if icon.frame_value == EMPTY:
                    icon.frame = icon.frame_value
                    self.open_board(i+1, j)
                elif icon.frame_value != MINE:
                    icon.frame = icon.frame_value
                  
        # bottom right
        if (self.valid_space(i+1,j-1)):
            icon = self.grid[i+1][j-1].icon_rect
            if (icon.frame == OPEN):
                if icon.frame_value == EMPTY:
                    icon.frame = icon.frame_value
                    self.open_board(i+1, j-1)
                elif icon.frame_value != MINE:
                    icon.frame = icon.frame_value
    

    def setup_board(self, r, c):
        self.mines = 10
        while (self.mines > 0):
            i,j = random.randrange(BLOCKS_PER_ROW), random.randrange(BLOCKS_PER_COLUMN)
            if (self.grid[i][j].icon_rect.frame_value != MINE and i != r and j != c):
                self.grid[i][j].icon_rect.frame_value = MINE
                self.mines -= 1
                # top left
                if (self.valid_space(i-1,j+1)):
                    icon = self.grid[i-1][j+1].icon_rect
                    if icon.frame_value >= ONE:
                        icon.frame_value += SMALL_OFFSET
                    else:
                        icon.frame_value += OFFSET
                # top middle
                if (self.valid_space(i-1,j)):
                    icon = self.grid[i-1][j].icon_rect
                    if icon.frame_value >= ONE:
                        icon.frame_value += SMALL_OFFSET
                    else:
                        icon.frame_value += OFFSET                   
                # top right
                if (self.valid_space(i-1,j-1)):
                    icon = self.grid[i-1][j-1].icon_rect
                    if icon.frame_value >= ONE:
                        icon.frame_value += SMALL_OFFSET
                    else:
                        icon.frame_value += OFFSET                   
                # middle left
                if (self.valid_space(i,j+1)):
                    icon = self.grid[i][j+1].icon_rect
                    if icon.frame_value >= ONE:
                        icon.frame_value += SMALL_OFFSET
                    else:
                        icon.frame_value += OFFSET                 
                # middle right
                if (self.valid_space(i,j-1)):
                    icon = self.grid[i][j-1].icon_rect
                    if icon.frame_value >= ONE:
                        icon.frame_value += SMALL_OFFSET
                    else:
                        icon.frame_value += OFFSET                
                # bottom left
                if (self.valid_space(i+1,j+1)):
                    icon = self.grid[i+1][j+1].icon_rect
                    if icon.frame_value >= ONE:
                        icon.frame_value += SMALL_OFFSET
                    else:
                        icon.frame_value += OFFSET                 
                # bottom middle
                if (self.valid_space(i+1,j)):
                    icon = self.grid[i+1][j].icon_rect
                    if icon.frame_value >= ONE:
                        icon.frame_value += SMALL_OFFSET
                    else:
                        icon.frame_value += OFFSET                   
                # bottom right
                if (self.valid_space(i+1,j-1)):
                    icon = self.grid[i+1][j-1].icon_rect
                    if icon.frame_value >= ONE:
                        icon.frame_value += SMALL_OFFSET
                    else:
                        icon.frame_value += OFFSET        

    def update(self):
        self.process_input(self.window)


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

    def on_mouse_clicked(self, button, action, mods):
        for r,row in enumerate(self.grid):
            for c,btn in enumerate(row):
                if (btn.cursor_in_bounds(self.lastX, math.fabs(HEIGHT - self.lastY))):
                    if button == MOUSE_BUTTON_1 and action == 1:
                        if (self.running):
                            btn.icon_rect.frame = btn.icon_rect.frame_value
                            if btn.icon_rect.frame == EMPTY:
                                self.open_board(r,c)
                        else:
                            self.setup_board(r,c)
                            btn.icon_rect.frame = EMPTY
                            self.open_board(r,c)
                            self.running = True
                    if button == MOUSE_BUTTON_2 and action == 1:
                        if (self.running):
                            if btn.icon_rect.frame == OPEN:
                                btn.icon_rect.frame = FLAG
                            elif btn.icon_rect.frame == FLAG:
                                btn.icon_rect.frame = OPEN



                    if button == 1 and action == 1:
                        pass

    def on_window_resized(self, width, height):
        pass

    def on_key_pressed(self, key, scancode, action, mods):
        pass

if __name__ == "__main__":
    app = App("Mines", WIDTH, HEIGHT, False)
    run(app)
