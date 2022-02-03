import sys
sys.path.append("/home/me/Documents/3d-graphics-project/engine/bin")
sys.path.append("/home/me/Documents/3d-graphics-project/engine/utils")

from engine.graphics import *
from keys import *
import random
import math
from bounding_sphere import *

WIDTH = 800
HEIGHT = 600

class LineTrail:

    def __init__(self):

        self.positions = []
        self.no_of_lines = 100
        self.lines = []

    def update(self, pos):

        if (len(self.positions) < self.no_of_lines):
            self.positions.append(pos)
        else:
            self.positions = self.positions[1:]
            self.lines = self.lines[1:]
            self.positions.append(pos)

        pos1 = self.positions[len(self.positions)-2]
        pos2 = pos
        self.l = Line3D(pos1, pos2)
        self.l.color = vec3(0,1,0)
        self.lines.append(self.l)

class Rope():
    def __init__(self, pos):
        self.position = pos
        self.velocity = vec3(0,0,0)
        self.force = vec3(0,-10,0)

class SmokeTrail:

    def __init__(self):

        self.positions = []
        self.no_of_lines = 100
        self.lines = []

    def update(self, pos):

        if (len(self.positions) < self.no_of_lines):
            self.positions.append(pos)
        else:
            self.positions = self.positions[1:]
            self.lines = self.lines[1:]
            self.positions.append(pos)

        pos1 = self.positions[len(self.positions)-2]
        pos2 = pos
        self.l = Line3D(pos1, pos2)
        self.l.color = vec3(0,1,0)
        self.lines.append(self.l)

class HeliDemo(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        set_cursor_visible(self.window, False)
        self.set_background_color(vec3(173.0/255.0, 216.0/255.0, 230.0/255.0))
        self.light = Light(vec3(-3,3,1), vec3(1,1,1))
        self.active_camera.set_far_plane(100.0)

        self.heli_position = vec3(-40.0,-37,-15)
        self.heli = AnimatedObject("/home/me/Documents/3d-graphics-project/tests/heli/data/heli.dae")
        self.heli.set_frames(0.0, 5.0, 0.0)

        #self.heli.set_frames(0.0, 1.0, 0.0)
        self.map = StaticObject("/home/me/Documents/3d-graphics-project/tests/heli/data/mountains.obj")
        #self.map.model_matrix = translate(self.map.model_matrix, vec3(21.2147, 30.0086, 28.7207) * -1.0)
        self.map.model_matrix = scale(self.map.model_matrix, vec3(400,200,400))

        #self.plane.set_to_draw = False
        self.audio_window = AudioWindow()
        #self.audio_window.play("/home/me/Documents/3d-graphics-project/tests/heli/data/heli.wav")
        #self.audio_window.set_volume("/home/me/Documents/3d-graphics-project/tests/heli/data/heli.wav", 0.05)

        #self.axis = StaticObject("data/3d_axis.obj")

        self.active_camera.yaw = 0

        self.heli.front = self.active_camera.front
        self.heli.yaw = 0.0
        self.heli.pitch = 0.0
        self.heli.roll = 0.0

        self.heli.right = self.active_camera.right
        self.heli.up = self.active_camera.up
        self.heli.position = vec3(0,30,0)
        self.heli.speed = 0.0
        self.heli.velocity = vec3(0,0,0)
        self.heli_max_speed = 10.0
        self.gravity = vec3(0,-10,0)
        self.third_person = True
        self.tick = 0


        self.rope = Rope(self.heli.position + vec3(0,-10,0))
        #self.line_trail = LineTrail()

        v = [self.map.model]
        map_matrix = [self.map.model_matrix]
        self.entity = CharacterEntity(v, map_matrix, vec3(5.0, 5.0, 5.0))
        self.entity.add_static_model(v, map_matrix)
        self.first_mouse = False
        self.entity.position = vec3(-10,11.5,0)
        self.shift_pressed = False
        self.camera_offset = 0
        self.use_custom_view_matrix = True
        self._yaw = self.active_camera.yaw
        self.i = 0
        self.smoke_trail = []
        self.no_of_smoke_puffs = 100
        self.timer = 0

        self.rlastX = 0
        self.rlastY = 0

    def update(self):

        self.process_input(self.window)
        front = vec3(0,0,0)
        front.x = math.cos(math.radians(self.heli.yaw)) * math.cos(math.radians(self.heli.pitch))
        front.y = math.sin(math.radians(self.heli.pitch))
        front.z = math.sin(math.radians(self.heli.yaw)) * math.cos(math.radians(self.heli.pitch))

        self.heli.front = normalize(front)
        roll_mat = rotate(mat4(1.0), math.radians(self.heli.roll), self.heli.front)
        self.heli.right = normalize(cross(self.heli.front, vec3(0,1,0))); 
        right = vec4(self.heli.right.x, self.heli.right.y, self.heli.right.z, 1.0)
        right = roll_mat * right
        self.heli.right = vec3(right.x, right.y, right.z)
        self.heli.up    = normalize(cross(self.heli.right, self.heli.front))


        self.front_line = Line3D(self.entity.position + vec3(0,1,0), self.entity.position + vec3(0,1,0) + front)
        self.front_line.color = vec3(0,0,1)
        self.right_line = Line3D(self.entity.position + vec3(0,1,0), self.entity.position + vec3(0,1,0) + self.heli.right)
        self.right_line.color = vec3(1,0,0)
        self.up_line = Line3D(self.entity.position + vec3(0,1,0), self.entity.position + vec3(0,1,0) + self.heli.up)
        self.up_line.color = vec3(0,1,0)
        dv = self.heli.up * self.deltaTime * self.heli.speed

        self.heli.velocity += dv

        if (self.first_mouse):
            self.heli.velocity += self.gravity * self.deltaTime
        if (length(self.heli.velocity) > self.heli_max_speed):
            self.heli.velocity = normalize(self.heli.velocity) * self.heli_max_speed
        self.heli.position += self.heli.velocity * self.deltaTime

        self.heli.velocity.x *= 0.9999
        self.heli.velocity.z *= 0.9999

        self.heli.model_matrix = translate(mat4(1.0), self.heli.position)

        if (not self.third_person):
            self.heli.model_matrix = translate(self.heli.model_matrix, self.heli.up*0.5)

        self.heli.model_matrix = rotate(self.heli.model_matrix, math.radians(-self.heli.yaw), self.heli.up)
        self.heli.model_matrix = rotate(self.heli.model_matrix, math.radians(self.heli.pitch), self.heli.right)
        self.heli.model_matrix = rotate(self.heli.model_matrix, math.radians(self.heli.roll), self.heli.front)

        self.heli.model_matrix = rotate(self.heli.model_matrix, math.radians(-90.0), vec3(1,0,0))

        t = self.currentFrame - int(self.currentFrame)
        self.heli.set_frames(0.0, 5.0, self.currentFrame)   

        if (self.third_person):
            modelHeight = 20.0
            ytheta = -self.active_camera.pitch
            # algorithm from ThinMatrix video on third person cameras
            horizDist = modelHeight * math.cos(math.radians(ytheta))
            vertDist = modelHeight * math.sin(math.radians(ytheta))
            xtheta = self.active_camera.yaw - 90.0
            offsetx = horizDist * math.sin(math.radians(-xtheta))
            offsetz = horizDist * math.cos(math.radians(xtheta))

            self.active_camera.position = self.heli.position + vec3(-offsetx, vertDist, -offsetz) + vec3(0,4,0)
        else:
            self.active_camera.position = self.heli.position - self.heli.up*0.5 + self.heli.front * 0.7
        
        self.entity.velocity = self.heli.velocity
        self.entity.velocity.x *= 0.05
        self.entity.velocity.z *= 0.05
        self.entity.velocity.y *= 0.05

        self.entity.gravity = vec3(0,0,0)
        self.entity.update()
        self.heli.position = self.entity.position.copy()
        v = vec3(random.randrange(-20,20,1), random.randrange(-20,20,1), random.randrange(-20,20,1))
        #self.line_trail.update(self.heli.position)
        if (not self.third_person):
            self.active_camera.view_matrix = lookAt(self.active_camera.position, self.active_camera.position + self.active_camera.front, self.heli.up)
        else:
            self.active_camera.view_matrix = lookAt(self.active_camera.position, self.active_camera.position + self.active_camera.front, vec3(0,1,0))

        if (len(self.smoke_trail) < self.no_of_smoke_puffs):
            self.particle1 = Particle("./data/smoke_atlas.png", mat4(1.0),8,8)
            self.particle1.frame = random.randrange(1,64,1)
            _scale = random.uniform(1.0,4.0)
            self.particle1.model_matrix = scale(translate(mat4(1.0), self.heli.position-self.heli.up*2-self.heli.front*4), vec3(_scale, _scale, _scale))
            self.particle1.alpha = random.uniform(0.05, 0.1)
            self.smoke_trail.append(self.particle1)
        else:
            self.particle1 = self.smoke_trail[0]
            self.particle1.frame = random.randrange(1,64,1)
            self.particle1.alpha = random.uniform(0.05, 0.1)
            _scale = random.uniform(1.0,4.0)

            self.particle1.model_matrix = scale(translate(mat4(1.0), self.heli.position-self.heli.up*2-self.heli.front*4), vec3(_scale, _scale, _scale))
            self.smoke_trail.append(self.particle1)
            self.smoke_trail = self.smoke_trail[1:]

        for particle in self.smoke_trail:
            particle.row = int(particle.frame / 8)
            particle.col = particle.frame - 8 * particle.row
            particle.model_matrix = scale(translate(particle.model_matrix, vec3(0,self.deltaTime*0.5,0)), vec3(1+self.deltaTime, 1+self.deltaTime, 1+self.deltaTime))
            particle.frame += 1
            particle.alpha *=0.99

        #print (self.get_fps())
    def process_input(self, window):

        if (is_joystick_present()):
            print ('joysticks')
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


            if (not self.third_person):
                self.active_camera.pitch += axes[4]*speed

            self.heli.pitch += axes[4]*speed
            self.heli.roll += axes[3]*speed

            self.heli.yaw += axes[0]*speed
            if (not self.third_person):
                self.active_camera.yaw += axes[0]*speed

            if (xoffset):
                self.heli.speed = 30.0 * -axes[1]
            else:
                self.heli.speed = -10.0
            #print ("speed", self.heli.speed, xoffset)
            # if (math.fabs(WIDTH/2 - rxpos) > tolerance):
            #     self.active_camera.ProcessKeyboard(0, self.deltaTime*(WIDTH/2 - rxpos)*0.003)
            # if (math.fabs(HEIGHT/2 - rypos) > tolerance):
            #     self.active_camera.ProcessKeyboard(2, self.deltaTime*(HEIGHT/2 - rypos)*-0.003)


        if (get_key(window, KEY_ESCAPE) == PRESS):
            set_window_should_close(self.window, True);

        if (get_key(window, KEY_W) == PRESS):
            self.first_mouse = True

            #if (self.plane.pitch < 45.0):
            #self.heli.pitch -= 0.1
            self.heli.speed = 30.0

        if (get_key(window, KEY_S) == PRESS):
            #if (self.plane.pitch > -45.0):
            self.heli.speed = -30.0

        if (get_key(window, KEY_A) == PRESS):
            #if (self.plane.yaw < 45.0):
            #self.heli.yaw -= 1
            self.heli.yaw -= 1
            if (not self.third_person):
                self.active_camera.yaw += -1

        if (get_key(window, KEY_D) == PRESS):
            #if (self.plane.yaw > -45.0):
            #self.heli.yaw +=1
            self.heli.yaw += 1
            if (not self.third_person):
                self.active_camera.yaw -= -1

        # if (get_key(window, KEY_W) != PRESS and get_key(window, KEY_S) != PRESS):
        #     self.heli.speed = 0.0
        self.active_camera.yaw = self.heli.yaw
        if (self.shift_pressed):
            self.active_camera.yaw += self.camera_offset

    def on_mouse_moved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top
        if (self.shift_pressed):
            self.camera_offset += xoffset * 0.1

        self.lastX = xpos
        self.lastY = ypos

        if (not self.third_person):
            self.active_camera.pitch += -yoffset * self.deltaTime

        self.heli.pitch += -yoffset * self.deltaTime
        self.heli.roll += xoffset * self.deltaTime
        #self.active_camera.yaw += xoffset * self.deltaTime * 3.0

        #self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)

    def on_key_pressed(self, key, scancode, action, mods):
        # if (action == 1):
        #     if (key == KEY_F):
        #         if (self.third_person):
        #             self.third_person = False
        #             self.audio_window.set_volume("/home/me/Documents/3d-graphics-project/tests/heli/data/heli.wav", 0.02)

        #         else:
        #             self.third_person = True
        #             self.audio_window.set_volume("/home/me/Documents/3d-graphics-project/tests/heli/data/heli.wav", 0.05)
        if (key == KEY_V and action == 1):
            self.debug = True
        elif (key == KEY_V and action == 0):
            self.debug = False

        if (key == KEY_LEFT_SHIFT and action == 1):
            self.shift_pressed = True
        elif (key == KEY_LEFT_SHIFT and action == 0):
            self.shift_pressed = False
            self.camera_offset = 0

if __name__ == "__main__":
    app = HeliDemo("HeliDemo", WIDTH, HEIGHT, False)
    run(app)

