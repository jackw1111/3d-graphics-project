# basic 2d physics of a circle and the ground, conserving rotational momentum and friction
# press R to generate a random velocity
# can be easy enough to extend to rects, (test collisions against each vertex and the ground, if colliding the normal is the centre to that vertex), make sure you use correct MOI
import sys
 
import pygame
from pygame.locals import *
import math
from glm import *
import random
 
white = (255,255,255)
black = (0,0,0)
red = (255,0,0)
green = (0,255,0)
blue = (0,0,255)
 
pygame.init()
 
fps = 60
fpsClock = pygame.time.Clock()
 
width, height = 640, 480
screen = pygame.display.set_mode((width, height))
 
t = 0.0
dt = 0.01
depth_epsilon = 1.0

gravity = vec2(0,2000)
 
current_time = pygame.time.get_ticks()

def perp(v):
    return vec2(-v.y, v.x)

class Wall:
    def __init__(self):
        # define wall by plane equation
        self.normal = vec2(0,0)  # inward pointing
        self.c = 0 # ax + by + c = 0
        self.start_point = vec2(0,0)
        self.end_point = vec2(0,0)
        self.cm_position = 0
        self.orientation = 0
        self.cm_velocity = 0
        self.angular_velocity = 0

        self.mass = math.inf
        self.coefficient_of_restitution = 0.6
        self.friction = 0.3
        self.one_over_mass = 1.0 / self.mass
        self.one_over_cm_moment_of_inertia = 0

class RigidBody:

    def __init__(self, pos, sz, vel, orient, ang_vel, restitution, density, m):
        self.color = (255,0,0)
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

        # MOI for rect
        self.one_over_cm_moment_of_inertia = 1.0 / ((self.mass / 12.0) *
            (self.sz.x * self.sz.x + self.sz.y * self.sz.y))

        # MOI for circle
        #self.one_over_cm_moment_of_inertia = 1.0 / self.mass * (pow(math.pi * self.sz.x,4) / 4.0)

    def draw(self):
        c = pygame.draw.circle(screen, self.color, (self.position.x, self.position.y), self.sz.x)
        x,y = self.sz.x,0
        _x = x * math.cos(math.radians(self.repr_orientation)) - y * math.sin(math.radians(self.repr_orientation))
        _y = x * math.sin(math.radians(self.repr_orientation)) + y * math.cos(math.radians(self.repr_orientation))
        l = pygame.draw.line(screen, black, (self.position.x, self.position.y), (_x +self.position.x, _y + self.position.y), 3)


class Contact():
    def __init__(self, body_a, body_b):
        self.body_a = body_a
        self.body_b = body_b
        self.point = vec2(0,0)
        self.normal_impulse = 0
        self.penetration = 0
        self.relative_velocity = 0

class App():

    def __init__(self, *args, **kwargs):
        self.aBodies = []
        self.aWalls = []
        self.contacts = []

        w1 = Wall()
        w1.normal = vec2(0.5,-0.5)
        w1.c = 50
        w2 = Wall()
        w2.normal = vec2(-0.5,-0.5)
        w2.c = 400
        self.aWalls.append(w1)
        self.aWalls.append(w2)

        self.start_time = 0.0
        self.r1 = RigidBody(vec2(width/2,height/2), vec2(20,20), vec2(100,0), math.radians(0.0), 0.0, 1.0, 0.1, 1.0)
        self.r1.color = (255,0,0)
        self.r2 = RigidBody(vec2(width/2,height/2 + 50), vec2(20,20), vec2(100,0), math.radians(0.0), 0.0, 1.0, 0.1, 1.0)
        self.r2.color = (0,255,0)        
        self.aBodies.append(self.r1)
        #self.r2.one_over_mass = 0.0
        self.aBodies.append(self.r2)

    def check_for_collisions(self):
        # clear contacts, collect contacts every frame and resolve with impulse
        self.contacts = []
        for i in range(len(self.aBodies)):
            body = self.aBodies[i]
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
                        contact.normal = wall.normal
                        contact.penetration = depth
                        contact.relative_velocity = relative_velocity
                        self.contacts.append(contact)

        for i in range(len(self.aBodies)):
            bodya = self.aBodies[i]
            for j in range(i, len(self.aBodies)):
                bodyb = self.aBodies[j]
                if i != j:
                    depth = distance(bodya.cm_position, bodyb.cm_position) - (bodya.sz.x + bodyb.sz.x)
                    normal = normalize(bodyb.cm_position - bodya.cm_position)
                    pa = bodya.cm_position + normal * bodya.sz.x
                    pb = bodyb.cm_position - normal * bodyb.sz.x 
                    position = (pa + pb) * 0.5                   
                    CM_to_corner_perp = perp(position - bodyb.cm_position)
                    va = bodya.cm_velocity + CM_to_corner_perp * bodya.angular_velocity
                    vb = bodyb.cm_velocity + CM_to_corner_perp * bodyb.angular_velocity
                    relative_velocity = vb - va
                    if depth < depth_epsilon:
                        if dot(relative_velocity, normal) < 0.0:
                            contact = Contact(bodya, bodyb)
                            contact.point = position
                            contact.normal = normal * -1.0
                            self.contacts.append(contact)

    # you can change to verlet
    def integrate(self, deltaTime):
        for counter in range(len(self.aBodies)):
            body = self.aBodies[counter]
            body.cm_position += body.cm_velocity * deltaTime 
            body.orientation += body.angular_velocity * deltaTime
            body.cm_velocity +=  body.cm_force * (self.aBodies[counter].one_over_mass * deltaTime)
            body.angular_velocity += (self.aBodies[counter].one_over_cm_moment_of_inertia * deltaTime) * body.torque
    
    def compute_forces(self):

        for counter in range(len(self.aBodies)):
            # clear forces
            body = self.aBodies[counter]
            body.torque = 0.0
            body.cm_force = vec2(0,0);
            if (body.one_over_mass != 0):
                v = 1.0 / body.one_over_mass
                body.cm_force += gravity * v

    def resolve_collisions(self, contact):
        # collision response (normal) impulse
        body_a = contact.body_a
        body_b = contact.body_b
        position = contact.point
        impulse_direction = contact.normal
        CM_to_corner_perp = perp(position - body_a.cm_position)
        Velocity = body_a.cm_velocity + CM_to_corner_perp * body_a.angular_velocity
        impulse_numerator = -(1.0 + body_a.coefficient_of_restitution) * dot(Velocity,impulse_direction)
        perp_dot = dot(CM_to_corner_perp, impulse_direction)
        impulse_denominator = body_a.one_over_mass + body_a.one_over_cm_moment_of_inertia * perp_dot * perp_dot
        bias = 0
        k_min_penetration = 0.01
        k_biasFactor = 0.2
        bias = -k_biasFactor * 1.0 / 0.02 * min(0.0, contact.penetration + k_min_penetration)
        e = body_a.coefficient_of_restitution
        k_min_velocity = 1.0
        if contact.relative_velocity < -k_min_velocity:
            if e * contact.relative_velocity < bias:
                bias = -e * contact.relative_velocity

        normal_impulse = (impulse_numerator + bias) / impulse_denominator
        # clamp to positive impulses, a hack instead of accumulating impulses (see erin cattos presentation)
        #contact.normal_impulse += normal_impulse
        normal_impulse = max(0, normal_impulse)
        body_a.cm_velocity += impulse_direction * normal_impulse * body_a.one_over_mass
        body_a.angular_velocity += normal_impulse * body_a.one_over_cm_moment_of_inertia * perp_dot

        body_b.cm_velocity -= impulse_direction * normal_impulse * body_b.one_over_mass
        body_b.angular_velocity -= normal_impulse * body_b.one_over_cm_moment_of_inertia * perp_dot

        # friction (tangent) impulse
        impulse_direction = perp(contact.normal)
        CM_to_corner_perp = perp(position - body_a.cm_position)
        Velocity = body_a.cm_velocity + CM_to_corner_perp * body_a.angular_velocity
        impulse_numerator = -1.0 * dot(Velocity,impulse_direction)
        perp_dot = dot(CM_to_corner_perp, impulse_direction)
        impulse_denominator = body_a.one_over_mass + body_a.one_over_cm_moment_of_inertia * perp_dot * perp_dot
        tangent_impulse = impulse_numerator / impulse_denominator
        # clamp to friction limits (see catto)
        tangent_impulse = max(-1.0*body_a.friction * normal_impulse, tangent_impulse)
        tangent_impulse = min(tangent_impulse, body_a.friction * normal_impulse)

        body_a.cm_velocity += impulse_direction * tangent_impulse * body_a.one_over_mass
        body_a.angular_velocity += tangent_impulse * body_a.one_over_cm_moment_of_inertia * perp_dot

        body_b.cm_velocity -= impulse_direction * tangent_impulse * body_b.one_over_mass
        body_b.angular_velocity += tangent_impulse * body_b.one_over_cm_moment_of_inertia * perp_dot
###################

    def update(self):

        self.compute_forces()
        self.integrate(dt)
        self.check_for_collisions()
        for c in self.contacts:
            c.normal_impulse = 0
            self.resolve_collisions(c)

        for r in self.aBodies:
            r.position =  r.cm_position
            # hacky way of setting orientation to degrees for drawing instead of modifying
            # the physics orientation value
            r.repr_orientation = math.degrees(r.orientation)
            r.draw()

def run():
    app = App()
    while True:
        screen.fill((0, 0, 0))
      
        for event in pygame.event.get():
            if event.type == QUIT:
                pygame.quit()
                sys.exit()
            elif event.type == KEYDOWN:
                if event.key == K_ESCAPE:
                    pygame.quit()
                    sys.exit()

                if event.key == K_r:
                    app.aBodies[0].cm_position = vec2(width/2, height/2)
                    app.aBodies[0].cm_velocity = vec2(random.randrange(-100,100,10), random.randrange(-100,100,10))
                    app.aBodies[0].orientation = 0.0

                    app.aBodies[1].cm_position = vec2(width/2, height/2 + 50)
                    app.aBodies[1].cm_velocity = vec2(random.randrange(-100,100,10), random.randrange(-100,100,10))
                    app.aBodies[1].orientation = 0.0

                if event.key == K_q:
                    r2 = RigidBody(vec2(width/2, height/2), vec2(20,20), vec2(100,0), math.radians(0.0), 0.0, 1.0, 0.1, 1.0)
                    r2.color = (0,255,0)   
                    r2.cm_velocity = vec2(random.randrange(-100,100,10), random.randrange(-100,100,10))
                    app.aBodies.append(r2)

        app.update()
     
        pygame.display.flip()
        fpsClock.tick(fps)

if __name__ == "__main__":
    run()
