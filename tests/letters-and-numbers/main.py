import sys
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from engine.graphics import *
from keys import *
from player import *
from button import *

import random
import time
import math
import itertools
import threading

WIDTH =  960
HEIGHT = 540

class MainMenuScene():
    def __init__(self):
        self.bg = Rect(vec2(WIDTH/2, HEIGHT/2), vec2(WIDTH, HEIGHT), "./data/bg.png")
        self.bg.ordering = -1
        self.play_button = Button("PLAY", vec2(WIDTH/2,HEIGHT/4), vec2(150,70), "./data/button.png")
        self.play_button.set_text_alignment(vec2(-50,-20))

    def show(self):
        self.bg.ordering = 1
        self.play_button.show()

    def hide(self):
        self.bg.ordering = -1
        self.play_button.hide()

class SelectionScene():
    def __init__(self):
        self.bg = Rect(vec2(WIDTH/2, HEIGHT/2), vec2(WIDTH, HEIGHT), "./data/bg2.png")
        self.bg.ordering = -1
        self.letters_button = Button("LETTERS", vec2(300,HEIGHT/4), vec2(250,80), "./data/button.png")
        self.letters_button.set_text_alignment(vec2(-90,-20))
        self.numbers_button = Button("NUMBERS", vec2(WIDTH-300,HEIGHT/4), vec2(250,80), "./data/button.png")
        self.numbers_button.set_text_alignment(vec2(-90,-20))

    def show(self):
        self.bg.ordering = 1
        self.letters_button.show()
        self.numbers_button.show()

    def hide(self):
        self.bg.ordering = -1
        self.letters_button.hide()
        self.numbers_button.hide()

class LettersScene():
    vowels = ['A', 'E', 'I', 'O', 'U']
    consonants = ['B', 'C', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'V', 'W', 'X', 'Y', 'Z']
    def __init__(self):
        self.bg = Rect(vec2(WIDTH/2, HEIGHT/2), vec2(WIDTH, HEIGHT), "./data/letters.png")
        self.bg.ordering = 0
        self.vowel_button = Button("VOWEL", vec2(300,HEIGHT/3), vec2(250,80), "./data/button.png")
        self.vowel_button.show()
        self.vowel_button.set_text_alignment(vec2(-60,-20))
        self.consonant_button = Button("CONSONANT", vec2(WIDTH-300,HEIGHT/3), vec2(300,80), "./data/button.png")
        self.consonant_button.set_text_alignment(vec2(-120,-20))
        self.consonant_button.show()
        self.card_count = 0
        self.all_cards = []
        self.offset = 0

    def show(self):
        self.bg.ordering = 1
        self.vowel_button.show()
        self.consonant_button.show()
        for card in self.all_cards:
            card.show()

    def hide(self):
        self.bg.ordering = -1
        self.vowel_button.hide()
        self.consonant_button.hide()
        for card in self.all_cards:
            card.hide()

    def add_card(self, card_type):
        if (card_type == "vowel"):
            c = Button(random.choice(LettersScene.vowels), vec2(WIDTH/4 + self.offset, HEIGHT-120), vec2(75,75), "./data/card.png")
            c.set_text_alignment(vec2(-12, -16))
            c.show()
            self.all_cards.append(c)
        elif (card_type == "consonant"):
            c = Button(random.choice(LettersScene.consonants), vec2(WIDTH/4 + self.offset, HEIGHT-120), vec2(75,75), "./data/card.png")
            c.set_text_alignment(vec2(-12, -16))
            c.show()
            self.all_cards.append(c)
        self.card_count+=1
        self.offset += 77

    def reset(self):
        self.card_count = 0
        self.all_cards = []
        self.offset = 0        

class NumbersScene():
    smalls = [1,2,3,4,5,6,7,8,9,10]
    larges = [25, 50, 75, 100]
    def __init__(self):
        self.bg = Rect(vec2(WIDTH/2, HEIGHT/2), vec2(WIDTH, HEIGHT), "./data/numbers.png")
        self.bg.ordering = -1
        self.small_button = Button("SMALL", vec2(300,HEIGHT/4), vec2(150,70), "./data/button.png")
        self.small_button.set_text_alignment(vec2(-50,-20))
        self.small_button.show()
        self.large_button = Button("LARGE", vec2(WIDTH-300,HEIGHT/4), vec2(150,70), "./data/button.png")
        self.large_button.set_text_alignment(vec2(-50,-20))
        self.large_button.show()
        self.card_count = 0
        self.all_cards = []
        self.offset = 0

    def add_card(self, card_type):
        if (card_type == "small"):
            c = Button(str(random.choice(NumbersScene.smalls)), vec2(WIDTH/4 + self.offset, HEIGHT-120), vec2(75,75), "./data/card.png")
            c.set_text_alignment(vec2(-12, -16))
            c.show()
            self.all_cards.append(c)
        elif (card_type == "large"):
            c = Button(str(random.choice(NumbersScene.larges)), vec2(WIDTH/4 + self.offset, HEIGHT-120), vec2(75,75), "./data/card.png")
            c.set_text_alignment(vec2(-35, -16))
            c.show()
            self.all_cards.append(c)
        self.card_count+=1
        self.offset += 77

    def show(self):
        self.bg.ordering = 1
        self.small_button.show()
        self.large_button.show()
        for card in self.all_cards:
            card.show()

    def hide(self):
        self.bg.ordering = -1
        self.small_button.hide()
        self.large_button.hide()
        for card in self.all_cards:
            card.hide()
            
    def reset(self):
        self.card_count = 0 
        self.all_cards = []
        self.offset = 0     
        best_solution = ''

class LettersGameScene():

    duration = 0

    dictionary = []

    def __init__(self):
        self.bg = Rect(vec2(WIDTH/2, HEIGHT/2), vec2(WIDTH, HEIGHT), "./data/game.png")
        self.bg.ordering = -1
        self.board1 = Rect(vec2(200,HEIGHT-150), vec2(WIDTH/3, HEIGHT/3), "./data/board.png")
        self.board1.ordering = -1
        self.board2 = Rect(vec2(WIDTH/2, 100), vec2(WIDTH, HEIGHT/3), "./data/letters_dock.png")
        self.board2.ordering = -1
        with open("./data/dictionary.txt") as f:
            l = f.readline()
            LettersGameScene.dictionary = set(l.split(','))
        self.solver_thread = threading.Thread(target=self.generate_best_word)
        self.offset = 0
        self.all_cards = []
        self.click_count = 0
        self.answer = Label2D("", vec2(WIDTH/2, HEIGHT/2), "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf", 1)

    def generate_best_word(self):
        new_list = []
        all_words = []
        for i in range(len(self.all_letters)):
            x = list(itertools.permutations(''.join(self.all_letters), i))
            for s in x:
                r = ''.join(s)
                new_list.append(r)
        for word in set(new_list):
            if word in LettersGameScene.dictionary:
                all_words.append(word)

        self.best_word = max(all_words, key=len)
        print (self.best_word)

    def set_letters(self, letters):
        self.all_letters = letters
        for l in self.all_letters:
            c = Button(l.upper(), vec2(WIDTH/4 + self.offset, 75), vec2(75,75), "./data/card.png")
            c.set_text_alignment(vec2(-12, -16))
            c.show()
            self.all_cards.append(c)
            self.offset += 77

    def show(self):
        self.solver_thread = threading.Thread(target=self.generate_best_word)
        self.solver_thread.start()
        self.start_timer()
        self.bg.ordering = 1
        self.board1.ordering = 2
        self.board2.ordering = 2
        for c in self.all_cards:
            c.show()
        self.answer.to_draw = 1

    def hide(self):
        self.bg.ordering = -1
        self.board1.ordering = -1
        self.board2.ordering = -1
        for c in self.all_cards:
            c.hide()
        self.answer.to_draw = 0

    def reset(self):
        self.all_cards = []
        self.offset = 0
        self.click_count = 0
        self.answer.text = ''
        self.best_word = ''

    def start_timer(self):
        self.timer = time.time()

    def get_elapsed_time(self):
        return time.time() - self.timer    

class Expr:
    '''An Expr can be built with two different calls:
           -Expr(number) to build a literal expression
           -Expr(a, op, b) to build a complex expression. 
            There a and b will be of type Expr,
            and op will be one of ('+','-', '*', '/').
    '''
    def __init__(self, *args):
        if len(args) == 1:
            self.left = self.right = self.op = None
            self.value = args[0]
        else:
            self.left = args[0]
            self.right = args[2]
            self.op = args[1]
            if self.op == '+':
                self.value = self.left.value + self.right.value
            elif self.op == '-':
                self.value = self.left.value - self.right.value
            elif self.op == '*':
                self.value = self.left.value * self.right.value
            elif self.op == '/':
                self.value = self.left.value // self.right.value

    def __str__(self):
        '''It can be done smarter not to print redundant parentheses,
           but that is out of the scope of this problem.
        '''
        if self.op:
            return "({0}{1}{2})".format(self.left, self.op, self.right)
        else:
            return "{0}".format(self.value)

OPS = ('+', '-', '*', '/')
best_solution = ''
def SearchTrees(current, target):
    global best_solution
    ''' current is the current set of expressions.
        target is the target number.
    '''
    for a,b in itertools.combinations(current, 2):
        current.remove(a)
        current.remove(b)
        for o in OPS:
            # This checks whether this operation is commutative
            if o == '-' or o == '/':
                conmut = ((a,b), (b,a))
            else:
                conmut = ((a,b),)

            for aa, bb in conmut:
                # You do not specify what to do with the division.
                # I'm assuming that only integer divisions are allowed.
                if o == '/' and (bb.value == 0 or aa.value % bb.value != 0):
                    continue
                e = Expr(aa, o, bb)
                # If a solution is found, print it
                if e.value == target:
                    solution = str(e.value) + '=' + str(e)
                    if (best_solution == '' or len(solution) < len(best_solution)):
                        best_solution = solution
                        print (best_solution)
                current.add(e)
                # Recursive call!
                SearchTrees(current, target)
                # Do not forget to leave the set as it were before
                current.remove(e)
        # Ditto
        current.add(b)
        current.add(a)  

class NumbersGameScene():

    duration = 0

    def __init__(self):
        self.bg = Rect(vec2(WIDTH/2, HEIGHT/2), vec2(WIDTH, HEIGHT), "./data/game.png")
        self.bg.ordering = -1
        self.board1 = Rect(vec2(200,HEIGHT-150), vec2(WIDTH/3, HEIGHT/3), "./data/board.png")
        self.board1.ordering = -1
        self.board2 = Rect(vec2(WIDTH/2, 100), vec2(WIDTH, HEIGHT/3), "./data/numbers_dock.png")
        self.board2.ordering = -1
        self.all_numbers = []
        self.solver_thread = threading.Thread(target=self.generate_solution)
        self.offset = 0
        self.all_cards = []
        self.click_count = 0
        self.answer = Label2D("", vec2(WIDTH/2, HEIGHT/2), "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf", 1)
        self.target = Label2D("", vec2(WIDTH/2, 200), "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf", 1)

    def generate_solution(self):
        self.target.text = str(random.randrange(101,999,1))
        SearchTrees(set(map(Expr, self.all_numbers)), int(self.target.text))

    def show(self):
        self.solver_thread = threading.Thread(target=self.generate_solution)
        self.solver_thread.start()
        self.start_timer()
        self.bg.ordering = 1
        self.board1.ordering = 2
        self.board2.ordering = 2
        for c in self.all_cards:
            c.show()
        self.answer.to_draw = 1
        self.target.to_draw = 1

    def hide(self):
        self.bg.ordering = -1
        self.board1.ordering = -1
        self.board2.ordering = -1
        for c in self.all_cards:
            c.hide()
        self.answer.to_draw = 0
        self.target.to_draw = 0

    def start_timer(self):
        self.timer = time.time()

    def get_elapsed_time(self):
        return time.time() - self.timer   

    def set_numbers(self, numbers):
        self.all_numbers = numbers
        for n in self.all_numbers:
            c = Button(str(n), vec2(WIDTH/4 + self.offset, 75), vec2(75,75), "./data/card.png")
            c.set_text_alignment(vec2(-35, -16))
            c.show()
            self.all_cards.append(c)
            self.offset += 77

    def reset(self):
        self.all_cards = []
        self.offset = 0
        self.click_count = 0
        self.answer.text = ''
        self.target.text = ''

class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        set_cursor_visible(self.window, True)
        self.sky_box.load_skybox = False

        self.letters_scene = LettersScene()
        self.letters_scene.hide()
        self.numbers_scene = NumbersScene()
        self.numbers_scene.hide()
        self.letters_game_scene = LettersGameScene()
        self.letters_game_scene.hide()
        self.numbers_game_scene = NumbersGameScene()
        self.numbers_game_scene.hide()
        self.selection_scene = SelectionScene()
        self.selection_scene.hide()
        self.main_menu_scene = MainMenuScene()
        self.main_menu_scene.show()

        self.audio_window = AudioWindow()
        self.audio_window.play("./data/intro.wav")
        self.audio_window.set_volume("./data/intro.wav", 0.0)

        self.current_scene = None
        self.set_scene(self.main_menu_scene)

    def set_scene(self, scene):
        if (not self.current_scene):
            self.current_scene = scene
        else:
            self.current_scene.hide()
            self.current_scene = scene
            self.current_scene.show()

    def update(self):
        self.process_input(self.window)


    def process_input(self, window):
        if (get_key(window, KEY_ESCAPE) == PRESS):
            set_window_should_close(self.window, True);

    def on_mouse_moved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos

        self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)


    def on_mouse_clicked(self, button, action, mods):

        if (button == 0):

            if (isinstance(self.current_scene, MainMenuScene)):
                if (self.current_scene.play_button.cursor_in_bounds(self.lastX, HEIGHT - self.lastY)):
                    if (action == 0):
                        self.current_scene.play_button.rect.shaded = 0
                        self.set_scene(self.selection_scene)
                    else:
                        self.current_scene.play_button.rect.shaded = 1

            elif (isinstance(self.current_scene, SelectionScene)):
                if (self.current_scene.letters_button.cursor_in_bounds(self.lastX, HEIGHT - self.lastY)):
                    if (action == 0):
                        self.current_scene.letters_button.rect.shaded = 0
                        self.letters_scene.reset()
                        self.set_scene(self.letters_scene)
                    else:
                        self.current_scene.letters_button.rect.shaded = 1

                elif (self.current_scene.numbers_button.cursor_in_bounds(self.lastX, HEIGHT - self.lastY)):
                    if (action == 0):
                        self.current_scene.numbers_button.rect.shaded = 0
                        self.numbers_scene.reset()
                        self.set_scene(self.numbers_scene)
                    else:
                        self.current_scene.numbers_button.rect.shaded = 1


            elif (isinstance(self.current_scene, LettersGameScene)):
                if (self.current_scene.get_elapsed_time() > LettersGameScene.duration):
                    if (action == 0):
                        print (self.current_scene.click_count)
                        if self.current_scene.click_count == 1:
                            self.audio_window.stop("./data/clock.wav")
                            self.audio_window.play("./data/intro.wav")
                            for c in self.letters_scene.all_cards:
                                c.hide()
                            for c in self.numbers_scene.all_cards:
                                c.hide()
                            self.set_scene(self.main_menu_scene)
                        else:
                            self.current_scene.click_count += 1
                            self.current_scene.answer = Label2D(self.current_scene.best_word, vec2(WIDTH/2, HEIGHT/2), "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf", 1)

            elif (isinstance(self.current_scene, NumbersGameScene)):
                if (self.current_scene.get_elapsed_time() > NumbersGameScene.duration):
                    if (action == 0):
                        print (self.current_scene.click_count)
                        if self.current_scene.click_count == 1:
                            self.audio_window.stop("./data/clock.wav")
                            self.audio_window.play("./data/intro.wav")
                            for c in self.letters_scene.all_cards:
                                c.hide()
                            for c in self.numbers_scene.all_cards:
                                c.hide()
                            self.set_scene(self.main_menu_scene)
                        else:
                            self.current_scene.click_count += 1
                            self.current_scene.answer = Label2D(best_solution, vec2(WIDTH/4, HEIGHT/2), "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf", 1)                            

            elif (isinstance(self.current_scene, NumbersScene)):

                if (self.current_scene.small_button.cursor_in_bounds(self.lastX, HEIGHT - self.lastY)):
                    if (action == 0):
                        self.current_scene.small_button.rect.shaded = 0
                        self.current_scene.add_card("small")
                    else:
                        self.current_scene.small_button.rect.shaded = 1

                elif (self.current_scene.large_button.cursor_in_bounds(self.lastX, HEIGHT - self.lastY)):
                    if (action == 0):
                        self.current_scene.large_button.rect.shaded = 0
                        self.current_scene.add_card("large")
                    else:
                        self.current_scene.large_button.rect.shaded = 1


                if (self.current_scene.card_count == 6):

                    all_numbers = []
                    for card in self.current_scene.all_cards:
                        all_numbers.append(int(card.label.text.lower()))
                    self.numbers_game_scene.reset()
                    self.numbers_game_scene.set_numbers(all_numbers)
                    self.set_scene(self.numbers_game_scene)
                    self.audio_window.stop("./data/intro.wav")
                    self.audio_window.play("./data/clock.wav")

            elif (isinstance(self.current_scene, LettersScene)):

                if (self.current_scene.vowel_button.cursor_in_bounds(self.lastX, HEIGHT - self.lastY)):
                    if (action == 0):
                        self.current_scene.vowel_button.rect.shaded = 0
                        self.current_scene.add_card("vowel")
                    else:
                        self.current_scene.vowel_button.rect.shaded = 1
                elif (self.current_scene.consonant_button.cursor_in_bounds(self.lastX, HEIGHT - self.lastY)):
                    if (action == 0):
                        self.current_scene.consonant_button.rect.shaded = 0
                        self.current_scene.add_card("consonant")
                    else:
                        self.current_scene.consonant_button.rect.shaded = 1

                if (self.current_scene.card_count == 8):
                    all_letters = []
                    for card in self.current_scene.all_cards:
                        all_letters.append(card.label.text.lower())
                    self.letters_game_scene.reset()
                    self.letters_game_scene.set_letters(all_letters)
                    self.set_scene(self.letters_game_scene)
                    self.audio_window.stop("./data/intro.wav")
                    self.audio_window.play("./data/clock.wav")

    def on_window_resized(self, width, height):
        pass

    def on_key_pressed(self, key, scancode, action, mods):
        pass

if __name__ == "__main__":
    app = App("letters-and-numbers", WIDTH, HEIGHT, False, False)
    run(app)
