import sys
sys.path.append("/home/me/Documents/3d-graphics-project/engine/bin")
sys.path.append("/home/me/Documents/3d-graphics-project/engine/utils")
from console import *
from axis_3d import *
from engine.graphics import *
#from engine.core.physics import collision_SAT
from keys import *

from button import *
import random
import time
import math
from OpenGL.GL import *
import numpy
import queue

from rubik import solve
from rubik.cube import Cube
from rubik.solve import Solver
from rubik.optimize import optimize_moves

SOLVED_CUBE_STR = "WWWWWWWWWRRRBBBOOOGGGRRRBBBOOOGGGRRRBBBOOOGGGYYYYYYYYY"
MOVES = ["Li", "Ri", "Ui", "Di", "Fi", "Bi", "Mi", "Ei", "Si", "L", "R", "U", "D", "F", "B", "M", "E", "S"]

seed = 42 #54071 #random.randrange(0, 100, 1)
print (seed)
random.seed(seed)


WIDTH = 800
HEIGHT = 600

def play_frames(frame_interval, current_time):
    start, end = frame_interval
    mantissa = current_time - int(current_time)
    return current_time % (end-start) + start

class App(Application):

    original_top_side = [6, 7, 8, 15, 16, 17, 24, 25, 26]
    original_bottom_side = [0, 1, 2, 9, 10, 11, 18, 19, 20]
    original_back_side = [0, 1, 2, 3, 4, 5, 6, 7, 8]
    original_front_side = [18, 19, 20, 21, 22, 23, 24, 25, 26]
    original_left_side = [0, 9, 18, 3, 12, 21, 6, 15, 24]
    original_right_side = [2, 11, 20, 5, 14, 23, 8, 17, 26]
    original_middle_side = [1, 10, 19, 4, 13, 22, 7, 16, 25]
    original_equator_side = [3, 4, 5, 12, 13, 14, 21, 22, 23]
    original_standing_side = [9, 10, 11, 12, 13, 14, 15, 16, 17]

    top = original_top_side 
    bottom = original_bottom_side 
    back = original_back_side 
    front = original_front_side 
    left = original_left_side
    right = original_right_side 
    middle = original_middle_side
    equator = original_equator_side
    standing = original_standing_side
    scamble_moves = ''
    front_face = "orange"

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        set_cursor_visible(self.window, True)

        self.all_blocks = []
        self.all_blocks_transforms = []

        self.counter = 0
        for k in range(-1, 2):
            for j in range(-1, 2):
                for i in range(-1, 2):

                    tmp_model = StaticModel("./data/block.obj")
                    tmp_model.id = self.counter
                    self.counter+=1
                    self.all_blocks.append(tmp_model)
                    tmp_transform = mat4(1.0)
                    tmp_transform = translate(tmp_transform, vec3(2*i, 2*j, 2*k))
                    tmp_model.model_matrix = tmp_transform
                    self.all_blocks_transforms.append(tmp_transform)

        self.console = Console(WIDTH, HEIGHT)

        self.rect = Rect(vec2(100,100),vec2(100,100), "")
        self.rect.color = vec3(0,1,0)
        self.label = Label("text", vec2(100,100), "../minecraft/data/Minecraftia.ttf", 1)
        self.fps_label = Label("", vec2(30,HEIGHT - 50), "../minecraft/data/Minecraftia.ttf", 0.5)
        self.rect.setup_with_color(vec3(1,0,0))
        self.show_shadows = True
        self.show_ssao = False
        self.light = Light(vec3(0, 1, 4), vec3(1,1,1))

        self.angle = self.active_camera.yaw
        self.axes_map = {"F":"F", "B":"B", "L":"L", "R":"R", "Y":"Y", "T":"T", "U":"U","Ui":"Ui", "D": "D", "Ti":"Ti", "Di": "Di", "Fi":"Fi", "Bi":"Bi", "Li":"Li", "Ri":"Ri", "Yi":"Yi", "M":"M", "E":"E", "S":"S", "Mi":"Mi", "Ei":"Ei", "Si":"Si", "Z":"Z", "Zi":"Zi", "X":"X", "Xi":"Xi"}
        self.animation_queue = queue.Queue()


        self.left_cw_button = Button("L", vec2(100,30), vec2(50, 50))
        self.left_ccw_button = Button("Li", vec2(200,30), vec2(50, 50))
        self.right_cw_button = Button("R", vec2(100,100), vec2(50, 50))
        self.right_ccw_button = Button("Ri", vec2(200,100), vec2(50, 50))
        self.top_cw_button = Button("U", vec2(100,170), vec2(50, 50))
        self.top_ccw_button = Button("Ui", vec2(200,170), vec2(50, 50))
        self.bottom_cw_button = Button("D", vec2(100,240), vec2(50, 50))
        self.bottom_ccw_button = Button("Di", vec2(200,240), vec2(50, 50))
        self.front_cw_button = Button("F", vec2(100,310), vec2(50, 50))
        self.front_ccw_button = Button("Fi", vec2(200,310), vec2(50, 50))


        self.rotation_step_x = 1
        self.rotation_step_y = 1
        self.rotation_step_z = 1

        self.buttons = [self.left_cw_button, \
                        self.left_ccw_button, \
                        self.right_cw_button, \
                        self.right_ccw_button, \
                        self.top_cw_button, \
                        self.top_ccw_button, \
                        self.bottom_cw_button, \
                        self.bottom_ccw_button, \
                        self.front_cw_button, \
                        self.front_ccw_button]

        self.scramble_moves = ''
        self.cube = self.random_cube()
        l = self.scramble_moves.split(sep=" ")
        for move in l:
            if (move == "E"):
                self.queue_animation(self.axes_map["Ei"])
            elif (move == "Ei"):
                self.queue_animation(self.axes_map["E"])
            elif (move == "M"):
                self.queue_animation(self.axes_map["Mi"])
            elif (move == "Mi"):
                self.queue_animation(self.axes_map["M"])
            else:
                self.queue_animation(self.axes_map[move])        
        # print (self.cube) 
        moves = self.solve()
        #set_clear_color(random.uniform(0.0,1.0), random.uniform(0.0,1.0),random.uniform(0.0,1.0))
    
        print (moves)
        for move in moves:
            if (move == "E"):
                self.queue_animation(self.axes_map["Ei"])
            elif (move == "Ei"):
                self.queue_animation(self.axes_map["E"])
            elif (move == "M"):
                self.queue_animation(self.axes_map["Mi"])
            elif (move == "Mi"):
                self.queue_animation(self.axes_map["M"])
            else:
                self.queue_animation(self.axes_map[move])  

    def random_cube(self):
        """
        :return: A new scrambled Cube
        """
        self.scramble_moves = " ".join(random.choices(MOVES, k=20))
        a = Cube(SOLVED_CUBE_STR)
        print (self.scramble_moves)
        a.sequence(self.scramble_moves)
        return a


    def solve(self):

        self.solver = Solver(self.cube)

        start = time.time()
        self.solver.solve()
        duration = time.time() - start

        if self.cube.is_solved():
            opt_moves = optimize_moves(self.solver.moves)
            return opt_moves
        return self.solver.moves


    def to_3d_array(self, array):
        return [array[:3], array[3:6], array[6:]]


    def queue_animation(self, face):
        for i in range(45):
            self.animation_queue.put(face)

    def rotate_camera(self, axis_tag):
        if (axis_tag == "Y"):

            #self.camera.Position += self.camera.Right * math.radians(2)
            #self.view = lookAt(self.camera.Position, vec3(0,0,0), vec3(0,1,0))
            radius = length(vec3(self.active_camera.position.x, 0.0, self.active_camera.position.z))
            camX = math.sin(math.radians(2) * self.rotation_step_y) * radius
            camZ = math.cos(math.radians(2) * self.rotation_step_y) * radius
            self.active_camera.position = vec3(camX, self.active_camera.position.y, camZ)
            self.active_camera.yaw -= 2
            #print (self.angle)
            #print ("position:", self.camera.Position)
            self.active_camera.updateCameraVectors()
            self.rotation_step_y += 1
        elif (axis_tag == "Yi"):
            #self.camera.Position += self.camera.Right * math.radians(2)
            #self.view = lookAt(self.camera.Position, vec3(0,0,0), vec3(0,1,0))
            radius = length(vec3(self.active_camera.position.x, 0.0, self.active_camera.position.z))
            camX = math.sin(math.radians(2) * self.rotation_step_y) * radius
            camZ = math.cos(math.radians(2) * self.rotation_step_y) * radius
            self.active_camera.position = vec3(camX, self.active_camera.position.y, camZ)
            self.active_camera.yaw += 2
            #print (self.angle)
            #print ("position:", self.camera.Position)
            self.active_camera.updateCameraVectors()
            self.rotation_step_y -= 1            
        elif (axis_tag == "X"):

            #self.camera.Position += self.camera.Right * math.radians(2)
            #self.view = lookAt(self.camera.Position, vec3(0,0,0), vec3(0,1,0))
            radius = length(vec3(0.0, self.active_camera.position.y, self.active_camera.position.z))
            camY = math.sin(math.radians(2) * self.rotation_step_z) * radius
            camZ = math.cos(math.radians(2) * self.rotation_step_z) * radius
            self.active_camera.position = vec3(self.active_camera.position.x, camY, camZ)
            if (self.front_face == "blue"):
                self.active_camera.pitch += 2
            else:
                self.active_camera.pitch -= 2
            #print (self.angle)
            #print ("position:", self.camera.Position)
            self.active_camera.updateCameraVectors()
            self.rotation_step_z += 1
        elif (axis_tag == "Xi"):
            #self.camera.Position += self.camera.Right * math.radians(2)
            #self.view = lookAt(self.camera.Position, vec3(0,0,0), vec3(0,1,0))
            radius = length(vec3(0.0, self.camera.position.y, self.camera.position.z))
            camY = math.sin(math.radians(2) * self.rotation_step_z) * radius
            camZ = math.cos(math.radians(2) * self.rotation_step_z) * radius
            self.active_camera.position = vec3(self.active_camera.position.x, camY, camZ)
            if (self.front_face == "blue"):
                self.active_camera.pitch -= 2
            else:
                self.active_camera.pitch += 2
            #print (self.angle)
            #print ("position:", self.camera.Position)
            self.active_camera.updateCameraVectors()
            self.rotation_step_z -= 1  
        return None

    def rotate_face(self, face_tag):
        if (face_tag in ["F", "R", "U", "Bi", "Li", "Di", "M", "E", "S"]):
            rot_dir = -2
        elif (face_tag in ["Fi", "Ri", "Ui", "B", "L", "D", "Mi", "Ei", "Si"]):
            rot_dir = 2

        if (face_tag in ["F", "Ri", "Ui", "Bi", "L", "D", "Mi", "Ei", "S"]):
            no_rots = 1
        elif (face_tag in ["Fi", "R", "U", "B", "Li", "Di", "M", "E", "Si"]):
            no_rots = 3

        if (face_tag in ["F", "Fi", "B", "Bi", "S", "Si"]):
            rot_axis = vec3(0,0,1)
        elif (face_tag in ["L", "Li", "R", "Ri", "M", "Mi"]):
            rot_axis = vec3(1,0,0)
        elif (face_tag in ["U", "Ui", "D", "Di", "E", "Ei"]):
            rot_axis = vec3(0,1,0)
        else:
            return self.rotate_camera(face_tag)

        face = self.faces_map[face_tag]
        face_matrix = self.to_3d_array(face)

        for i,block in enumerate(self.all_blocks):
            if (i in face):
                current_block_model = rotate(mat4(1.0), math.radians(rot_dir), rot_axis)
                self.all_blocks_transforms[i] = current_block_model  * self.all_blocks_transforms[i]
                rotate_face = numpy.ndarray.flatten(numpy.rot90(face_matrix,k=no_rots))

        return rotate_face

    def set_front_face(self, x, y):
        self.angle = math.degrees(numpy.arctan((y + 0.001)/(x + 0.001)))
        # 2nd quad
        if (x < 0 and y > 0):
            self.angle = 180.0 + self.angle
        # 3rd quad
        elif (x < 0 and y < 0):
            self.angle = 180.0 + self.angle
        # 4th quadangle
        elif (x > 0 and y < 0):
            self.angle = 360.0 + self.angle
        #print (angle)
        if self.angle > 225 and self.angle < 315:
            #print ("green")
            self.front_face = "green"
            self.axes_map = {"F":"F", "B":"B", "L":"L", "R":"R", "Y":"Y", "T":"T", "U":"U","Ui":"Ui", "D": "D", "Ti":"Ti", "Di": "Di", "Fi":"Fi", "Bi":"Bi", "Li":"Li", "Ri":"Ri", "Yi":"Yi", "M":"M", "E":"E", "S":"S", "Mi":"Mi", "Ei":"Ei", "Si":"Si", "Z":"Z", "Zi":"Zi", "X":"X", "Xi":"Xi"}

        elif self.angle > 315 or self.angle < 45:
            #print ("orange")
            self.front_face = "orange"
            self.axes_map = {"F":"R", "B":"L", "L":"F", "R":"B", "Y":"Y", "T":"T", "D": "D","U":"U","Ui":"Ui", "Ti":"Ti", "Di": "Di", "Fi":"Ri", "Bi":"Li", "Li":"Fi", "Ri":"Bi", "Yi":"Yi", "M":"S", "E":"E", "S":"M", "Mi":"Si", "Ei":"Ei", "Si":"Mi", "Z":"Z", "Zi":"Zi", "X":"X", "Xi":"Xi"}

        elif self.angle > 45 and self.angle < 135:
            #print ("blue")
            self.front_face = "blue"
            self.axes_map = {"F":"B", "B":"F", "L":"R", "R":"L", "Y":"Y", "T":"T", "D": "D","U":"U","Ui":"Ui", "Ti":"Ti", "Di": "Di","Fi":"Bi", "Bi":"Fi", "Li":"Ri", "Ri":"Li", "Yi":"Yi", "M":"M", "E":"E", "S":"Si", "Mi":"Mi", "Ei":"Ei", "Si":"S", "Z":"Z", "Zi":"Zi", "X":"X", "Xi":"Xi"}

        elif self.angle > 135 and self.angle < 225:
            #print ("red")
            self.axes_map = {"F":"L", "B":"R", "L":"B", "R":"F", "Y":"Y", "T":"T", "D": "D","U":"U","Ui":"Ui", "Ti":"Ti", "Di": "Di", "Fi":"Li", "Bi":"Ri", "Li":"Bi", "Ri":"Fi", "Yi":"Yi", "M":"S", "E":"E", "S":"Mi", "Mi":"Si", "Ei":"Ei", "Si":"M", "Z":"Z", "Zi":"Zi", "X":"X", "Xi":"Xi"}
            self.front_face = "red"
        self.set_faces_map()


    def set_faces_map(self):
        self.faces_map =   {"F":self.front, "Fi":self.front, \
                        "B":self.back, "Bi":self.back, \
                        "D":self.bottom, "Di":self.bottom, \
                        "U":self.top, "Ui":self.top, \
                        "L":self.left, "Li":self.left,\
                        "R":self.right, "Ri":self.right,\
                        "M": self.middle, "Mi":self.middle, \
                        "E":self.equator, "Ei": self.equator, \
                        "S":self.standing, "Si":self.standing,"Yi":None, "Y":None, "Z":None, "Zi":None, "X":None, "Xi":None}  

    def rotate_camera(self, axis_tag):
        if (axis_tag == "Y"):

            #self.camera.Position += self.camera.Right * math.radians(2)
            #self.view = lookAt(self.camera.Position, vec3(0,0,0), vec3(0,1,0))
            radius = length(vec3(self.active_camera.position.x, 0.0, self.active_camera.position.z))
            camX = math.sin(math.radians(2) * self.rotation_step_y) * radius
            camZ = math.cos(math.radians(2) * self.rotation_step_y) * radius
            self.active_camera.position = vec3(camX, self.active_camera.position.y, camZ)
            self.camera.yaw -= 2
            #print (self.angle)
            #print ("position:", self.camera.Position)
            self.active_camera.updateCameraVectors()
            self.rotation_step_y += 1
        elif (axis_tag == "Yi"):
            #self.camera.Position += self.camera.Right * math.radians(2)
            #self.view = lookAt(self.camera.Position, vec3(0,0,0), vec3(0,1,0))
            radius = length(vec3(self.active_camera.position.x, 0.0, self.active_camera.position.z))
            camX = math.sin(math.radians(2) * self.rotation_step_y) * radius
            camZ = math.cos(math.radians(2) * self.rotation_step_y) * radius
            self.active_camera.position = vec3(camX, self.active_camera.position.y, camZ)
            self.active_camera.yaw += 2
            #print (self.angle)
            #print ("position:", self.camera.Position)
            self.active_camera.updateCameraVectors()
            self.rotation_step_y -= 1            
        elif (axis_tag == "X"):

            #self.camera.Position += self.camera.Right * math.radians(2)
            #self.view = lookAt(self.camera.Position, vec3(0,0,0), vec3(0,1,0))
            radius = length(vec3(0.0, self.active_camera.position.y, self.active_camera.position.z))
            camY = math.sin(math.radians(2) * self.rotation_step_z) * radius
            camZ = math.cos(math.radians(2) * self.rotation_step_z) * radius
            self.active_camera.position = vec3(self.active_camera.position.x, camY, camZ)
            if (self.front_face == "blue"):
                self.camera.pitch += 2
            else:
                self.camera.pitch -= 2
            #print (self.angle)
            #print ("position:", self.camera.Position)
            self.active_camera.updateCameraVectors()
            self.rotation_step_z += 1
        elif (axis_tag == "Xi"):
            #self.camera.Position += self.camera.Right * math.radians(2)
            #self.view = lookAt(self.camera.Position, vec3(0,0,0), vec3(0,1,0))
            radius = length(vec3(0.0, self.active_camera.position.y, self.active_camera.position.z))
            camY = math.sin(math.radians(2) * self.rotation_step_z) * radius
            camZ = math.cos(math.radians(2) * self.rotation_step_z) * radius
            self.active_camera.position = vec3(self.active_camera.position.x, camY, camZ)
            if (self.front_face == "blue"):
                self.active_camera.pitch -= 2
            else:
                self.active_camera.pitch += 2
            #print (self.angle)
            #print ("position:", self.camera.Position)
            self.active_camera.updateCameraVectors()
            self.rotation_step_z -= 1  
        return None

    def update(self):

        self.processInput(self.window)
        self.fps_label.text = str(int(1.0/(self.deltaTime+0.00001)))
        self.console.update(self.currentFrame, self.deltaTime)
        self.light.position = self.active_camera.position
        cartesian_coords = vec3(self.active_camera.position.x, -self.active_camera.position.z, self.active_camera.position.y)
        normalized_position = normalize(cartesian_coords)
        self.set_front_face(normalized_position.x, normalized_position.y)
        if (self.animation_queue.qsize() != 0):
            next_animation = self.animation_queue.get()
            rotate_face = self.rotate_face(next_animation)
            if self.animation_queue.qsize() % 45 == 0:
                self.set_rotation(next_animation, rotate_face)

        for i,block in enumerate(self.all_blocks):
            block.model_matrix = self.all_blocks_transforms[i]

    def set_rotation(self, face, rotate_face):
        face = self.faces_map[face]
        new_blocks = self.all_blocks[:]
        new_blocks_transforms = self.all_blocks_transforms[:]
        if (face != None):
            for i in range(len(face)):
               self.all_blocks[face[i]] = new_blocks[rotate_face[i]]
               self.all_blocks_transforms[face[i]] = new_blocks_transforms[rotate_face[i]]             


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

    def onMouseClicked(self, button, action, mods):
        if (button == 2 and action == 1):
            set_cursor_visible(self.window, False)
        if (button == 2 and action == 0):
            self.lastX = WIDTH/2
            self.lastY = HEIGHT/2
            set_cursor_visible(self.window, True) 

        if (button == 0 and action == 1):
            # if (self.start != True):
            #     self.start = True
            # else:
            #     self.reverse = True

            for btn in self.buttons:
                if (btn.cursor_in_bounds(self.lastX, HEIGHT - self.lastY)):
                    self.queue_animation(self.axes_map[btn.text])

    def onWindowResized(self, width, height):
        pass

    def onKeyPressed(self, key, scancode, action, mods):
        self.console.onKeyPressed(key, scancode, action, mods)
        
if __name__ == "__main__":
    app = App("Rubik's Cube", WIDTH, HEIGHT, False)
    run(app)
