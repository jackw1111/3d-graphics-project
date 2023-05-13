import sys
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from console import *
from engine.graphics import *
from keys import *
import random
import time
import math

WIDTH, HEIGHT = 800, 600
gravity = vec2(0,-10)
Penetrating = 0
Colliding = 1
Clear = 2
Epsilon = 0.00001

# TO DO replace mat2
def rotate_point(pt, angle):
    x,y = pt.x, pt.y
    _x = x * math.cos(math.radians(angle)) - y * math.sin(math.radians(angle))
    _y = x * math.sin(math.radians(angle)) + y * math.cos(math.radians(angle))
    return vec2(_x, _y)

def perp(v):
    return vec2(-v.y, v.x)

class Wall:
    def __init__(self, start, end):
        # define wall by plane equation
        self.normal = vec2(0,0)  # inward pointing
        self.c = 0 # ax + by + c = 0
        self.start_point = start
        self.end_point = end

        self.l1 = Line2D(start,end, WIDTH, HEIGHT)

class RigidBody:
    aBodies = []

    def __init__(self, pos, sz, vel, orient, ang_vel, restitution, density, m):
        self.icon_rect = Rect2D(pos, sz, "./data/red_circle.png", 1, 1)
        self.icon_rect.frame = 0
        self.icon_rect.ordering = 1

        self.cm_position = pos
        self.orientation = orient
        self.cm_velocity = vel
        self.angular_velocity = ang_vel
        self.cm_force = vec2(0,0)
        self.torque = 0
        self.a_bounding_vertices = [vec2(0,0), vec2(0,0), vec2(0,0), vec2(0,0)]
        self.radius = 20.0
        self.sz = sz

        self.mass = density * self.sz.x * self.sz.y
        self.coefficient_of_restitution = 0.5
        self.friction = 0.8
        self.one_over_mass = 1.0 / self.mass

        # integrate over the body to find the moment of inertia
        self.one_over_cm_moment_of_inertia = 1.0 / (math.pi / 4.0 * math.pow(self.radius, 4))

        RigidBody.aBodies.append(self)

        self.prev_cm_position = pos
        self.prev_orientation = orient
        self.prev_cm_velocity  = vel
        self.prev_angular_velocity = ang_vel
        self.prev_cm_force = vec2(0,0)
        self.prev_torque = 0.0

    def swap_values(self):
        self.prev_cm_position = self.cm_position
        self.prev_orientation =  self.orientation
        self.prev_cm_velocity = self.cm_velocity
        self.prev_angular_velocity = self.angular_velocity
        self.prev_cm_force = self.cm_force
        self.prev_torque = self.torque

class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        make_context_current(self.window)
        self.sky_box.load_skybox = False
        self.light = Light(vec3(1,1,1), vec3(1,1,1))

        self.collision_state = Clear
        self.collision_normal = vec2(0,0)
        self.colliding_body_index = 0
        self.colliding_corner_index = 0

        self.aWalls = []

        w2 = Wall(vec2(0,0), vec2(WIDTH, 0))

        w2.normal = vec2(0.0,1.0)
        w2.c = HEIGHT/2.0

        self.aWalls.append(w2)


        self.start_time = 0.0
        self.r1 = RigidBody(vec2(100,100), vec2(40,40), vec2(5,15), math.radians(0.0), 2.0, 1.0, 0.1, 1.0)
        self.r2 = RigidBody(vec2(100,200), vec2(40,40), vec2(5,0), math.radians(0.0), 2.0, 1.0, 0.1, 1.0)

    def get_time(self):
        self.start_time += 0.005
        return self.start_time

    def calculate_vertices(self):
        for counter in range(len(RigidBody.aBodies)):
            body = RigidBody.aBodies[counter]
            o = body.orientation
            position = body.cm_position
            width = body.sz.x / 2.0
            height = body.sz.y / 2.0

            v1 = position + mat2(math.cos(o), math.sin(o), -math.sin(o), math.cos(o)) * vec2( width, height)
            v2 = position + mat2(math.cos(o), math.sin(o), -math.sin(o), math.cos(o)) * vec2( width, -height)
            v3 = position + mat2(math.cos(o), math.sin(o), -math.sin(o), math.cos(o)) * vec2( -width, -height)
            v4 = position + mat2(math.cos(o), math.sin(o), -math.sin(o), math.cos(o)) * vec2( -width, height)

            body.a_bounding_vertices[0] = v1
            body.a_bounding_vertices[1] = v2
            body.a_bounding_vertices[2] = v3
            body.a_bounding_vertices[3] = v4

    def reset_bodies_data(self):
        # we made a successful step, so swap configurations
        # to "save" the data for the next step
        for body in RigidBody.aBodies:
            body.swap_values()


    def check_for_collisions(self):
        # collision with walls
        # self.collision_state = Clear
        # depth_epsilon = 1.0
        # half_width = WIDTH / 2.0
        # half_height = HEIGHT / 2.0
        # for i in range(len(RigidBody.aBodies)):
        #     body = RigidBody.aBodies[i]
        #     if self.collision_state is not Penetrating:

        #         for wall_index,wall in enumerate(self.aWalls):
        #             if self.collision_state is not Penetrating:
        #                 position = body.cm_position - wall.normal * body.radius
        #                 CMToCornerPerp = perp(position - body.cm_position)
        #                 Velocity = body.cm_velocity + CMToCornerPerp * body.angular_velocity
        #                 if self.collision_state is not Penetrating:
        #                     axbyc = dot(position, wall.normal) + wall.c
        #                     if axbyc < -depth_epsilon:
        #                         self.collision_state = Penetrating
        #                     elif axbyc < depth_epsilon:
        #                         relative_velocity = dot(wall.normal, Velocity)
        #                         if relative_velocity < 0.0:
        #                             self.collision_state = Colliding
        #                             self.collision_normal = wall.normal
        #                             self.colliding_corner_index = 0
        #                             body.a_bounding_vertices[0] = position
        #                             self.colliding_body_index = i

        # return self.collision_state

        self.collision_state = Clear
        depth_epsilon = 0.001
        half_width = WIDTH / 2.0
        half_height = HEIGHT / 2.0
        for i in range(len(RigidBody.aBodies)):
            body1 = RigidBody.aBodies[i]
            if self.collision_state is not Penetrating:
                for j in range(len(RigidBody.aBodies)):
                    if i == j:
                        continue
                    body2 = RigidBody.aBodies[j]

                    if self.collision_state is not Penetrating:
                        normal = normalize(body2.cm_position - body1.cm_position)
                        if distance(body1.cm_position, body2.cm_position) < (body1.radius + body2.radius):
                            normal = normalize(body2.cm_position - body1.cm_position)
                            p_a = body1.cm_position + normal * body1.radius
                            p_b = body2.cm_position - normal * body2.radius

                            depth = distance(body1.cm_position, body2.cm_position) - (body1.radius + body2.radius)

                            CMToCornerPerpA = perp(p_a - body1.cm_position)
                            CMToCornerPerpB = perp(p_b - body2.cm_position)

                            va = body1.cm_velocity + CMToCornerPerpA * body1.angular_velocity
                            vb = body2.cm_velocity + CMToCornerPerpB * body2.angular_velocity
                            Velocity = vb - va

                            axbyc = dot(p_a, normal)
                            if depth < -depth_epsilon:
                                self.collision_state = Penetrating
                            elif depth < depth_epsilon:
                                relative_velocity = dot(normal, Velocity)
                                if relative_velocity < 0.0:
                                    self.collision_state = Colliding
                                    self.collision_normal = normal
                                    self.colliding_corner_index = 0
                                    body1.a_bounding_vertices[0] = p_a
                                    self.colliding_body_index = i

        return self.collision_state
    
    def integrate(self, deltaTime):
        for counter in range(len(RigidBody.aBodies)):
            body = RigidBody.aBodies[counter]
            body.cm_position = body.prev_cm_position + body.prev_cm_velocity * deltaTime 
            body.orientation = body.prev_orientation + body.prev_angular_velocity * deltaTime
            body.cm_velocity = body.prev_cm_velocity +  body.prev_cm_force * (RigidBody.aBodies[counter].one_over_mass * deltaTime)
            body.angular_velocity = body.prev_angular_velocity + (RigidBody.aBodies[counter].one_over_cm_moment_of_inertia * deltaTime) * body.prev_torque
    
    def compute_forces(self):

        for counter in range(len(RigidBody.aBodies)):
            # clear forces
            body = RigidBody.aBodies[counter]
            body.prev_torque = 0.0
            body.prev_cm_force = vec2(0,0);
            v = 1.0 / body.one_over_mass
            body.prev_cm_force += gravity * v

    def resolve_collisions(self):
        # collision response (normal) impulse
        body = RigidBody.aBodies[self.colliding_body_index]
        position = body.a_bounding_vertices[self.colliding_corner_index]
        impulse_direction = self.collision_normal
        CMToCornerPerp = perp(position - body.cm_position)
        Velocity = body.cm_velocity + CMToCornerPerp * body.angular_velocity
        impulse_numerator = -(1.0 + body.coefficient_of_restitution) * dot(Velocity,impulse_direction)
        perp_dot = dot(CMToCornerPerp, impulse_direction)
        impulse_denominator = body.one_over_mass + body.one_over_cm_moment_of_inertia * perp_dot * perp_dot
        normal_impulse = impulse_numerator / impulse_denominator
        #normal_impulse = max(0, normal_impulse)
        print (normal_impulse, body, self.r2)
        body.cm_velocity += impulse_direction * normal_impulse * body.one_over_mass
        body.angular_velocity += normal_impulse * body.one_over_cm_moment_of_inertia * perp_dot

        # friction (tangent) impulse
        impulse_direction = perp(self.collision_normal)
        CMToCornerPerp = perp(position - body.cm_position)
        Velocity = body.cm_velocity + CMToCornerPerp * body.angular_velocity
        impulse_numerator = - dot(Velocity,impulse_direction)
        perp_dot = dot(CMToCornerPerp, impulse_direction)
        impulse_denominator = body.one_over_mass + body.one_over_cm_moment_of_inertia * perp_dot * perp_dot
        tangent_impulse = impulse_numerator / impulse_denominator
        tangent_impulse = max(tangent_impulse, -1.0 * math.sqrt(2) * body.friction * normal_impulse)
        tangent_impulse = min(tangent_impulse,        math.sqrt(2) * body.friction * normal_impulse)
        body.cm_velocity += impulse_direction * tangent_impulse * body.one_over_mass
        body.angular_velocity += tangent_impulse * body.one_over_cm_moment_of_inertia * perp_dot

    def simulate(self, deltaTime):
        CurrentTime = 0.0
        TargetTime = deltaTime
        while CurrentTime < deltaTime:
            self.compute_forces()
            self.integrate(TargetTime-CurrentTime)
            self.calculate_vertices()
            self.check_for_collisions()
            if self.collision_state == Penetrating:
                TargetTime = (CurrentTime + TargetTime) / 2.0
                assert(math.fabs(TargetTime - CurrentTime) > Epsilon)
            else:
                if self.collision_state == Colliding:
                    Counter = 0
                    while True:
                        self.resolve_collisions();
                        Counter += 1

                        if (not (self.check_for_collisions() == Colliding) and (Counter < 100)):
                            break;
                        
                    assert(Counter < 100)


                CurrentTime = TargetTime
                TargetTime = deltaTime

                self.reset_bodies_data()

    def update(self):

        self.last_time = self.get_time()
        # use a fixed timestep until we implement a better integrator

        Time = self.last_time + 0.02

        self.simulate(Time - self.last_time)

        self.last_time = Time

        for r in RigidBody.aBodies:
            r.icon_rect.position =  r.cm_position + vec2(WIDTH/2, HEIGHT/2)
            r.icon_rect.orientation = math.degrees(r.orientation)

    def on_key_pressed(self, key, scancode, action, mods):
        if key == KEY_ESCAPE:
            set_window_should_close(self.window, True)

        if key == KEY_R:
            self.r1.prev_cm_position = vec2(100,100)
            self.r2.prev_cm_position = vec2(300,200)

    def process_input(self, window):
        pass
    def on_mouse_clicked(self, button, action, mods):
        pass
    def on_window_resized(self, width, height):
        pass
    def on_mouse_moved(self, xpos, ypos):
        pass
if __name__ == "__main__":
    app = App("2d-physics", WIDTH, HEIGHT, False, False)
    run(app)
