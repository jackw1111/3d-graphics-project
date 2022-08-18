import sys
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from engine.graphics import *
from engine.physics import *
from console import *
from axis_3d import *
from third_person_camera import *
from grid import *
from keys import *
import random
import time
import math
from OpenGL.GL import *

WIDTH = 800
HEIGHT = 600

def myround(x, base=4):
    return base * round(x/base)


class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        make_context_current(self.window)
        set_cursor_visible(self.window, False)
        self.console = Console(WIDTH, HEIGHT)
        self.shadow_map_resolution = 2048
        self.light = Light(vec3(0, 1, 4), vec3(1,1,1))
        glLineWidth(5)
        self.control_camera = True
        #self.axis_3d = Axis3D()

        self.active_camera.position = vec3(0,5,5)

        self.gravity = vec3(0,-10,0)
        self.contacts = ContactVector()
        self.joints = SphericalJointVector()
        self.aBodies = RigidBodyVector()
        self.object_models = []
        self.dt = 0.02

        self.crosshair = Rect2D(vec2(WIDTH/2, HEIGHT/2), vec2(25, 25), "../csgo/data/crosshair.png",1,1)
        self.crosshair.ordering = 1

        self.dock = Rect2D(vec2(WIDTH/2 - 25.0, HEIGHT/10), vec2(500, 50), "./data/dock.png",1,1)
        self.dock.ordering = 2
        spacing = 50.0
        self.grass_block_img = Rect2D(vec2(WIDTH/2 - spacing * 5.0, HEIGHT/10), vec2(40, 40), "./data/grass_block.png",1,1)
        self.grass_block_img.ordering = 3

        self.sand_block_img = Rect2D(vec2(WIDTH/2 - spacing * 4.0, HEIGHT/10), vec2(40, 40), "./data/sand_block.png",1,1)
        self.sand_block_img.ordering = 3

        self.gravel_block_img = Rect2D(vec2(WIDTH/2 - spacing * 3.0, HEIGHT/10), vec2(40, 40), "./data/gravel_block.png",1,1)
        self.gravel_block_img.ordering = 3

        self.wood_block_img = Rect2D(vec2(WIDTH/2 - spacing * 2.0, HEIGHT/10), vec2(40, 40), "./data/wood_block.png",1,1)
        self.wood_block_img.ordering = 3

        self.water_block_img = Rect2D(vec2(WIDTH/2 - spacing * 1.0, HEIGHT/10), vec2(40, 40), "./data/water_block.png",1,1)
        self.water_block_img.ordering = 3

        self.tree_block_img = Rect2D(vec2(WIDTH/2, HEIGHT/10), vec2(40, 40), "./data/tree_block.png",1,1)
        self.tree_block_img.ordering = 3

        self.leaves_block_img = Rect2D(vec2(WIDTH/2 + spacing * 1.0, HEIGHT/10), vec2(40, 40), "./data/leaves_block.png",1,1)
        self.leaves_block_img.ordering = 3

        self.selection_box_img = Rect2D(vec2(WIDTH/2 - spacing * 4.0, HEIGHT/10), vec2(40, 40), "./data/selection_box.png",1,1)
        self.selection_box_img.ordering = 4

        self.player = AnimatedObject("./data/steve.dae")
        self.player.model_matrix = mat4(1.0)
        self.player.model_matrix = rotate(self.player.model_matrix, math.radians(-90.0), vec3(1,0,0))
        self.player.model_matrix = scale(self.player.model_matrix, vec3(0.1, 0.1, 0.1))
        self.player.set_to_draw = True
        # BUG: have to set color otherwise AnimatedObject doesnt draw!
        self.player.color = vec4(0,0,0,1)
        self.player.offset = 0.0

        body = RigidBody2()
        body.friction = 0.0
        body.coefficient_of_restitution = 0.0
        init_capsule(body)
        body.inverse_body_inertia_tensor = mat3(vec3(0,0,0),
                                               vec3(0,0,0),
                                               vec3(0,0,0))
        body.cm_position = vec3(0,7,0)
        #body.object.set_to_draw = False
        body.mass = 50.0
        body.one_over_mass = 1.0 / body.mass

        self.aBodies.append(body)
        self.object_models.append(StaticObject("../dice/data/capsule.obj"))
        self.object_models[0].set_to_draw = False
        i = 0
        body = RigidBody2()
        init_box(body, 1.0, 1.0, 1.0)
        body.inverse_body_inertia_tensor = mat3(vec3(0,0,0),
                                               vec3(0,0,0),
                                               vec3(0,0,0))
        body.coefficient_of_restitution = 0.0
        body.one_over_mass = 0.0
        body.cm_position = vec3(i*2,0,i*2)
        self.block_type = "./data/sand_block.obj"
        #body.friction = 0.0
        self.aBodies.append(body)
        self.object_models.append(StaticObject("./data/grass_block.obj"))

        self.label = Label2D("press F to switch to between first and third person", vec2(WIDTH/3, HEIGHT/3), "./data/Minecraftia.ttf", 0.3)
        self.lines = []

        self.third_person_camera = ThirdPersonCamera(body.cm_position, vec3(0,0,-1), vec3(0,1,0), math.radians(0.0), math.radians(90.0))
        self.third_person_camera.distance = 10.0
        self.is_third_person = True

        self.last_jump_value = None
        self.current_jump_value = None

        self.kinematic_force = vec3(0,0,0)
        self.jump_force = vec3(0,0,0)

    def update(self):
        self.process_input(self.window)
        self.console.update(self.currentFrame, self.deltaTime)

        body = self.aBodies[0]
        body.cm_force = self.gravity
        body.cm_force += self.jump_force
        self.kinematic_force -= body.cm_velocity * 2.0
        body.cm_force += self.kinematic_force
        simulate(self.aBodies, self.contacts, self.joints, self.dt)

        #print (self.get_fps())
        for i in range(len(self.aBodies)):
            self.object_models[i].model_matrix = self.aBodies[i].get_model_matrix()

        self.player.offset += 0.01
        self.player.set_frames(0.0, 1.11, self.player.offset)
        self.lines = []

    def process_input(self, window):
        if (get_key(window, KEY_ESCAPE) == PRESS):
            set_window_should_close(self.window, True);
        if (self.is_third_person):
            self.active_camera.position = self.third_person_camera.get_position(self.active_camera, self.aBodies[0].cm_position)
        else:
            self.active_camera.position = self.aBodies[0].cm_position + vec3(0,1,0)
        self.player.model_matrix = mat4(1.0)
        self.player.model_matrix = translate(self.player.model_matrix, self.aBodies[0].cm_position + vec3(0,0.5,0))
        self.player.model_matrix = rotate(self.player.model_matrix, math.radians(-90.0), vec3(1,0,0))
        self.player.model_matrix = scale(self.player.model_matrix, vec3(0.3, 0.3, 0.3))

        self.active_camera.MovementSpeed = 20.0

        keypress = 0
        self.kinematic_force_forward = vec3(0,0,0)
        self.kinematic_force_side = vec3(0,0,0)

        if (get_key(window, KEY_W) == PRESS):
            self.kinematic_force_forward += self.active_camera.front
            keypress = 1
        elif (get_key(window, KEY_S) == PRESS):
            self.kinematic_force_forward  -= self.active_camera.front
            keypress = 1
        if (get_key(window, KEY_A) == PRESS):
            self.kinematic_force_side  -= self.active_camera.right
            keypress = 1
        elif (get_key(window, KEY_D) == PRESS):
            self.kinematic_force_side  += self.active_camera.right
            keypress = 1

        self.kinematic_force_forward = normalize(self.kinematic_force_forward) * 10.0
        self.kinematic_force_side = normalize(self.kinematic_force_side) * 10.0
        self.kinematic_force = self.kinematic_force_forward + self.kinematic_force_side

        self.current_jump_value = get_key(window, KEY_SPACE)
        if self.last_jump_value == RELEASE and self.current_jump_value == PRESS:
            self.jump_force = vec3(0,500,0)
        else:
            self.jump_force = vec3(0,0,0)
        self.last_jump_value = self.current_jump_value


    def wireframe_box(self, pos):
        hw = 1.0
        points =[vec3(-hw, hw, hw),
                vec3(-hw,-hw, hw),
                vec3( hw,-hw, hw),
                vec3( hw, hw, hw),
                vec3(-hw, hw,-hw),
                vec3(-hw,-hw,-hw),
                vec3( hw,-hw,-hw),
                vec3( hw, hw,-hw)]

        for i in range(len(points)):
            points[i] += pos


        self.l1 = Line3D(points[0], points[1])
        self.l2 = Line3D(points[1], points[2])
        self.l3 = Line3D(points[2], points[3])
        self.l4 = Line3D(points[3], points[0])

        self.l5 = Line3D(points[4], points[5])
        self.l6 = Line3D(points[5], points[6])
        self.l7 = Line3D(points[6], points[7])
        self.l8 = Line3D(points[7], points[4])

        self.l9 =  Line3D(points[0], points[4])
        self.l10 = Line3D(points[1], points[5])
        self.l11 = Line3D(points[2], points[6])
        self.l12 = Line3D(points[3], points[7])

        self.lines.append(self.l1)
        self.lines.append(self.l2)
        self.lines.append(self.l3)
        self.lines.append(self.l4)
        self.lines.append(self.l5)
        self.lines.append(self.l6)
        self.lines.append(self.l7)
        self.lines.append(self.l8)
        self.lines.append(self.l9)
        self.lines.append(self.l10)
        self.lines.append(self.l11)
        self.lines.append(self.l12)

    def on_mouse_moved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos
        if self.control_camera:
            self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)

        self.current_coord = vec3(self.lastX, HEIGHT-self.lastY, 0.0)

        ray_wor = ray_cast(WIDTH/2, HEIGHT/2, self.active_camera.view_matrix, self.active_camera.projection_matrix, WIDTH, HEIGHT)
        my_pos = vec3(myround(self.active_camera.position.x, 2), myround(self.active_camera.position.y, 2), myround(self.active_camera.position.z, 2))
        intersecting = 0.0

        for i in range(4,6,1):
            for j in range(1,len(self.aBodies)):
                pos  = self.aBodies[j].cm_position
                intersecting += ray_obb_intersection(self.active_camera.position, ray_wor, vec3(-1,-1,-1),vec3(1,1,1), translate(mat4(1.0), pos))

            new_pos = self.active_camera.position + self.active_camera.front * i
            new_pos = vec3(int(new_pos.x), int(new_pos.y), int(new_pos.z))
            new_pos = vec3(myround(new_pos.x, 2), myround(new_pos.y, 2), myround(new_pos.z, 2))

            if intersecting <= 0.0 and new_pos != my_pos and new_pos != my_pos - vec3(0,2,0) and new_pos != my_pos - vec3(0,4,0):
                self.wireframe_box(new_pos)
                break
            self.wireframe_box(vec3(1000,1000,1000))


    def on_mouse_clicked(self, button, action, mods):
        if (button == 0 and action == 1):

            ray_wor = ray_cast(WIDTH/2, HEIGHT/2, self.active_camera.view_matrix, self.active_camera.projection_matrix, WIDTH, HEIGHT)
            my_pos = vec3(myround(self.active_camera.position.x, 2), myround(self.active_camera.position.y, 2), myround(self.active_camera.position.z, 2))
            intersecting = 0.0

            for i in range(4,7,1):
                for j in range(1,len(self.aBodies)):
                    pos  = self.aBodies[j].cm_position
                    intersecting += ray_obb_intersection(self.active_camera.position, ray_wor, vec3(-1,-1,-1),vec3(1,1,1), translate(mat4(1.0), pos))

                new_pos = self.active_camera.position + self.active_camera.front * i
                new_pos = vec3(int(new_pos.x), int(new_pos.y), int(new_pos.z))
                new_pos = vec3(myround(new_pos.x, 2), myround(new_pos.y, 2), myround(new_pos.z, 2))
                #  
                if intersecting <= 0.0 and new_pos != my_pos and new_pos != my_pos - vec3(0,2,0) and new_pos != my_pos - vec3(0,4,0):

                    body = RigidBody2() 
                    init_box(body, 1.0, 1.0, 1.0)
                    body.inverse_body_inertia_tensor = mat3(vec3(0,0,0),
                                                           vec3(0,0,0),
                                                           vec3(0,0,0))
                    body.coefficient_of_restitution = 0.0
                    body.one_over_mass = 0.0
                    body.cm_position = new_pos
                    #body.friction = 0.0
                    self.aBodies.append(body)
                    self.object_models.append(StaticObject(self.block_type))
                    break

            # if (button == 2 and action == 1):
            #     self.control_camera = True
            #     set_cursor_visible(self.window, False)
            # if (button == 2 and action == 0):
            #     self.control_camera = False
            #     set_cursor_visible(self.window, True)

    def on_window_resized(self, width, height):
        pass

    def on_key_pressed(self, key, scancode, action, mods):
        self.console.on_key_pressed(key, scancode, action, mods)
        if (key == KEY_F and action == PRESS):
            if self.is_third_person:
                self.label.to_draw = False
                self.is_third_person = False
                self.player.set_to_draw = False
                #self.object_models[0].set_to_draw = False
                self.crosshair.to_draw = True
            else:
                self.is_third_person = True
                self.player.set_to_draw = True
                #self.object_models[0].set_to_draw = True
                self.crosshair.to_draw = False

        spacing = 50.0
        if (key == KEY_1 and action == PRESS):
            self.selection_box_img.position = vec2(WIDTH/2 + spacing * 1.0 - spacing * 6.0, HEIGHT/10)
            self.block_type = "./data/grass_block.obj"

        if (key == KEY_2 and action == PRESS):
            self.selection_box_img.position = vec2(WIDTH/2 + spacing * 2.0 - spacing * 6.0, HEIGHT/10)
            self.block_type = "./data/sand_block.obj"

        if (key == KEY_3 and action == PRESS):
            self.selection_box_img.position = vec2(WIDTH/2 + spacing * 3.0 - spacing * 6.0, HEIGHT/10)
            self.block_type = "./data/gravel_block.obj"

        if (key == KEY_4 and action == PRESS):
            self.selection_box_img.position = vec2(WIDTH/2 + spacing * 4.0 - spacing * 6.0, HEIGHT/10)
            self.block_type = "./data/wood_block.obj"

        if (key == KEY_5 and action == PRESS):
            self.selection_box_img.position = vec2(WIDTH/2 + spacing * 5.0 - spacing * 6.0, HEIGHT/10)
            self.block_type = "./data/water_block.obj"

        if (key == KEY_6 and action == PRESS):
            self.selection_box_img.position = vec2(WIDTH/2 + spacing * 6.0 - spacing * 6.0, HEIGHT/10)
            self.block_type = "./data/tree_block.obj"

        if (key == KEY_7 and action == PRESS):
            self.selection_box_img.position = vec2(WIDTH/2 + spacing * 7.0 - spacing * 6.0, HEIGHT/10)
            self.block_type = "./data/leaves_block.obj"

if __name__ == "__main__":
    app = App("test", WIDTH, HEIGHT, False, True)
    run(app)
