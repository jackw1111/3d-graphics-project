import sys
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from engine.graphics import *
from engine.physics import *
from keys import *
import math
import random
from axis_3d import *
from third_person_camera import *

WIDTH, HEIGHT = 800, 600
Epsilon = 0.00001
depth_epsilon = 0.001

class World(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        make_context_current(self.window)
        set_cursor_visible(self.window, False)
        self.active_camera.set_far_plane(100.0)
        self.sky_box.load_skybox = False
        self.frustum_culling = False
        self.set_background_color(vec3(0,0,0))
        self.light = Light(vec3(0, 3, 2), vec3(1,1,1))
        self.active_camera.position = vec3(0,4,-20)
        self.active_camera.yaw = 90.0
        self.gravity = vec3(0,-10,0)
        self.dt = 0.02

        self.CurrentTime = 0.0
        self.TargetTime = 0.0

        self.Penetrating = 0
        self.Colliding = 1
        self.Clear = 2
        self.contacts = ContactVector()
        self.joints = SphericalJointVector()
        self.aBodies = RigidBodyVector()
        self.object_models = []

        self.create_box(vec3(-10,15,0), vec3(0,0,0))
        self.create_cylinder(vec3(-10,12,0), vec3(0,0,0))
        self.create_cylinder(vec3(-10,9,0), vec3(0,0,0))
        self.create_cylinder(vec3(-10,6,0), vec3(0,0,0))
        self.create_cylinder(vec3(-10,3,0), vec3(0,0,0))
        self.create_cylinder(vec3(-10,6,0), vec3(0,0,0))
        self.create_cylinder(vec3(-10,3,0), vec3(0,0,0))
        self.create_box(vec3(0,10,0), vec3(0,0,0))

        self.c1 = SphericalJoint(0, vec3(0,1,0), 1, vec3(0,1,0))
        self.joints.append(self.c1)
        self.c2 = SphericalJoint(0, vec3(0,-1,0), 2, vec3(0,1,0))
        self.joints.append(self.c2)
        self.c3 = SphericalJoint(0, vec3(-1,0,0), 3, vec3(0,1,0))
        self.joints.append(self.c3)
        self.c4 = SphericalJoint(0, vec3(1,0,0), 4, vec3(0,1,0))
        self.joints.append(self.c4)
        self.c5 = SphericalJoint(0, vec3(0,0,1), 5, vec3(0,1,0))
        self.joints.append(self.c5)
        self.c6 = SphericalJoint(0, vec3(0,0,-1), 6, vec3(0,1,0))
        self.joints.append(self.c6)

        self.triangle_mesh = StaticObject("./data/ramp.obj")
        self.triangle_mesh.set_to_draw = True
        raw_vertices = []
        for m in self.triangle_mesh.model.meshes:
            for v in m.vertices:
                raw_vertices.append(v.Position)

        tris = init_triangle_mesh(raw_vertices)
        for tri in tris:
            self.aBodies.append(tri)
            self.object_models.append(None)

        self.paused = False
        self.reset()
        self.first_frame = True
        self.balls = []
        for i in range(20):
            self.ball = StaticObject("./data/orange_ball.dae")
            self.ball.model_matrix = scale(mat4(1.0), vec3(0,0,0))
            self.balls.append(self.ball)

        self.axis_3d = Axis3D()
        self.third_person_camera = ThirdPersonCamera(vec3(0,5,0), vec3(0,0,-1), vec3(0,1,0), math.radians(0.0), math.radians(90.0))
        self.third_person_camera.distance = 8.0
        self.lines = []

    def reset(self):
        for i in range(0, len(self.aBodies)):
            self.aBodies[i].cm_position = vec3(-6,(i+1)*10,-1)

            self.aBodies[i].cm_velocity = vec3(0.0,0,0)
            self.aBodies[i].angular_momentum = vec3(0,0,0)
            self.aBodies[i].angular_velocity = vec3(0,0,0)
            self.aBodies[i].angular_velocity = vec3(0,0,0)
            self.aBodies[i].orientation = mat3(vec3(1,0,0), vec3(0,1,0), vec3(0,0,1))
            #self.aBodies[i].orientation = mat3_cast(rotate(mat4(1.0), math.radians(90.0), vec3(1,0,0)))
            self.aBodies[i].coefficient_of_restitution = 0.5
            r1 = random.randrange(-180, 180, 1)
            r2 = random.randrange(-10,10,1) / 10.0
            r3 = random.randrange(-10,10,1) / 10.0
            r4 = random.randrange(-10,10,1) / 10.0
            self.aBodies[i].friction = 0.5
            
    def render(self):

        if self.debug:
            for i,ball in enumerate(self.balls):
                ball.model_matrix = scale(translate(mat4(1.0), vec3(0,0,0)), vec3(0.0, 0.0, 0.0))

            for i,c in enumerate(self.contacts):
                p = c.collision_point
                if i < len(self.balls):
                    self.balls[i].model_matrix = scale(translate(mat4(1.0), p), vec3(0.1, 0.1, 0.1))
                    c.l.set_endpoints(p, p + c.normal)
                    #print (p, c.normal)
            for body in self.aBodies:
                for f in body.faces:
                    s, e = f.position_ws, vec3(f.position_ws.x, f.position_ws.y, f.position_ws.z) + f.normal_ws * 0.3
                    f.line.set_endpoints(vec3(s.x, s.y, s.z), vec3(e.x, e.y, e.z))
                for e in body.edges:
                    start, end = e.v1_ws, e.v2_ws
                    e.line.set_endpoints(vec3(start.x, start.y, start.z), vec3(end.x, end.y, end.z))

    def update(self):
        self.process_input(self.window)
        for body in self.aBodies:
            body.cm_force = self.gravity

        Kws = 0.9      # Hooke's spring constant
        Kwd = 0.1       # damping constant
        body = self.aBodies[7]
        position = body.orientation * body.a_body_bounding_vertices[0] + body.cm_position
        anchor = vec3(0,10,0)
        u = position - body.cm_position
        vu = body.cm_velocity + cross(body.angular_velocity, u)
        spring = (position - anchor) * -Kws
        # project velocity onto spring to get damping vector
        # this is basically a Gram-Schmidt projection
        damping_force =  spring * -Kwd * ( dot(vu, spring) / dot(spring, spring) )
        spring += damping_force

        body.cm_force += spring
        body.torque += cross(u, spring)
        #print (spring)
        self.l6 = Line3D(position, anchor)
        self.l6.color = vec3(1,1,0)

        if not self.paused:
            simulate(self.aBodies, self.contacts, self.joints, self.dt)

        self.render()
        for i in range(len(self.aBodies)):
            if self.object_models[i] != None:
                self.object_models[i].model_matrix = self.aBodies[i].get_model_matrix()


    def process_input(self, window):
        #self.active_camera.position = self.third_person_camera.get_position(self.active_camera, vec3(0,5,0))

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
        if (get_key(window, KEY_LEFT_SHIFT) == PRESS):
            self.active_camera.MovementSpeed = 20.0
        if (get_key(window, KEY_LEFT_SHIFT) == RELEASE):
            self.active_camera.MovementSpeed = 4.0


    def on_key_pressed(self, key, scancode, action, mods):
        if key == KEY_R and action == 1:
            self.reset()
        if key == KEY_P and action == 1:
            if self.paused:
                self.paused = False
            else:
                self.paused = True

        if key == KEY_Q and action == 1:
            if self.debug:
                self.debug = False
            else:
                self.debug = True

        if (key == KEY_5 and action == 1):
            world_pos = ray_cast(WIDTH/2, HEIGHT/2, self.active_camera.view_matrix, self.active_camera.projection_matrix, WIDTH, HEIGHT)
            self.aBodies[0].cm_position = self.active_camera.position + world_pos * 5.0
            self.aBodies[0].cm_velocity = world_pos * 20.0

        if (key == KEY_1 and action == 1):
            world_pos = ray_cast(WIDTH/2, HEIGHT/2, self.active_camera.view_matrix, self.active_camera.projection_matrix, WIDTH, HEIGHT)
            self.create_cylinder(self.active_camera.position + world_pos * 5.0, world_pos * 20.0)


        if (key == KEY_2 and action == 1):
            world_pos = ray_cast(WIDTH/2, HEIGHT/2, self.active_camera.view_matrix, self.active_camera.projection_matrix, WIDTH, HEIGHT)
            self.create_box(self.active_camera.position + world_pos * 5.0, world_pos * 20.0)


        if (key == KEY_3 and action == 1):
            world_pos = ray_cast(WIDTH/2, HEIGHT/2, self.active_camera.view_matrix, self.active_camera.projection_matrix, WIDTH, HEIGHT)
            self.create_sphere(self.active_camera.position + world_pos * 5.0, world_pos * 20.0)


        if (key == KEY_4 and action == 1):
            world_pos = ray_cast(WIDTH/2, HEIGHT/2, self.active_camera.view_matrix, self.active_camera.projection_matrix, WIDTH, HEIGHT)
            self.create_capsule(self.active_camera.position + world_pos * 5.0, world_pos * 20.0)

    def create_capsule(self, pos, vel):
        body = RigidBody2()
        init_capsule(body)
        body.cm_position = pos
        body.cm_velocity = vel
        body.coefficient_of_restitution = 0.3
        self.obj = StaticObject("./data/capsule.obj")
        self.aBodies.append(body)
        self.object_models.append(self.obj)


    def create_cylinder(self, pos, vel):
        body = RigidBody2()
        init_cylinder(body)
        body.cm_position = pos
        body.cm_velocity = vel
        body.coefficient_of_restitution = 0.3

        self.obj = StaticObject("./data/cylinder.obj")
        self.aBodies.append(body)
        self.object_models.append(self.obj)

    def create_box(self, pos, vel):
        body = RigidBody2()
        init_box(body, 1,1,1)
        body.cm_position = pos
        body.cm_velocity = vel
        body.coefficient_of_restitution = 0.5

        self.obj = StaticObject("./data/cube.obj")
        self.aBodies.append(body)
        self.object_models.append(self.obj)

    def create_sphere(self, pos, vel):
        # body = RigidBody2("./data/basketball.dae")
        body = RigidBody2()
        init_sphere(body)
        body.cm_position = pos
        body.cm_velocity = vel
        body.coefficient_of_restitution = 0.3
        self.obj = StaticObject("./data/red_ball.dae")
        self.aBodies.append(body)
        self.object_models.append(self.obj)

    def on_mouse_moved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top
        self.lastX = xpos
        self.lastY = ypos
        self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)

    def on_mouse_clicked(self, button, action, mods):
        pass

if __name__ == "__main__":
    world = World("3D Physics", WIDTH, HEIGHT, False, False)
    run(world)
