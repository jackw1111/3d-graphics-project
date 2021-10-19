import sys
sys.path.append("../../engine/bin")
from engine.graphics import *
from keys import *
import math

WIDTH = 800
HEIGHT = 600


class App(Application):

    def __init__(self, *args, **kwargs):

        Application.__init__(self, *args, **kwargs)
        set_cursor_visible(self.window, False)
        self.set_background_color(vec3(0.0, 0.0, 0.0))
        self.running = False

        self.sky_box.load_skybox = False
        self.light = Light(vec3(0, 1, 4), vec3(1,1,1))

        self.map = StaticObject("./data/map.obj")

        self.paddle = Rect(vec2(WIDTH/2, HEIGHT/2), vec2(100,50), "./data/paddle.png")
        self.paddle_pos = vec2(WIDTH/2, HEIGHT/2)

        self.score = Label("Score: ", vec2(0, HEIGHT*5/6), "./data/Minecraftia.ttf", 1.0)
        self.score_value = 0

        self.high_score = Label("High Score: ", vec2(0, HEIGHT*3/4), "./data/Minecraftia.ttf", 1.0)
        self.high_score_value = 0

        self.ballModel = StaticObject("./data/ball.obj")
        self.ball_pos = vec3(0,0,-3)
        self.deltaZPosition = vec3(0,0,1)
        self.deltaXPosition = vec3(1,0,0)
        self.deltaYPosition = vec3(0,1,0)
        self.xspeed = 0.15
        self.yspeed = 0.1
        self.zspeed = 0.3

    def update(self):

        self.processInput(self.window)

        if (self.running):
            if (self.ball_pos.z < -85.0):
                self.zspeed = -1*self.zspeed
            elif (self.ball_pos.z > 4.0):
                self.zspeed = -1*self.zspeed
                x,y = self.paddle_pos.x, self.paddle_pos.y
                x = 2*x / WIDTH - 1;
                y = 2*y / HEIGHT - 1;

                if (distance(vec2(x,y), vec2(self.ball_pos.x * 0.125, self.ball_pos.y* 0.125)) > 0.55):
                    self.running = False
                    self.score_value = 0

                    self.zspeed = 0.3
                else:
                    self.zspeed *= 1.1
                    self.score_value += 10
                    if (self.score_value > self.high_score_value):
                        self.high_score_value = self.score_value


            if (self.ball_pos.x < -9.0):
                self.xspeed = -1*self.xspeed
            elif (self.ball_pos.x >9.0):
                self.xspeed = -1*self.xspeed

            if (self.ball_pos.y < -9.0):
                self.yspeed = -1*self.yspeed
            elif (self.ball_pos.y >9.0):
                self.yspeed = -1*self.yspeed

            self.ball_pos = self.ball_pos + self.deltaZPosition*self.zspeed
            self.ball_pos = self.ball_pos + self.deltaXPosition*self.xspeed
            self.ball_pos = self.ball_pos + self.deltaYPosition*self.yspeed

        self.model = scale(mat4(1.0), vec3(0.1, 0.1, 0.1))
        self.model = translate(self.model, self.ball_pos)
        self.ballModel.model_matrix = self.model

        # AI paddle TO DO
        #self.paddle.position = vec2(WIDTH/2 + 4*self.ball_pos.x, HEIGHT/2 + 4*self.ball_pos.y)
        self.paddle.position = self.paddle_pos
        self.score.text = "Score: " + str(self.score_value)
        self.high_score.text = "High Score: " + str(self.high_score_value)

    def processInput(self, window):
        if (get_key(window, KEY_ESCAPE) == PRESS):
            set_window_should_close(self.window, True);

    def onMouseMoved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos

        _x = xpos
        _y = HEIGHT - ypos
        # restrict paddle to screen boundary
        self.paddle_pos = vec2(max(min(_x, WIDTH), 0), max(min(_y, HEIGHT), 0))

    def onMouseClicked(self, button, action, mods):
        if (self.running == False and button == 0 and action == 1):
            self.running = True
            self.ball_pos = vec3(0,0,-3)

if __name__ == "__main__":
    app = App("curve-ball", WIDTH, HEIGHT, False)
    run(app)
