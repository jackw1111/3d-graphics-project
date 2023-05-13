import sys
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from engine.graphics import *
from keys import *
import random
import time
import math
import datetime

WIDTH = 800
HEIGHT = 600

SMALLDISTX = WIDTH/10
SMALLDISTY = HEIGHT/10
RESOLUTION = 12

kingwensequence = [
0,0,0,2,7,4,3,2,6,8,
13,5,26,25,24,15,13,16,14,19,
17,24,20,25,63,60,56,55,47,53,
36,38,39,43,39,35,22,24,22,21,
29,30,27,26,26,21,23,19,57,62,
61,55,57,57,35,50,40,29,28,26,
50,51,52,61,60,60,42,42,43,43,
42,41,45,41,46,23,35,34,21,21,
19,51,40,49,29,29,31,40,36,33,
29,26,30,16,18,14,66,64,64,56,
53,57,49,51,47,44,46,47,56,51,
53,25,37,30,31,28,30,36,35,22,
28,32,27,32,34,35,52,49,48,51,
51,53,40,43,42,26,30,28,55,41,
53,52,51,47,61,64,65,39,41,41,
22,21,23,43,41,38,24,22,24,14,
17,19,52,50,47,42,40,42,26,27,
27,34,38,33,44,44,42,41,40,37,
33,31,26,44,34,38,46,44,44,36,
37,34,36,36,36,38,43,38,27,26,
30,32,37,39,50,49,48,29,37,36,
10,19,17,24,20,25,53,52,50,53,
57,55,34,44,45,13,9,5,34,26,
32,31,41,42,31,32,30,21,19,23,
43,36,31,47,45,43,47,62,52,41,
36,38,46,47,40,43,42,42,36,38,
43,53,52,53,47,49,48,47,41,44,
15,11,19,51,40,49,23,23,25,34,
30,27,7,4,4,32,22,32,68,70,
66,68,79,71,43,45,41,38,40,41,
24,25,23,35,33,38,43,50,48,18,
17,26,34,38,33,38,40,41,34,31,
30,33,33,35,28,23,22,26,30,26,
75,77,71,62,63,63,37,40,41,49,
47,51,32,37,33,49,47,44,32,38,
28,38,39,37,22,20,17,44,50,40,
32,33,33,40,44,39,32,32,40,39,
34,41,33,33,32,32,38,36,22,20,
20,12,13,10]

def getrow(kws, i):
    v = []
    for n in range(10):
        v.append(kws[i*10+n])
    return v

def wi(i):
    return kingwensequence[int(i) % 384]

def v(x):
    return wi(int(x)) + (x-int(x)) * (wi(x+1)-wi(x)) 

def timewave(x):
    temp = 0

    for i in range(-5,5,1):
        temp += (64**i)*v(x/(64**i))

    temp = temp/(64**3)
    return temp

def day(d,m,y):
    _day = 60*60*24
    dt = datetime.datetime(y, m, d, 0, 0)
    epoch_time = datetime.datetime(2012, 12, 21)

    delta = (dt - epoch_time)
    result = -1 * delta.total_seconds() / _day
    if result < 0:
        return 0
    else:
        return result 

class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        set_cursor_visible(self.window, True)
        self.sky_box.load_skybox = False
        self.set_background_color(vec3(0.0, 0.0, 0.0))

        self.xline = Line2D(vec2(SMALLDISTX, SMALLDISTY), vec2(WIDTH-SMALLDISTX, SMALLDISTY), WIDTH, HEIGHT)
        self.yline = Line2D(vec2(SMALLDISTX, SMALLDISTY), vec2(SMALLDISTX, HEIGHT-SMALLDISTY), WIDTH, HEIGHT)

        self.graph = []
        xpos1 = 0
        ypos1 = 0
        xpos2 = 0
        ypos2 = 0
        self.points = []
        self.labels = []
        for i in range(1,RESOLUTION+1,1):
            r = timewave(day(21,i,2012)) * 100000
            pos = vec2(SMALLDISTX + i * 40, SMALLDISTY + r)

            self.points.append(pos)
            self.label = Label2D("21/"+str(int(i/2)+1)+"/2012", vec2(pos.x,50), "../minecraft/data/Minecraftia.ttf", 0.2)
            self.labels.append(self.label)

        for i in range(1, len(self.points)):

            self.l = Line2D(self.points[i-1], self.points[i], WIDTH, HEIGHT)
            self.graph.append(self.l)

        self.value = 0


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

        self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)


    def on_mouse_clicked(self, button, action, mods):
        pass

    def on_window_resized(self, width, height):
        pass

    def on_key_pressed(self, key, scancode, action, mods):

        if (key == KEY_R and action == 1):
            pass


if __name__ == "__main__":
    app = App("timewavezero", WIDTH, HEIGHT, False, False)
    run(app)
