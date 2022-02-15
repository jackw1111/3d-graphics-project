import sys
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from engine.graphics import *
from keys import *
import random

class Console():

    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.cmd = ""
        self.spacing = 30.0

        self.console = Label(">" + self.cmd + " ", vec2(self.spacing, self.height-self.spacing), "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf", 0.5)
        self.bg = Rect2D(vec2(0, self.height), vec2(int(self.width*2), int(self.height/3)), "./data/console_bg.png",1,1)
        self.bg.ordering = 1
        self.debug_mode = False
        self.show = False

        self.custom_cmds = []
        self.shift_pressed = False

    def update(self, currentFrame, deltaTime, MVP=mat4(1.0)):
        self.console.text = ">" + self.cmd + " "

        if self.show == True:
            self.bg.to_draw = True
            self.console.to_draw = True
            if (int(currentFrame) % 2 == 0):
                self.console.text = ">" + self.cmd +" "

            else:
                self.console.text = ">" + self.cmd + "_"
        else:
            self.bg.to_draw = False
            self.console.to_draw = False

    def new_command(self, key, cmd):
        self.custom_cmds.append([key, cmd])

    def on_key_pressed(self, key, scancode, action, mods):
        if (self.show == True and key == KEY_BACKSPACE and action == 1):
            self.cmd = self.cmd[:len(self.cmd)-1]

        if (self.show == True and key == KEY_ENTER and action == 1):
            data = self.cmd.split()
            self.cmd = data[0]
            arg = data[1]
            # process custom cmds assigned with new_command()
            for cmd in self.custom_cmds:
                _cmd, _func = cmd[0], cmd[1]
                if (self.cmd == _cmd):
                    _func(arg)

            if (self.cmd == "reset"):
                pass

            elif (self.cmd == "quit"):
                exit()

            elif (self.cmd == "clearcolor"):
                set_clear_color(random.uniform(0.0,1.0), random.uniform(0.0,1.0),random.uniform(0.0,1.0))

            elif (self.cmd == "debugmode"):
                if self.debug_mode:
                    self.debug_mode = False
                else:
                    self.debug_mode = True
            else:
                output = run_command(self.cmd)
                print (str(output))
                self.cmd = str(output)

    def on_char_pressed(self, c):
        c = chr(c)
        if c == '`':
            if self.show == True:
                self.show = False
                self.cmd = ""
            else:
                self.show = True
        else:
            self.cmd += c

