import sys
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from console import *
from axis_3d import *
from engine.graphics import *
#from engine.core.physics import collision_SAT
from keys import *
from player import *
import random
import time
import math
from OpenGL.GL import *
from third_person_camera import *

WIDTH = 640
HEIGHT = 480

class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        make_context_current(self.window)
        set_cursor_visible(self.window, False)

        self.light = Light(vec3(-4,8,-2), vec3(1,1,1))

        self.crosshair = Rect2D(vec2(WIDTH/2, HEIGHT/2), vec2(25, 25), "./data/crosshair.png",1,1)
        self.third_person_camera = ThirdPersonCamera(vec3(0,0,0), vec3(0,0,-1), vec3(0,1,0), 0.0, -90.0)
        self.third_person_camera.distance = 10.0
        # create a 2nd player
        # "../angry-bots/data/unity_player.fbx")
        self.other_player = AnimatedObject("./data/aot_enemy.dae")
        self.other_player_collision_mesh = AnimatedObject("./data/aot_enemy_collision_mesh.dae")
        self.other_player_collision_mesh.set_to_draw = False
        self.correct_orientation = translate(self.other_player.model_matrix, vec3(10,7.0,5))
        self.correct_orientation = scale(self.correct_orientation, vec3(1.5, 1.5, 1.5))
        self.other_player.model_matrix = rotate(self.correct_orientation, math.radians(-90.0), vec3(1,0,0))

        #self.other_player.draw_bounding_box = True
        #self.other_player.position = vec3(-40,-36.5,20)

        #self.other_player = StaticObject("./data/dj_skully/dj_scully.obj")
        #self.other_player.model_matrix = translate(mat4(1.0), self.other_player.position)  
        #self.other_player.model_matrix = scale(self.other_player.model_matrix, vec3(0.01, 0.01, 0.01))
        self.map1 = StaticObject("./data/map1.obj")
        self.map1.model_matrix = scale(self.map1.model_matrix, vec3(0.3,0.3,0.3))
        self.map1_collision_mesh = StaticObject("./data/map1_collision_mesh.obj")
        self.map1_collision_mesh.model_matrix = scale(self.map1_collision_mesh.model_matrix, vec3(0.3,0.3,0.3))
        self.map1_collision_mesh.set_to_draw = False

        self.player = AnimatedObject("./data/aot_player2.dae")
        self.player.model_matrix = translate(mat4(1.0), vec3(20,5,20))  
        self.player.model_matrix = scale(self.player.model_matrix, vec3(1.2, 1.2, 1.2))

        self.console = Console(WIDTH, HEIGHT)

        self.show_shadows = True

        self.rlastX = 0
        self.rlastY = 0
        self.speed = 1
        self.player_rotation = 90

        self.map_position =self.map1_collision_mesh.model_matrix
        collision_objects = [self.map_position]
        v = [self.map1_collision_mesh.model]
        self.entity = CharacterEntity(v, collision_objects, vec3(0.7, 1.2, 0.7))
        self.entity.add_static_model(v, collision_objects)
        self.active_camera.MovementSpeed = 70.0

        self.entity2 = CharacterEntity(v, collision_objects, vec3(0.7, 1.2, 0.7))

        self.set_background_color(vec3(0.8, 0.9, 1.0))

        self.gravity = vec3(0,-10,0)
        self.jump = vec3(0,0,0)
        self.t  = 0
        glLineWidth(5)
        self.i = 0
        #self.entity.add_animated_object(self.other_player)
        self.third_person = True
        self.entity.position = vec3(10,10,10)
        self.entity2.add_static_model(v, collision_objects)

        self.entity2.add_animated_object(self.other_player)
        self.entity2.position = vec3(10,10,10)
        self.ray_wor = vec3(0,0,0)
        self.swinging = False
        self.swing_dir = vec3(0,0,0)
        self.swing_line = Line3D(vec3(0,0,0), vec3(0,0,0))
        self.swing_line.color = vec3(0,1,0)
        self.swing_start_pos = vec3(0,0,0)

    def update(self):

        self.player.set_frames(2.5,5,self.t*8)
        self.t += 0.01

        self.entity2.add_animated_object(self.other_player_collision_mesh)

        self.process_input(self.window)
        self.console.update(self.currentFrame, self.deltaTime)

        if (is_joystick_present()):
            speed = self.active_camera.MovementSpeed
            axes = FloatVector()
            axes = get_joystick_axes()
            xpos = (axes[3] + 1)*WIDTH/2 
            ypos = (axes[4] + 1)*HEIGHT/2
            xoffset = xpos - self.lastX
            yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

            self.lastX = xpos
            self.lastY = ypos

            self.active_camera.ProcessMouseMovement(axes[3]*speed, axes[4]*speed, True)

            rxpos = (axes[1] + 1)*WIDTH/2 
            rypos = (axes[0] + 1)*HEIGHT/2

            rxoffset = rxpos - self.rlastX
            ryoffset = rypos - self.rlastY #reversed since y-coordinates go from bottom to top


            self.rlastX = rxpos
            self.rlastY = rypos
            tolerance = 25.0

            self.entity.velocity -= self.active_camera.front * axes[1] * self.deltaTime * speed
            self.entity.velocity += self.active_camera.right * axes[0] * self.deltaTime * speed


            if (math.fabs(WIDTH/2 - rxpos) > tolerance):
                self.active_camera.ProcessKeyboard(0, self.deltaTime*(WIDTH/2 - rxpos)*0.003)
            if (math.fabs(HEIGHT/2 - rypos) > tolerance):
                self.active_camera.ProcessKeyboard(2, self.deltaTime*(HEIGHT/2 - rypos)*-0.003)

        self.entity.velocity.x *= 0.1
        self.entity.velocity.z *= 0.1
        self.entity.velocity.y *= 0.1
        self.entity.velocity += self.jump * self.deltaTime
        self.entity.gravity = self.gravity * self.deltaTime

        self.entity.update()

        if (self.entity.grounded):
            self.jump.y = 0

        if (self.jump.y > 0):
            self.jump.y -= self.deltaTime * 20
        else:
            self.jump.y = 0


        self.entity2.velocity.x *= 0.1
        self.entity2.velocity.z *= 0.1
        self.entity2.velocity.y *= 0.1
        self.entity2.velocity += self.jump * self.deltaTime
        self.swing_dir += self.active_camera.front * 0.1
        #self.entity2.velocity = normalize(self.swing_dir) * 0.5
        print ("swinging: " , self.swinging)
        if (not self.swinging):
            self.entity2.gravity = self.gravity * self.deltaTime
        else:
            self.entity2.gravity = vec3(0,0,0)
        if (self.swinging):
            self.swing_line.set_endpoints(self.swing_start_pos, self.entity2.position)
        self.entity2.update()

        if (self.entity2.grounded):
            self.jump.y = 0

        if (self.jump.y > 0):
            self.jump.y -= self.deltaTime * 20
        else:
            self.jump.y = 0

        if (self.third_person):
            self.active_camera.position = self.third_person_camera.get_position(self.active_camera, self.entity2.position)

        self.correct_orientation = translate(mat4(1.0), self.entity2.position-vec3(0,1.2,0))
        self.correct_orientation = translate(self.correct_orientation, vec3(0,1,0))
        self.correct_orientation = scale(self.correct_orientation, vec3(0.7, 0.7, 0.7))
        #self.player.model_matrix = rotate(self.correct_orientation, math.radians(90.0), vec3(0,1,0))
        self.player.model_matrix = rotate(self.correct_orientation, math.radians(self.player_rotation), vec3(0,1,0))
        self.player.model_matrix = rotate(self.player.model_matrix, math.radians(-90.0), vec3(1,0,0))

        self.other_player.set_frames(0, 5.0, self.t*4)
        self.other_player_collision_mesh.set_frames(0, 5.0, self.t)

        self.start_pos = 0
        self.end_pos = 0

    def process_input(self, window):
        if (get_key(window, KEY_ESCAPE) == PRESS):
            set_window_should_close(self.window, True);
        speed = self.active_camera.MovementSpeed * self.deltaTime;
        total_velocity = vec3(0,0,0)
        if (get_key(window, KEY_W) == PRESS):
            total_velocity += self.active_camera.front
            self.player_rotation = 180 - self.active_camera.yaw - 90.0
        if (get_key(window, KEY_S) == PRESS):
            total_velocity -= self.active_camera.front
            self.player_rotation = 0 - self.active_camera.yaw - 90.0
        if (get_key(window, KEY_A) == PRESS):
            total_velocity -= self.active_camera.right
            self.player_rotation = -90 - self.active_camera.yaw - 90.0
        if (get_key(window, KEY_D) == PRESS):
            total_velocity += self.active_camera.right
            self.player_rotation = 90 - self.active_camera.yaw - 90.0

        self.entity2.velocity = normalize(total_velocity) * speed

        if (get_key(window, KEY_R) == PRESS):
            self.entity2.position = vec3(10,10,10)

        # if (get_key(self.window, KEY_ESCAPE) == PRESS):
        #     set_window_should_close(self.window, True);

    def on_mouse_moved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos
        self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)

    def on_mouse_clicked(self, button, action, mods):
        print (MOUSE_BUTTON_2, button, action)
        if (button == MOUSE_BUTTON_1 and action == 1):
            x = (2.0 * WIDTH/2) / WIDTH - 1.0
            y = 1.0 - (2.0 * HEIGHT/2) / HEIGHT
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
            self.ray_wor = vec3(inv_ray_wor.x, inv_ray_wor.y, inv_ray_wor.z)
            self.ray_wor = normalize(self.ray_wor)
            print (self.ray_wor)
            # if (ray_intersect_sphere(self.entity.position, ray_wor, self.other_player.position, 2.0)):
            #     print ("player 2 hit!")
            #     self.other_player.color = vec3(1,0,0)
            #     self.other_player.set_frames(6.0, 8.0, 0.0)   
            # else:
            #     self.other_player.color = vec3(-1,-1,-1)
            #     self.other_player.set_frames(0.2, 2.7, 0.0)  

            t =  ray_intersect_animated_object(self.active_camera.position, self.ray_wor, self.other_player)
            if (t):
                self.start_pos = self.entity2.position
                self.end_pos = self.active_camera.position + self.ray_wor * t
                self.swing_line.set_endpoints(self.start_pos, self.end_pos)
                self.swing_start_pos = self.end_pos
                self.swinging = True
                self.swing_dir = self.ray_wor
        
        if (button == MOUSE_BUTTON_1 and action == 0):
            self.swinging = False

    def on_window_resized(self, width, height):
        pass

    def on_key_pressed(self, key, scancode, action, mods):
        self.console.on_key_pressed(key, scancode, action, mods)
        if (key == KEY_V and action == 1):
            self.debug = True
        elif (key == KEY_V and action == 0):
            self.debug = False
        if (key == KEY_SPACE and action == 1):
            if (self.jump == vec3(0,0,0)):
                self.jump = vec3(0,24,0)

if __name__ == "__main__":
    app = App("aot", WIDTH, HEIGHT, False, False)
    run(app)
