import sys
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from engine.graphics import *
from keys import *
import random

WIDTH = 800
HEIGHT = 600

class Button():
    def __init__(self, txt, pos, sz, bg=""):
        self.text = txt
        self.position = pos
        self.size = sz
        self.label = Label2D(self.text, self.position, "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf", 1)
        self.rect = Rect2D(self.position, self.size, bg,1,1)
        self.rect.color = vec3(0.2, 0.2, 0.2)
        self.rect.ordering = 0

    def set_text_alignment(self, pos):
        self.label.position = self.label.position + pos

    def show(self):
        self.rect.ordering = 3
        self.label.to_draw = 1

    def hide(self):
        self.rect.ordering = -1
        self.label.to_draw = 0

    # def setup(self, pos:vec2, sz:vec2, txt:str) -> None:
    #     self.position = pos;
    #     self.text = txt;
    #     self.size = sz;
    #     self.rect.setup_with_color(vec3(0.0, 1.0, 0.0))

    def draw(self, x:float, y:float) -> None:
        if (self.cursor_in_bounds(x, y)):
            self.label.color = vec3(0.2, 0.3, 0.2)
            self.rect.color = vec3(0.2, 0.3, 0.2)
        else:
            self.label.color = vec3(0.2, 0.2, 0.2)
            self.rect.color = vec3(0.2, 0.2, 0.2)

    def cursor_in_bounds(self, x:float, y:float) -> bool:
        # convert cursor position to NDC
        x = 2*x / float(WIDTH) - 1
        y = 2*y / float(HEIGHT)- 1

        hw = self.rect.size.x / float(WIDTH)
        hh = self.rect.size.y / float(HEIGHT)

        # convert button position to NDC
        self.ndc_position = vec2(2*self.rect.position.x / WIDTH -1, 2*self.rect.position.y / HEIGHT -1);

        topRight = vec2(self.ndc_position.x + hw,  self.ndc_position.y + hh);
        bottomRight = vec2(self.ndc_position.x + hw,  self.ndc_position.y - hh);
        bottomLeft = vec2(self.ndc_position.x - hw,  self.ndc_position.y - hh);
        topLeft = vec2(self.ndc_position.x - hw,  self.ndc_position.y + hh);

        # check cursor is within the button boundary
        if (x > topLeft.x and x < topRight.x and y > bottomLeft.y and y < topLeft.y):
            return True

        return False