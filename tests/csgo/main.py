import sys
sys.path.append("/home/me/Documents/3d-graphics-project/engine/bin")
sys.path.append("/home/me/Documents/3d-graphics-project/engine/utils")
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

WIDTH = 640
HEIGHT = 480

class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        make_context_current(self.window)
        set_cursor_visible(self.window, False)

        self.light = Light(vec3(-4,8,-2), vec3(1,1,1))

        self.ak47_icon = Rect(vec2(100 ,HEIGHT - 100), vec2(100,100), "./data/ak47.png")
        self.awp_icon = Rect(vec2(100 , HEIGHT - 100), vec2(100,100), "./data/awp.png")

        # simple UI
        self.crosshair = Rect(vec2(WIDTH/2, HEIGHT/2), vec2(25, 25), "./data/crosshair.png")
        self.scope = Rect(vec2(WIDTH/2, HEIGHT/2), vec2(WIDTH, HEIGHT), "./data/scope.png")
        self.scope.to_draw = False

        # FPS gun

        self.gun1 = AnimatedObject("./data/fps_hands_aks74u.fbx")
        self.gun1.set_frames(0.1, 0.101, 0.0)

        self.gun2 = AnimatedObject("./data/AWP/fps_hands_awp.dae")

        self.gun = self.gun1

        # create a 2nd player
        self.other_player = AnimatedObject("../angry-bots/data/unity_player.fbx")
        self.other_player.position = vec3(-40,-36.5,20)

        #self.other_player = StaticObject("./data/dj_skully/dj_scully.obj")
        self.other_player.model_matrix = translate(mat4(1.0), self.other_player.position)  
        self.other_player.model_matrix = scale(self.other_player.model_matrix, vec3(0.01, 0.01, 0.01))

        self.audio_window = AudioWindow()
        #self.audio_window.play("./data/theme.wav")
        #self.audio_window.set_volume("./data/theme.wav", 0.1)

        self.map = StaticObject("./data/de_dust2/de_dust2.dae")
        self.map.model_matrix = translate(self.map.model_matrix, vec3(21.2147, 30.0086, 28.7207) * -1.0)
        #self.map.model_matrix = translate(mat4(1.0), vec3(-1,0,0))
        #self.map.model_matrix = scale(self.map.model_matrix, vec3(3,3,3))


        self.console = Console(WIDTH, HEIGHT)

        self.show_shadows = True

        self.rlastX = 0
        self.rlastY = 0
        self.speed = 1

        self.map_position =self.map.model_matrix
        collision_objects = [self.map_position]
        v = [self.map.model]
        self.entity = CharacterEntity(v, collision_objects, vec3(0.7, 1.2, 0.7))

        self.active_camera.MovementSpeed = 70.0

        self.set_background_color(vec3(0.8, 0.9, 1.0))

        self.gravity = vec3(0,-10,0)
        self.jump = vec3(0,0,0)


    def update(self):
        self.processInput(self.window)
        self.console.update(self.currentFrame, self.deltaTime)
        #self.shadow_map_center = self.active_camera.position + self.active_camera.front * 10.0

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

        #self.gun.getFrame(self.currentFrame % 60)
        self.entity.velocity.x *= 0.1
        self.entity.velocity.z *= 0.1
        self.entity.velocity.y *= 0.1
        self.entity.velocity += self.gravity * self.deltaTime
        self.entity.velocity += self.jump * self.deltaTime
        self.entity.update()

        if (self.entity.grounded):
            self.jump.y = 0

        if (self.jump.y > 0):
            self.jump.y -= self.deltaTime * 20
        else:
            self.jump.y = 0
        self.active_camera.position = self.entity.position

        if (self.gun == self.gun1):
            self.gun.model_matrix = translate(mat4(1.0), self.active_camera.position)
            self.gun.model_matrix = rotate(self.gun.model_matrix, -math.radians(self.active_camera.yaw + 90.0), vec3(0,1,0))
            self.gun.model_matrix = rotate(self.gun.model_matrix, math.radians(self.active_camera.pitch), vec3(1,0,0))
            self.gun.model_matrix = translate(self.gun.model_matrix, vec3(0.2,-0.5, -0.1))
            self.gun2.set_to_draw = False
            self.gun1.set_to_draw = True
            self.gun1.render_to_ui = 1
            self.ak47_icon.to_draw = True
            self.awp_icon.to_draw = False

        elif (self.gun == self.gun2):
            self.gun.model_matrix = translate(mat4(1.0), self.active_camera.position)
            self.gun.model_matrix = scale(self.gun.model_matrix, vec3(3,3,3))
            self.gun.model_matrix = rotate(self.gun.model_matrix, -math.radians(self.active_camera.yaw + 90.0), vec3(0,1,0))
            self.gun.model_matrix = rotate(self.gun.model_matrix, math.radians(self.active_camera.pitch), vec3(1,0,0))
            self.gun.model_matrix = translate(self.gun.model_matrix, vec3(0.1,-0.0, 0))
            self.gun2.set_to_draw =  True
            self.gun1.set_to_draw =  False
            self.ak47_icon.to_draw = False
            self.awp_icon.to_draw = True

    def processInput(self, window):
        if (get_key(window, KEY_ESCAPE) == PRESS):
            set_window_should_close(self.window, True);
        speed = self.active_camera.MovementSpeed * self.deltaTime;
        total_velocity = vec3(0,0,0)
        if (get_key(window, KEY_W) == PRESS):
            total_velocity += self.active_camera.front
        if (get_key(window, KEY_S) == PRESS):
            total_velocity -= self.active_camera.front
        if (get_key(window, KEY_A) == PRESS):
            total_velocity -= self.active_camera.right
        if (get_key(window, KEY_D) == PRESS):
            total_velocity += self.active_camera.right

        self.entity.velocity = normalize(total_velocity) * speed

        if (get_key(window, KEY_R) == PRESS):
            self.entity.position = vec3(-24, -4.5, 53) + vec3(21.2147, 30.0086, 28.7207) * -1.0

        # if (get_key(self.window, KEY_ESCAPE) == PRESS):
        #     set_window_should_close(self.window, True);

    def onMouseMoved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos
        self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)

    def onMouseClicked(self, button, action, mods):
        print (MOUSE_BUTTON_2, button, action)
        if (button == MOUSE_BUTTON_1 and action == 0):
            self.audio_window.play("./data/awp.wav")
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
            ray_wor = vec3(inv_ray_wor.x, inv_ray_wor.y, inv_ray_wor.z)
            ray_wor = normalize(ray_wor)
            print (ray_wor)
            if (ray_intersect_sphere(self.entity.position, ray_wor, self.other_player.position, 2.0)):
                print ("player 2 hit!")
                self.other_player.color = vec3(1,0,0)
                self.other_player.set_frames(6.0, 8.0, 0.0)   
            else:
                self.other_player.color = vec3(-1,-1,-1)
                self.other_player.set_frames(0.2, 2.7, 0.0)  

        if (button == MOUSE_BUTTON_2 and action == 0):
            # toggle scope on rmb click
            if self.scope.to_draw is False:
                self.active_camera.projection_matrix = perspective(math.radians(15.0), float(WIDTH)/float(HEIGHT), 0.1, 100.0)
                self.scope.to_draw = True
                self.gun1.set_to_draw =  False
                self.gun2.set_to_draw =  False
                self.crosshair.to_draw = False

            elif (self.scope.to_draw):
                self.scope.to_draw = False
                self.gun1.set_to_draw =  False
                self.gun2.set_to_draw =  False
                self.crosshair.to_draw = True
                self.active_camera.projection_matrix = perspective(math.radians(45.0), float(WIDTH)/float(HEIGHT), 0.1, 1000.0)

    def onWindowResized(self, width, height):
        pass

    def onKeyPressed(self, key, scancode, action, mods):
        self.console.onKeyPressed(key, scancode, action, mods)
        if (key == KEY_1 and action == 1):
            self.gun = self.gun1
        if (key == KEY_2 and action == 1):
            self.gun = self.gun2
        if (key == KEY_V and action == 1):
            self.debug = True
        elif (key == KEY_V and action == 0):
            self.debug = False
        if (key == KEY_SPACE and action == 1):
            if (self.jump == vec3(0,0,0)):
                self.jump = vec3(0,16,0)

if __name__ == "__main__":
    app = App("csgo", WIDTH, HEIGHT, False)
    run(app)
