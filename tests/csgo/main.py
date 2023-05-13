import sys
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from console import *
from axis_3d import *
from third_person_camera import *
from engine.graphics import *
from engine.physics import *
from keys import *
from player import *
import random
import time
import math
from OpenGL.GL import *
from multiprocessing import Process, Manager


WIDTH = 640
HEIGHT = 480

# TO DO fix lighting, fix gravity, fix UI

class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        make_context_current(self.window)
        set_cursor_visible(self.window, False)
        self.set_background_color(vec3(1,0,0))
        self.shadow_resolution = 2048
        self.active_camera.set_far_plane(1000.0)
        self.gravity = vec3(0,-15,0)
        self.contacts = ContactVector()
        self.joints = SphericalJointVector()
        self.aBodies = RigidBodyVector()
        self.aBodies2 = RigidBodyVector()

        self.object_models = []
        self.object_models2 = []

        self.dt = 0.02

        body = RigidBody2()
        body.friction = 0.0
        body.coefficient_of_restitution = 0.0
        body.dX2 = 7.5
        body.dY2 = 7.5
        body.dZ2 = 7.5
        init_sphere(body)
        self.sphere = StaticObject("../dice/data/red_ball.dae")
        self.sphere.set_to_draw = False
        body.inverse_body_inertia_tensor = mat3(vec3(0,0,0),
                                               vec3(0,0,0),
                                               vec3(0,0,0))
        body.cm_position = vec3(0,40,0)
        #body.object.set_to_draw = False
        body.mass = 1.0
        body.one_over_mass = 1.0 / body.mass

        self.aBodies.append(body)
        self.object_models.append(StaticObject("../dice/data/capsule.obj"))
        self.object_models[0].set_to_draw = False
        # simple UI
        self.crosshair = Rect2D(vec2(WIDTH/2, HEIGHT/2), vec2(25, 25), "./data/crosshair.png",1,1)
        self.crosshair.ordering = 1
        self.crosshair.set_to_draw = False
        self.scope = Rect2D(vec2(WIDTH/2, HEIGHT/2), vec2(WIDTH, HEIGHT), "./data/scope.png",1,1)
        self.scope.to_draw = False
        self.scope.ordering = 1

        self.loading_indicator = Rect2D(vec2(50,HEIGHT-50), vec2(100,100), "../particles/data/loading_spritesheet.png",12,4)
        self.loading_indicator.frame = 1
        #self.loading_indicator.ordering = 1
        self.blankscreen = Rect2D(vec2(WIDTH/2,HEIGHT/2), vec2(WIDTH,HEIGHT) , "./data/grey.png",1,1)
        #self.blankscreen.ordering = 1
        #self.bloodscreen = Rect2D(vec2(WIDTH/2,HEIGHT/2), vec2(WIDTH,HEIGHT) , "./data/bloodsmear.png",1,1)
        #self.bloodscreen.ordering = 1

        self.triangle_mesh = StaticObject("/home/me/Documents/3d-graphics-project2/tests/heli/data/sauerbraten/kmap5.obj")
        #self.triangle_mesh = StaticObject("./data/systemshock2.obj")        
        #self.triangle_mesh.set_to_draw = True
        self.light = Light(vec3(-4,8,-2), vec3(1,1,1))
        self.audio_window = AudioWindow()
        self.player = AnimatedObject("../angry-bots/data/player.fbx")
        self.player.color = vec4(0,0,0,1)
        self.player.model_matrix = translate(scale(mat4(1.0), vec3(0.1, 0.1, 0.1)), vec3(0,8,0))

        # BUG: have to set color otherwise AnimatedObject doesnt draw!
        self.gun1 = AnimatedObject("./data/fps_hands_aks74u.fbx")
        self.gun1.color = vec4(0,0,0,1)
        #self.gun1.set_frames(0.1, 0.101, 0.0)
        self.gun2 = AnimatedObject("./data/AWP/fps_hands_awp.dae")
        self.gun2.color = vec4(0,0,0,1)
        self.gun1.set_to_draw_shadow = False
        self.gun2.set_to_draw_shadow = False
        self.gun1.set_to_draw = False
        self.gun2.set_to_draw = False

        self.gun = self.gun1

        self.ak47_icon = Rect2D(vec2(100 ,HEIGHT - 100), vec2(100,100), "./data/ak47.png",1,1)
        self.awp_icon = Rect2D(vec2(100 , HEIGHT - 100), vec2(100,100), "./data/awp.png",1,1)
        self.ak47_icon.ordering = 1
        self.awp_icon.ordering = 1

        self.active_camera.MovementSpeed = 70.0

        self.set_background_color(vec3(0.8, 0.9, 1.0))

        self.jump = vec3(0,0,0)
        self.t  = 0
        glLineWidth(5)
        self.i = 0
        self.gravity = vec3(0,-50,0)

        self.last_jump_value = None
        self.current_jump_value = None

        self.third_person_camera = ThirdPersonCamera(body.cm_position, vec3(0,0,-1), vec3(0,1,0), math.radians(0.0), math.radians(90.0))
        self.third_person_camera.distance = 30.0
        self.is_third_person = False
        self.kinematic_force = vec3(0,0,0)
        self.console = Console(WIDTH, HEIGHT)

        raw_vertices = []
        for m in self.triangle_mesh.model.meshes:
            for v in m.vertices:
                raw_vertices.append(v.Position)
        tris = init_triangle_mesh(raw_vertices)
        for tri in tris:
            self.aBodies.append(tri)
            self.object_models.append(None)
            

        self.third_person_camera = ThirdPersonCamera(vec3(0,0,0), vec3(0,0,-1), vec3(0,1,0), 0.0, -90.0)
        self.third_person_camera.distance = 30.0

        # self.finishedloading = False
        # self.manager = Manager()
        # return_dict = self.manager.dict()
        # p = Process(target=self.loadthread, args=(return_dict, ))
        # p.start()
        #t1.join()

        self.depth = -7.4

    # def loadthread(self, return_dict):

    #     raw_vertices = []
    #     for m in self.triangle_mesh.model.meshes:
    #         for v in m.vertices:
    #             raw_vertices.append(v.Position)
    #     tris = init_triangle_mesh(raw_vertices)
    #     for tri in tris:
    #         self.aBodies.append(tri)
    #         self.object_models.append(None)

    #     # self.crosshair.set_to_draw = True
    #     # self.blankscreen.set_to_draw = False
    #     # self.loading_indicator.set_to_draw = False
    #     # self.loading_indicator.ordering = 0
    #     # self.blankscreen.ordering = 0

    #     self.finishedloading = True

    def wireframe_box(self, hw, pos):
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


    def update(self):
        #print (self.t)
        self.loading_indicator.frame += 1


        self.t += 0.01
        self.process_input(self.window)
        self.console.update(self.currentFrame, self.deltaTime)

        #if self.finishedloading:
        body = self.aBodies[0]
        body.cm_force += self.gravity
        body.cm_force += self.jump_force
        self.kinematic_force -= body.cm_velocity * 2.0
        body.cm_force += self.kinematic_force
        simulate(self.aBodies, self.contacts, self.joints, self.dt)

        for i in range(1,len(self.aBodies2)):
            body = self.aBodies2[i]
            body.cm_force += self.gravity
            print (len(self.aBodies2))
            joints = SphericalJointVector()
            contacts = ContactVector()
            simulate(self.aBodies2, contacts, joints, self.dt)
            # self.object_models2[i].model_matrix = self.aBodies2[i].get_model_matrix()
        #print (self.get_fps())
        self.object_models[0].model_matrix = self.aBodies[0].get_model_matrix()
        self.sphere.model_matrix =  self.aBodies[0].get_model_matrix()
        ids = self.aBodies[0].tri_ids
        self.lines = []
        if self.debug:
            for _id in ids:
                verts = self.aBodies[_id].faces[0].verts_ws
                self.l1 = Line3D(verts[0], verts[1])
                self.l2 = Line3D(verts[1], verts[2])
                self.l3 = Line3D(verts[2], verts[0])
                self.l1.color = vec3(0,1,0)
                self.l2.color = vec3(0,1,0)
                self.l3.color = vec3(0,1,0)
                self.lines.append(self.l1)
                self.lines.append(self.l2)
                self.lines.append(self.l3)
            boxes = get_all_nodes()
            for box in boxes:
                pos = vec3(box.x, box.y, box.z)
                hw = box.w
                self.wireframe_box(hw, pos)

        if (is_joystick_present()):
            #print ('joysticks')
            speed = 1.0#self.active_camera.MovementSpeed
            axes = FloatVector()
            axes = get_joystick_axes()
            xpos = (axes[3] + 1)*WIDTH/2 
            ypos = (axes[4] + 1)*HEIGHT/2
            xoffset = xpos - self.lastX
            yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

            self.lastX = xpos
            self.lastY = ypos
            print (axes[0], axes[1], axes[2], axes[3], axes[4])
            #self.active_camera.ProcessMouseMovement(axes[3]*speed, axes[4]*speed, True)

            # rxpos = (axes[1] + 1)*WIDTH/2 
            # rypos = (axes[0] + 1)*HEIGHT/2

            # rxoffset = rxpos - self.rlastX
            # ryoffset = rypos - self.rlastY #reversed since y-coordinates go from bottom to top


            # self.rlastX = rxpos
            # self.rlastY = rypos
            # tolerance = 25.0

            # if (math.fabs(WIDTH/2 - rxpos) > tolerance):
            #     self.active_camera.ProcessKeyboard(0, self.deltaTime*(WIDTH/2 - rxpos)*0.003)
            # if (math.fabs(HEIGHT/2 - rypos) > tolerance):
            #     self.active_camera.ProcessKeyboard(2, self.deltaTime*(HEIGHT/2 - rypos)*-0.003)


        if (self.gun == self.gun1):
            self.gun.model_matrix = translate(mat4(1.0), self.active_camera.position)
            self.gun.model_matrix = rotate(self.gun.model_matrix, -math.radians(self.active_camera.yaw + 90.0), vec3(0,1,0))
            self.gun.model_matrix = rotate(self.gun.model_matrix, math.radians(self.active_camera.pitch), vec3(1,0,0))
            self.gun.model_matrix = translate(self.gun.model_matrix, vec3(0.2,-0.5, -0.1))
            #self.gun2.set_to_draw = False
            #self.gun1.set_to_draw = True
            #self.gun1.render_to_ui = 1
            #self.ak47_icon.to_draw = True
            #self.awp_icon.to_draw = False

        elif (self.gun == self.gun2):
            self.gun.model_matrix = translate(mat4(1.0), self.active_camera.position)
            self.gun.model_matrix = scale(self.gun.model_matrix, vec3(3,3,3))
            self.gun.model_matrix = rotate(self.gun.model_matrix, -math.radians(self.active_camera.yaw + 90.0), vec3(0,1,0))
            self.gun.model_matrix = rotate(self.gun.model_matrix, math.radians(self.active_camera.pitch), vec3(1,0,0))
            self.gun.model_matrix = translate(self.gun.model_matrix, vec3(0.1,-0.0, 0))
            #self.gun2.set_to_draw =  True
            #self.gun1.set_to_draw =  False
            #self.ak47_icon.to_draw = False
            #self.awp_icon.to_draw = True

        #self.other_player.set_frames(0, 5.0, self.t)
        pos = get_position(self.aBodies[0].get_model_matrix())
        self.player.model_matrix = scale(mat4(1.0), vec3(0.01,0.01,0.01))
        self.player.model_matrix = translate(mat4(1.0), vec3(0,self.depth,0)) * self.aBodies[0].get_model_matrix() * rotate(self.player.model_matrix, math.radians(90.0-self.active_camera.yaw), vec3(0,1,0)) 
        self.active_camera.position = self.third_person_camera.get_position(self.active_camera, pos)

    def process_input(self, window):
        if (get_key(window, KEY_ESCAPE) == PRESS):
            set_window_should_close(self.window, True);

        t = self.currentFrame - int(self.currentFrame)
        any_key_pressed = False

        self.active_camera.MovementSpeed = 20.0
        keypress = 0
        self.kinematic_force_forward = vec3(0,0,0)
        self.kinematic_force_side = vec3(0,0,0)

        if (get_key(window, KEY_W) == PRESS):
            self.kinematic_force_forward += self.active_camera.front
            keypress = 1
            self.player.set_frames(2.85, 3.5, t)      
            any_key_pressed = True

        elif (get_key(window, KEY_S) == PRESS):
            self.kinematic_force_forward  -= self.active_camera.front
            keypress = 1
            self.player.set_frames(3.65, 4.3, t)            
            any_key_pressed = True

        if (get_key(window, KEY_A) == PRESS):
            self.kinematic_force_side  -= self.active_camera.right
            keypress = 1
            self.player.set_frames(5.32, 6, t)      
            any_key_pressed = True

        elif (get_key(window, KEY_D) == PRESS):
            self.kinematic_force_side  += self.active_camera.right
            keypress = 1
            self.player.set_frames(4.5, 5.15, t)      
            any_key_pressed = True

        if (not any_key_pressed):
            self.player.set_frames(0.2, 2.7, self.currentFrame)  

        self.kinematic_force_forward = normalize(self.kinematic_force_forward) * 100.0
        self.kinematic_force_side = normalize(self.kinematic_force_side) * 100.0
        self.kinematic_force = self.kinematic_force_forward + self.kinematic_force_side

        self.current_jump_value = get_key(window, KEY_SPACE)
        if self.last_jump_value == RELEASE and self.current_jump_value == PRESS:
            self.jump_force = vec3(0,3000,0)
        else:
            self.jump_force = vec3(0,0,0)
        self.last_jump_value = self.current_jump_value

        #if (self.is_third_person):
        #    self.active_camera.position = self.third_person_camera.get_position(self.active_camera, self.aBodies[0].cm_position)
        #else:
        #    self.active_camera.position = self.aBodies[0].cm_position + vec3(0,1,0)

    def on_mouse_moved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos
        self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)

    def on_mouse_clicked(self, button, action, mods):
        print (MOUSE_BUTTON_2, button, action)
        if (button == MOUSE_BUTTON_1 and action == 0):
            #self.audio_window.play("./data/awp.wav")
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


        if (button == MOUSE_BUTTON_2 and action == 0):
            # toggle scope on rmb click
            if not self.scope.to_draw:
                self.active_camera.projection_matrix = perspective(math.radians(15.0), float(WIDTH)/float(HEIGHT), 0.1, 1000.0)
                self.scope.to_draw = True
                self.gun1.set_to_draw =  False
                self.gun2.set_to_draw =  False
                self.crosshair.to_draw = False
            elif self.scope.to_draw:
                self.scope.to_draw = False
                self.gun1.set_to_draw =  True
                self.gun2.set_to_draw =  True
                self.crosshair.to_draw = True
                self.active_camera.projection_matrix = perspective(math.radians(45.0), float(WIDTH)/float(HEIGHT), 0.1, 1000.0)

    def on_window_resized(self, width, height):
        pass

    def on_key_pressed(self, key, scancode, action, mods):
        self.console.on_key_pressed(key, scancode, action, mods)
        if (key == KEY_1 and action == 1):
            self.gun = self.gun1
            self.gun1.set_to_draw =  True
            self.gun2.set_to_draw =  False

        if (key == KEY_2 and action == 1):
            self.gun = self.gun2
            self.gun1.set_to_draw =  False
            self.gun2.set_to_draw =  True

        if (key == KEY_3 and action == 1):
            self.depth -= 0.2

        if (key == KEY_SPACE and action == 1):
            if (self.jump == vec3(0,0,0)):
                self.jump = vec3(0,16,0)

        if (key == KEY_F and action == PRESS):
            if self.is_third_person:
                self.is_third_person = False
            else:
                self.is_third_person = True

        if (key == KEY_V and action == 1):
            if self.debug:
                self.debug = False
            else:
                self.debug = True

        if (key == KEY_4 and action == PRESS):
            body = RigidBody2()
            body.friction = 0.0
            body.coefficient_of_restitution = 0.0
            body.dX2 = 1.0
            body.dY2 = 1.0
            body.dZ2 = 1.0
            init_box(body, 1.0, 1.0, 1.0)
            self.box = StaticObject("../dice/data/cube.obj")
            self.box.set_to_draw = True
            # body.inverse_body_inertia_tensor = mat3(vec3(0,0,0),
            #                                        vec3(0,0,0),
            #                                        vec3(0,0,0))
            body.cm_position = self.aBodies[0].cm_position + vec3(0,3,0)
            #body.object.set_to_draw = False
            body.mass = 1.0
            body.one_over_mass = 1.0 / body.mass
            self.object_models2.append(self.box)
            self.aBodies2.append(body)

if __name__ == "__main__":
    app = App("csgo", WIDTH, HEIGHT, True, False)
    run(app)
