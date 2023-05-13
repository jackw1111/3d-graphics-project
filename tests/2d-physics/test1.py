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
depth_epsilon = 1.0
deltaTime = 0.02


def perp(v):
    return vec2(-v.y, v.x)

class Wall:
    def __init__(self):
        # define wall by plane equation
        self.normal = vec2(0,0)  # inward pointing
        self.c = 0 # ax + by + c = 0
        self.start_point = vec2(0,0)
        self.end_point = vec2(0,0)

class RigidBody:
    aBodies = []

    def __init__(self, pos, sz, vel, orient, ang_vel, restitution, density, m):
        self.icon_rect = Rect2D(pos, sz*2.0, "./data/red_circle.png", 1, 1)
        self.icon_rect.frame = 0
        self.icon_rect.ordering = 1

        self.cm_position = pos
        self.orientation = orient
        self.cm_velocity = vel
        self.angular_velocity = ang_vel
        self.cm_force = vec2(0,0)
        self.torque = 0

        self.sz = sz

        self.mass = density * self.sz.x * self.sz.y
        self.coefficient_of_restitution = 0.6
        self.friction = 0.3
        self.one_over_mass = 1.0 / self.mass

        # integrate over the body to find the moment of inertia
        self.one_over_cm_moment_of_inertia = 1.0 / ((self.mass / 12.0) *
            (self.sz.x * self.sz.x + self.sz.y * self.sz.y))

        RigidBody.aBodies.append(self)

class Contact():
    def __init__(self, body_a, body_b):
        self.body_a = body_a
        self.body_b = body_b
        self.point = vec2(0,0)

class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        make_context_current(self.window)
        self.sky_box.load_skybox = False
        self.light = Light(vec3(1,1,1), vec3(1,1,1))

        self.aWalls = []
        w2 = Wall()
        w2.normal = vec2(0.0,1.0)
        w2.c = - 3.0
        self.aWalls.append(w2)

        self.start_time = 0.0
        self.r = RigidBody(vec2(200,100), vec2(20,20), vec2(20,0), math.radians(0.0), 0.0, 1.0, 0.1, 1.0)
        self.contacts = []

    def check_for_collisions(self):
        self.contacts = []
        for i in range(len(RigidBody.aBodies)):
            body = RigidBody.aBodies[i]
            for wall in self.aWalls:
                position = body.cm_position - wall.normal * body.sz.x
                CM_to_corner_perp = perp(position - body.cm_position)
                Velocity = body.cm_velocity + CM_to_corner_perp * body.angular_velocity
                depth = dot(position, wall.normal) + wall.c
                if depth < depth_epsilon:
                    relative_velocity = dot(wall.normal, Velocity)
                    if relative_velocity < 0.0:
                        contact = Contact(body, wall)
                        contact.point = position
                        self.contacts.append(contact)

    
    def integrate(self, deltaTime):
        for counter in range(len(RigidBody.aBodies)):
            body = RigidBody.aBodies[counter]
            body.cm_position += body.cm_velocity * deltaTime 
            body.orientation += body.angular_velocity * deltaTime
            body.cm_velocity +=  body.cm_force * (RigidBody.aBodies[counter].one_over_mass * deltaTime)
            body.angular_velocity += (RigidBody.aBodies[counter].one_over_cm_moment_of_inertia * deltaTime) * body.torque
    
    def compute_forces(self):

        for counter in range(len(RigidBody.aBodies)):
            # clear forces
            body = RigidBody.aBodies[counter]
            body.torque = 0.0
            body.cm_force = vec2(0,0);
            v = 1.0 / body.one_over_mass
            body.cm_force += gravity * v

    def resolve_collisions(self, contact):
        # collision response (normal) impulse
        body = contact.body_a
        position = contact.point
        impulse_direction = contact.body_b.normal
        CM_to_corner_perp = perp(position - body.cm_position)
        Velocity = body.cm_velocity + CM_to_corner_perp * body.angular_velocity
        impulse_numerator = -(1.0 + body.coefficient_of_restitution) * dot(Velocity,impulse_direction)
        perp_dot = dot(CM_to_corner_perp, impulse_direction)
        impulse_denominator = body.one_over_mass + body.one_over_cm_moment_of_inertia * perp_dot * perp_dot
        normal_impulse = impulse_numerator / impulse_denominator
        normal_impulse = max(0, normal_impulse)
        body.cm_velocity += impulse_direction * normal_impulse * body.one_over_mass
        body.angular_velocity += normal_impulse * body.one_over_cm_moment_of_inertia * perp_dot

        # friction (tangent) impulse
        impulse_direction = perp(contact.body_b.normal)
        CM_to_corner_perp = perp(position - body.cm_position)
        Velocity = body.cm_velocity + CM_to_corner_perp * body.angular_velocity
        impulse_numerator = -1.0 * dot(Velocity,impulse_direction)
        perp_dot = dot(CM_to_corner_perp, impulse_direction)
        impulse_denominator = body.one_over_mass + body.one_over_cm_moment_of_inertia * perp_dot * perp_dot
        tangent_impulse = impulse_numerator / impulse_denominator
        tangent_impulse = max(-1.0*math.sqrt(2)*body.friction * normal_impulse, tangent_impulse)
        tangent_impulse = min(tangent_impulse, body.friction * normal_impulse * math.sqrt(2))
        body.cm_velocity += impulse_direction * tangent_impulse * body.one_over_mass
        body.angular_velocity += tangent_impulse * body.one_over_cm_moment_of_inertia * perp_dot

    def update(self):

        self.compute_forces()
        self.integrate(deltaTime)
        self.check_for_collisions()
        for c in self.contacts:
            self.resolve_collisions(c)

        for r in RigidBody.aBodies:
            r.icon_rect.position =  r.cm_position
            r.icon_rect.orientation = math.degrees(r.orientation)

    def on_key_pressed(self, key, scancode, action, mods):
        if key == KEY_ESCAPE:
            set_window_should_close(self.window, True)

        if key == KEY_R:
            self.r.prev_cm_position = vec2(100,100)

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
