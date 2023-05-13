import sys
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from console import *
from axis_3d import *
from engine.graphics import *
from keys import *
import random
import time
import math
from OpenGL.GL import *
#from line2d import *

WIDTH = 800
HEIGHT = 600

ref = 0

gravity = vec2(0,-10)

def line_segment_intersection(p1, p2, p3, p4):

    # where p1 p2 is a line segment
    # and p3 p4 is a line segment
    # return value between 0 and 1 along p1 p2 segment

    x1, y1 = p1.x, p1.y
    x2, y2 = p2.x, p2.y
    x3, y3 = p3.x, p3.y
    x4, y4 = p4.x, p4.y

    # calculate u value along first line segment
    na = ((x4 - x3)*(y1 -  y3) - (y4 - y3)*(x1 - x3))
    da =  ((x2 - x1)*(y4 - y3)  - (y2 - y1)*(x4 - x3))
    if da != 0.0:
        ua =  na/da
    else:
        ua = -1
    nb = ((x2 - x1)*(y1 -  y3) - (y2 - y1)*(x1 - x3))
    db = ((x2 - x1)*(y4 - y3)  - (y2 - y1)*(x4 - x3))
    if db != 0.0:
        ub = nb/db
    else:
        ub = -1
       
    # return parametric value of point
    if (ua >= 0 and ua <= 1 and ub >= 0 and ub <= 1):
        # such that point of collision p = p3 + ub*(p4 - p3)
        # (the diagonal vector penetration depth is then 1 - ub)
        return ub
    else:
        # no intersection
        return -1

def rect_rect_collision(_poly1, _poly2):

    poly1 = _poly1
    poly2 = _poly2

    collision_info = vec2(0,0)

    for shape in range(2):

        # repeat the edge diagonal collisions for each poly respectively 
        if (shape == 1):
            poly1 = _poly2
            poly2 = _poly1
 
        for a in range(len(poly1.translated_vertices)):
            b = (a + 1) % len(poly1.translated_vertices)

            # test edges of poly1
            v1, v2 = poly1.translated_vertices[a], poly1.translated_vertices[b]

            # with diagonals of poly2
            v3 = poly2.position
            for i in range(len(poly2.translated_vertices)):
                v4 = poly2.translated_vertices[i]

                u = line_segment_intersection(v1, v2, v3, v4)
                if (u != -1):
                    collision_point = v3 + normalize(v4 - v3) * u
                    d = 1 - u
                    if collision_info == vec2(0,0) or length(normalize(v4 - v3) * d) < length(collision_info):
                        collision_info = normalize(v4 - v3) * d
    
    if (collision_info != vec2(0,0)):
        poly1.colliding = True
        poly2.colliding = True
        collision_normal = normalize(collision_info)
        d = length(collision_info)
        poly1.collision_vector = collision_normal * d
        poly2.collision_vector = poly1.collision_vector
        relative_velocity = poly2.velocity - poly1.velocity
        vel_along_normal = dot(relative_velocity, collision_normal )

        # factor1 = (poly1.inverse_mass / (poly1.inverse_mass + poly2.inverse_mass)) 
        # factor2 = (poly2.inverse_mass / (poly1.inverse_mass + poly2.inverse_mass)) 

        # poly1.collision_info = collision_info * -1
        # poly2.collision_info = collision_info 

        # poly1.position -= collision_info * factor1
        # poly2.position += collision_info * factor2

        poly1.get_translated_vertices()
        poly2.get_translated_vertices()

        e = 1
        # poly1.collision_point = collision_point
        # poly2.collision_point = collision_point


        # a = dot(r_ap, collision_normal) ** 2 * poly1.inverse_moment_of_inertia
        # b = dot(r_bp, collision_normal) ** 2 * poly2.inverse_moment_of_inertia

        #j = -(1 + e) * vel_along_normal / (dot(collision_normal, collision_normal) * (poly1.inverse_mass + poly2.inverse_mass) + a + b)
        
        j = -(1 + e) * vel_along_normal / (poly1.inverse_mass + poly2.inverse_mass)
        #print (vel_along_normal)


        # poly1.velocity -= collision_normal * j  * poly1.inverse_mass
        # poly2.velocity += collision_normal * j  * poly2.inverse_mass

        percent = 0.2 # usually 20% to 80%
        slop = 0.01 # usually 0.01 to 0.1

        poly1.position += poly1.collision_vector * 1.01
        poly2.position += poly2.collision_vector * 1.01
        # poly1.position += correction
        # poly2.position -= correction

        poly1.velocity = reflect(poly1.velocity, collision_normal*-1)
        poly2.velocity = reflect(poly2.velocity, collision_normal)

        # v_ap = circle1.velocity + circle1.angular_velocity * r_ap
        # v_bp = circle2.velocity + circle2.angular_velocity * r_bp

        # r_ap = normalize(perp(collision_point - poly1.position))
        # r_bp = normalize(perp(collision_point - poly1.position))

        # poly1.angular_velocity += dot(r_ap, collision_normal * j) 
        # poly2.angular_velocity += dot(r_bp, collision_normal * j)

    else:
        poly1.colliding = False
        poly2.colliding = False


def check_collision(circle1, circle2):
    d = (circle1.r + circle2.r) - distance(circle1.position, circle2.position)
    collision_point = vec2(0,0)
    if (d > 0):
        circle1.colliding = True
        circle2.colliding = True
        collision_normal = normalize(circle2.position - circle1.position)
        collision_point = circle1.position + collision_normal * circle1.r
 
        #pygame.draw.circle(screen, white, (collision_point.x, collision_point.y), 5)
        vel_along_normal = dot( circle1.velocity, collision_normal )
 
        correction = circle1.r + circle2.r - length(circle1.position - circle2.position)
        factor1 = (circle1.inverse_mass / (circle1.inverse_mass + circle2.inverse_mass)) 
        factor2 = (circle2.inverse_mass / (circle1.inverse_mass + circle2.inverse_mass)) 
        circle1.position -= collision_normal * correction * factor1
        circle2.position += collision_normal * correction * factor2
 
        e = 1.0
        circle1.collision_point = collision_point
        circle2.collision_point = collision_point
 
        r_ap = normalize(perp(collision_point - circle1.position))
        r_bp = normalize(perp(collision_point - circle2.position))
 
        circle1.seperating_normal = r_ap
        circle2.seperating_normal = r_ap
 
        a = dot(r_ap, collision_normal) ** 2 * circle1.inverse_moment_of_inertia
        b = dot(r_bp, collision_normal) ** 2 * circle2.inverse_moment_of_inertia
 
        j = -(1 + e) * vel_along_normal / (dot(collision_normal, collision_normal) * (circle1.inverse_mass + circle2.inverse_mass) + a + b)
        impulse1 =  collision_normal     * j  * circle1.inverse_mass
        impulse2 =  collision_normal *-1 * j  * circle2.inverse_mass
 
        circle1.velocity += impulse1
        circle2.velocity += impulse2

        v_ap = circle1.velocity + r_ap * circle1.angular_velocity 
        v_bp = circle2.velocity + r_bp * circle2.angular_velocity
 
        circle1.angular_velocity += dot(r_ap, collision_normal * j) 
        circle2.angular_velocity += dot(r_bp, collision_normal * j)
 
    else:
        circle1.colliding = False
        circle2.colliding = False


def rotate_point(pt, angle):
    x,y = pt.x, pt.y
    _x = x * math.cos(math.radians(angle)) - y * math.sin(math.radians(angle))
    _y = x * math.sin(math.radians(angle)) + y * math.cos(math.radians(angle))
    return vec2(_x, _y)

def perp(v):
    return vec2(-v.y, v.x)

def reflect(I, N):
    return I  + N  * -2.0 * dot(N, I) 

class Circle:
    all_circles = []
    def __init__(self, pos, vel, orient, ang_vel, r, mass):
        self.icon_rect = Rect2D(pos, vec2(r*2, r*2), "./data/red_circle.png", 1, 1)
        self.icon_rect.frame = random.randrange(0,20,1)
        self.position = pos
        self.prev_position = pos
 
        self.velocity = vel
        self.prev_velocity = vel
 
        self.orientation = orient
        self.prev_orientation = orient
 
        self.angular_velocity = ang_vel
        self.prev_angular_velocity = ang_vel
 
        self.collision_point = vec2(0,0)
        self.seperating_normal = vec2(0,0)
 
        self.colliding = False
  
        self.r = r
 
        # TO DO real MOI doesnt work
        self.inverse_moment_of_inertia = 100.0
 
        if (mass != math.inf):
            self.mass = mass
            self.inverse_mass = 1.0/mass
        else:
            self.mass = math.inf
            self.inverse_mass = 0.0

        Circle.all_circles.append(self)

    def update(self, force, dt):
 
        accel = force * self.inverse_mass
 
        self.velocity += accel  * dt
 
        self.position += self.velocity * dt
        if (self.colliding):
            r_ap = normalize(perp(self.collision_point - self.position))
            torque = dot(self.seperating_normal, accel)
            self.angular_velocity += torque * (self.inverse_moment_of_inertia) * dt
 
        self.orientation -= self.angular_velocity * dt
 
         # update graphics to match physics
        self.icon_rect.position = self.position
        self.icon_rect.orientation = self.orientation

class Rect:
    all_rects = []
    def __init__(self, pos, vel, orient, ang_vel, sz, m):
        self.icon_rect = Rect2D(pos, sz, "./data/block_atlas.png", 5, 1)
        self.icon_rect.frame = 0
        self.icon_rect.ordering = 1
        self.position = pos
        self.size = sz
        self.orientation = orient
        self.angular_velocity = ang_vel
        self.velocity = vel
        self.mass = m
        self.inverse_mass = 1.0/self.mass

        self.collision_vector = vec2(0,0)

        self.translated_vertices = []
        self.get_translated_vertices()

        Rect.all_rects.append(self)

    def update(self, force, dt):
        accel = force * self.inverse_mass
        self.velocity += accel  * dt
        self.position += self.velocity * dt
        self.orientation -= self.angular_velocity * dt
 
         # update graphics to match physics
        self.icon_rect.position = self.position
        self.icon_rect.orientation = self.orientation
        self.get_translated_vertices()

    def get_translated_vertices(self):
        self.translated_vertices = [ self.position.copy() + rotate_point(vec2( -self.size.x/2, -self.size.y/2), self.orientation),
                    self.position.copy() + rotate_point(vec2( -self.size.x/2,  self.size.y/2), self.orientation),
                    self.position.copy() + rotate_point(vec2(  self.size.x/2,  self.size.y/2), self.orientation),
                    self.position.copy() + rotate_point(vec2(  self.size.x/2, -self.size.y/2), self.orientation)  ]


Penetrating = 0
Colliding = 1
Clear = 2
Epsilon = 0.00001

class BoundingBox:
    def __init__(self):
        self.aVertices = [vec2(0,0), vec2(0,0), vec2(0,0), vec2(0,0)]

class Configuration:
    def __init__(self):
        self.CMPosition = vec2(0,0)
        self.Orientation = 0
        self.CMVelocity = vec2(0,0)
        self.AngularVelocity = 0
        self.CMForce = vec2(0,0)
        self.Torque = 0
        self.bounding_box = BoundingBox()

class RigidBody:
    aBodies = []
    def __init__(self, Density, _Width, _Height, _CoefficientOfRestitution, pos, vel, orient, ang_vel):
        self.aConfigurations = []

        self.Mass = Density * _Width * _Height

        self.coefficient_of_restitution = _CoefficientOfRestitution

        self.Width = _Width
        self.Height = _Height

        self.OneOverMass = 1.0 / self.Mass

        # integrate over the body to find the moment of inertia

        self.OneOverCMMomentOfInertia = 1.0 / ((self.Mass / 12.0) *
            (self.Width * self.Width + self.Height * self.Height))

        self.aConfigurations.append(Configuration())
        self.aConfigurations.append(Configuration())

        # 0-out non-vector quantities
        self.aConfigurations[0].Orientation = orient
        self.aConfigurations[0].AngularVelocity = ang_vel
        self.aConfigurations[0].Torque = 0.0

        self.aConfigurations[0].CMPosition = vec2(pos.x, pos.y)
        self.aConfigurations[0].CMVelocity = vec2(vel.x, vel.y)
        self.aConfigurations[0].AngularVelocity = ang_vel

        self.orientation = 0.0
        self.angularVelocity = 0.0
        self.torque = 0.0

        self.CMPosition = vec2(pos.x, pos.y)
        self.CMVelocity = vec2(vel.x, vel.y)
        self.angularVelocity = ang_vel

        id = len(RigidBody.aBodies)

        RigidBody.aBodies.append(self)

class PhysicsRect:
    def __init__(self, pos, sz, vel, orient, ang_vel, restitution, density, m):
        self.rect = Rect(vec2(0,0), vel, orient, ang_vel, sz, m)
        self.rigid_body = RigidBody(density,sz.x,sz.y,restitution,vec2(0,0),vel,orient,ang_vel)
    def update(self):
        self.rect.position = self.rigid_body.position + vec2(WIDTH/2, HEIGHT/2) #vec2(v[20]+WIDTH/2, HEIGHT/2+v[21])
        self.rect.orientation = math.degrees(self.rigid_body.orientation)

class Wall:
    def __init__(self):
        # define wall by plane equation
        self.normal = vec2(0,0)  # inward pointing
        self.c = 0 # ax + by + c = 0
        self.start_point = vec2(0,0)
        self.end_point = vec2(0,0)

class SimulationWorld:
    def __init__(self, WIDTH, HEIGHT):
        self.vertices = []
        self.collision_state = Clear
        self.collision_normal = vec2(0,0)
        self.colliding_body_index = 0
        self.colliding_corner_index = 0
        self.source_configuration_index = 0
        self.target_configuration_index = 1
        self._WIDTH = WIDTH
        self._HEIGHT = HEIGHT
        self.number_of_walls = 5
        self.aWalls = []
        self.number_of_bodies = 2

        w1 = Wall()
        w2 = Wall()
        w3 = Wall()
        w4 = Wall()
        w5 = Wall()

        # initialize walls
        w1.normal = vec2(0.0,-1.0)
        w1.c = self._HEIGHT/2.0 - 3.0

        w2.normal = vec2(0.0,1.0)
        w2.c = self._HEIGHT/2.0 - 3.0

        w3.normal = vec2(-1.0,0.0)
        w3.c = self._WIDTH/2.0 - 3.0

        w4.normal = vec2(1.0,0.0)
        w4.c = self._WIDTH/2.0 - 3.0

        w5.normal = normalize(vec2(0.5,1.0))
        w5.c = self._WIDTH/2.0

        self.aWalls.append(w1)
        self.aWalls.append(w2)
        self.aWalls.append(w3)
        self.aWalls.append(w4)
        self.aWalls.append(w5)


        # generate the wall lines
        for counter in range(self.number_of_walls):
        
            wall = self.aWalls[counter]

            # make a big line in the direction of the wall

            PointOnWall = wall.normal * -wall.c
            v = perp(wall.normal)
            t0 = -sys.float_info.max
            t1 =  sys.float_info.max

            # now clip the line to the walls

            for wall_index in range(self.number_of_walls):
            
                if (wall_index != counter):
                
                    ClipWall = [self.aWalls[wall_index]]

                    Denominator = dot(v,ClipWall[ref].normal)

                    if(math.fabs(Denominator) > Epsilon):
                    
                        # not coplanar

                        t = -(ClipWall[ref].c + dot(PointOnWall,ClipWall[ref].normal)) / Denominator

                        if Denominator > 0.0:
                        
                            # the clip wall's clipping the t0 side of line
                            if t > t0:
                                t0 = t
                        
                        else:
                            # it's clipping the t1 side
                            if t < t1:
                                t1 = t
            

            # make sure we got clipped
            assert((t0 != -sys.float_info.max) and (t1 != sys.float_info.max))
            # but not completely clipped
            assert(t0 < t1)

            wall.start_point = PointOnWall + v * t0
            wall.end_point = PointOnWall + v * t1

        self.start_time = 0.0
        
    def get_time(self):
        self.start_time += 0.005
        return self.start_time

    def calculate_vertices(self, configuration_index):
        for counter in range(len(RigidBody.aBodies)):
            o = RigidBody.aBodies[counter].aConfigurations[configuration_index].Orientation
            position = RigidBody.aBodies[counter].aConfigurations[configuration_index].CMPosition
            width = RigidBody.aBodies[counter].Width / 2.0
            height = RigidBody.aBodies[counter].Height / 2.0

            v1 = position + mat2(math.cos(o), math.sin(o), -math.sin(o), math.cos(o)) * vec2( width, height)
            v2 = position + mat2(math.cos(o), math.sin(o), -math.sin(o), math.cos(o)) * vec2( width, -height)
            v3 = position + mat2(math.cos(o), math.sin(o), -math.sin(o), math.cos(o)) * vec2( -width, -height)
            v4 = position + mat2(math.cos(o), math.sin(o), -math.sin(o), math.cos(o)) * vec2( -width, height)


            box = RigidBody.aBodies[counter].aConfigurations[configuration_index].bounding_box
            box.aVertices[0] = v1
            box.aVertices[1] = v2
            box.aVertices[2] = v3
            box.aVertices[3] = v4

    def render(self):

        self.vertices = []

        # draw walls
        for counter in range(self.number_of_walls):
            wall = self.aWalls[counter]
            self.vertices.append(wall.start_point.x)
            self.vertices.append(wall.start_point.y)
            self.vertices.append(wall.end_point.x)
            self.vertices.append(wall.end_point.y)

        # draw bodies
        for counter in range(len(RigidBody.aBodies)):
            self.vertices.append(RigidBody.aBodies[counter].aConfigurations[self.target_configuration_index].CMPosition.x)
            self.vertices.append(RigidBody.aBodies[counter].aConfigurations[self.target_configuration_index].CMPosition.y)
            self.vertices.append(RigidBody.aBodies[counter].aConfigurations[self.target_configuration_index].Orientation)

            RigidBody.aBodies[counter].position = vec2(RigidBody.aBodies[counter].aConfigurations[self.target_configuration_index].CMPosition.x,RigidBody.aBodies[counter].aConfigurations[self.target_configuration_index].CMPosition.y)
            RigidBody.aBodies[counter].orientation = RigidBody.aBodies[counter].aConfigurations[self.target_configuration_index].Orientation

    def check_for_collisions(self, configuration_index):
        self.collision_state = Clear
        depth_epsilon = 1.0
        half_width = WIDTH / 2.0
        half_height = HEIGHT / 2.0
        for body in range(len(RigidBody.aBodies)):
            if self.collision_state is not Penetrating:
                configuration = [RigidBody.aBodies[body].aConfigurations[configuration_index]]
                box = [configuration[ref].bounding_box]

                for counter in range(4):
                    if self.collision_state is not Penetrating:
                        position = box[ref].aVertices[counter]
                        CMToCornerPerp = perp(position - configuration[ref].CMPosition)
                        Velocity = configuration[ref].CMVelocity + CMToCornerPerp * configuration[ref].AngularVelocity
                        for wall_index in range(self.number_of_walls):
                            if self.collision_state is not Penetrating:
                                wall = [self.aWalls[wall_index]]
                                axbyc = dot(position, wall[ref].normal) + wall[ref].c
                                if axbyc < -depth_epsilon:
                                    self.collision_state = Penetrating
                                elif axbyc < depth_epsilon:
                                    relative_velocity = dot(wall[ref].normal, Velocity)
                                    if relative_velocity < 0.0:
                                        self.collision_state = Colliding
                                        self.collision_normal = wall[ref].normal
                                        self.colliding_corner_index = counter
                                        self.colliding_body_index = body

        return self.collision_state

    
    def integrate(self, deltaTime):
        for counter in range(len(RigidBody.aBodies)):
            source = RigidBody.aBodies[counter].aConfigurations[self.source_configuration_index]
            target = RigidBody.aBodies[counter].aConfigurations[self.target_configuration_index]
            target.CMPosition = source.CMPosition + source.CMVelocity * deltaTime 
            target.Orientation = source.Orientation + source.AngularVelocity * deltaTime
            target.CMVelocity = source.CMVelocity +  source.CMForce * (RigidBody.aBodies[counter].OneOverMass * deltaTime)
            target.AngularVelocity = source.AngularVelocity + (RigidBody.aBodies[counter].OneOverCMMomentOfInertia * deltaTime) * source.Torque
    
    def compute_forces(self, configuration_index):

        for counter in range(len(RigidBody.aBodies)):
            # clear forces
            body = [RigidBody.aBodies[counter]]
            configuration = [body[ref].aConfigurations[configuration_index]]
            configuration[ref].Torque = 0.0
            configuration[ref].CMForce = vec2(0,0);
            v = 1.0 / body[ref].OneOverMass
            configuration[ref].CMForce += gravity * v

    def resolve_collisions(self, configuration_index):
        body = [RigidBody.aBodies[self.colliding_body_index]]
        configuration = [body[ref].aConfigurations[configuration_index]]
        position = configuration[ref].bounding_box.aVertices[self.colliding_corner_index]
        CMToCornerPerp = perp(position - configuration[ref].CMPosition)
        Velocity = configuration[ref].CMVelocity + CMToCornerPerp * configuration[ref].AngularVelocity
        impulse_numerator = -(1.0 + body[ref].coefficient_of_restitution) * dot(Velocity,self.collision_normal)
        perp_dot = dot(CMToCornerPerp, self.collision_normal)
        impulse_denominator = body[ref].OneOverMass + body[ref].OneOverCMMomentOfInertia * perp_dot * perp_dot
        impulse = impulse_numerator / impulse_denominator

        configuration[ref].CMVelocity += self.collision_normal * impulse * body[ref].OneOverMass
        configuration[ref].AngularVelocity += impulse * body[ref].OneOverCMMomentOfInertia * perp_dot

        body[ref].orientation = configuration[ref].Orientation
        body[ref].angularVelocity = configuration[ref].AngularVelocity
        body[ref].torque = configuration[ref].Torque

        body[ref].CMPosition = configuration[ref].CMPosition
        body[ref].CMVelocity = configuration[ref].CMVelocity
        body[ref].angularVelocity = configuration[ref].AngularVelocity

    def simulate(self, deltaTime):
        CurrentTime = 0.0
        TargetTime = deltaTime
        while CurrentTime < deltaTime:
            self.compute_forces(self.source_configuration_index)
            self.integrate(TargetTime-CurrentTime)
            self.calculate_vertices(self.target_configuration_index)
            self.check_for_collisions(self.target_configuration_index)
            if self.collision_state == Penetrating:
                TargetTime = (CurrentTime + TargetTime) / 2.0
                assert(math.fabs(TargetTime - CurrentTime) > Epsilon)
            else:
                if self.collision_state == Colliding:
                    Counter = 0
                    while True:
                        self.resolve_collisions(self.target_configuration_index);
                        Counter += 1

                        if (not (self.check_for_collisions(self.target_configuration_index) == Colliding) and (Counter < 100)):
                            break;
                        
                    assert(Counter < 100)


                CurrentTime = TargetTime
                TargetTime = deltaTime

                if self.source_configuration_index == 0:
                    self.source_configuration_index = 1
                else: 
                    self.source_configuration_index = 0

                if self.target_configuration_index == 0:
                    self.target_configuration_index = 1
                else: 
                    self.target_configuration_index = 0


    def run(self):
        self.last_time = self.get_time()
        # use a fixed timestep until we implement a better integrator

        Time = self.last_time + 0.02

        self.simulate(Time - self.last_time)
        self.render()

        self.last_time = Time

class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        make_context_current(self.window)
        #set_cursor_visible(self.window, False)
        self.sky_box.load_skybox = False
        self.light = Light(vec3(1,1,1), vec3(1,1,1))

        self.gravity = vec2(0,0)

        self.total_force = self.gravity

        self.all_rects = []

        self.active_camera.position = vec3(0,0,0.15)

        self.simulation_world = SimulationWorld(WIDTH, HEIGHT)

        self.l1 = Line2D(vec2(0,0), vec2(0,0), WIDTH, HEIGHT)
        self.l2 = Line2D(vec2(0,0), vec2(0,0), WIDTH, HEIGHT)
        self.l3 = Line2D(vec2(0,0), vec2(0,0), WIDTH, HEIGHT)
        self.l4 = Line2D(vec2(0,0), vec2(0,0), WIDTH, HEIGHT)
        self.l5 = Line2D(vec2(0,0), vec2(0,0), WIDTH, HEIGHT)

        #self.c = Circle(vec2(self.lastX, self.lastY), vec2(0,0), r_sz, 0, r_sz, r_sz)
        #self.r = Rect(vec2(self.lastX, self.lastY), vec2(0,0), 0.0, 0.0, vec2(r_sz, r_sz), r_sz)
        self.r = PhysicsRect(vec2(100,100), vec2(40,20), vec2(20,0), math.radians(0.0), 0.0, 1.0, 0.1, 1.0)
        self.all_rects.append(self.r)

    def update(self):

        self.simulation_world.run()
        v = self.simulation_world.vertices

        self.l1.set_endpoints(vec2(v[0]+WIDTH/2, HEIGHT/2+v[1]), vec2(v[2]+WIDTH/2, HEIGHT/2+v[3]))
        self.l2.set_endpoints(vec2(v[4]+WIDTH/2, HEIGHT/2+v[5]), vec2(v[6]+WIDTH/2, HEIGHT/2+v[7]))
        self.l3.set_endpoints(vec2(v[8]+WIDTH/2, HEIGHT/2+v[9]), vec2(v[10]+WIDTH/2, HEIGHT/2+v[11]))
        self.l4.set_endpoints(vec2(v[12]+WIDTH/2, HEIGHT/2+v[13]), vec2(v[14]+WIDTH/2, HEIGHT/2+v[15]))
        self.l5.set_endpoints(vec2(v[16]+WIDTH/2, HEIGHT/2+v[17]), vec2(v[18]+WIDTH/2, HEIGHT/2+v[19]))
        

        self.process_input(self.window)

        # update
        for c in Circle.all_circles:
            c.update(self.total_force, self.deltaTime)

        for r in Rect.all_rects:
            r.update(self.total_force, self.deltaTime)
     
        for c1 in Circle.all_circles:
            for c2 in Circle.all_circles:
                if (c1 != c2):
                    d = check_collision(c1, c2)

        # for r1 in Rect.all_rects:
        #     for r2 in Rect.all_rects:
        #         if (r1 != r2):
        #             d = rect_rect_collision(r1, r2)

        # if (self.rect1.colliding):
        #     self.set_background_color(vec3(0,1,0))
        # else:
        #     self.set_background_color(vec3(0,0,0))

        #self.rect1.position = vec2(self.lastX, self.lastY)
        # TO DO why is this necessary to make a copy?
        #r = self.rect.position.copy() - r.size*0.5
        #self.l1.set_endpoints(vec2(0,0), r)
        #print (self.rect1.collision_vector)

        #self.rect1.position = self.r1.position + vec2(WIDTH/2, HEIGHT/2) #vec2(v[20]+WIDTH/2, HEIGHT/2+v[21])
        #self.rect1.orientation = math.degrees(self.r1.orientation)
        #self.rect1.update_physics()

        for r in self.all_rects:
            r.update()

        # self.rect2.position = self.r2.position + vec2(WIDTH/2, HEIGHT/2)
        # self.rect2.orientation = math.degrees(self.r2.orientation)

        # self.rect3.position = self.r3.position + vec2(WIDTH/2, HEIGHT/2)
        # self.rect3.orientation = math.degrees(self.r3.orientation)

        #self.rect2.orientation -= 0.1

    def process_input(self, window):
        pass

    def on_mouse_moved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = ypos - self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = HEIGHT - ypos

    def on_mouse_clicked(self, button, action, mods):
        pass

    def on_window_resized(self, width, height):
        pass

    def on_key_pressed(self, key, scancode, action, mods):
        if key == KEY_ESCAPE:
            set_window_should_close(self.window, True)

if __name__ == "__main__":
    app = App("2d-physics", WIDTH, HEIGHT, False, False)
    run(app)
