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
    def __init__(self):
        # define wall by plane equation
        self.normal = vec2(0,0)  # inward pointing
        self.c = 0 # ax + by + c = 0
        self.start_point = vec2(0,0)
        self.end_point = vec2(0,0)

class RigidBody:
    aBodies = []

    def __init__(self, pos, sz, vel, orient, ang_vel, restitution, density, m):
        self.icon_rect = Rect2D(pos, sz, "./data/block_atlas.png", 5, 1)
        self.icon_rect.frame = 0
        self.icon_rect.ordering = 1

        self.cm_position = pos
        self.orientation = orient
        self.cm_velocity = vel
        self.angular_velocity = ang_vel
        self.cm_force = vec2(0,0)
        self.torque = 0
        self.a_bounding_vertices = [vec2(0,0), vec2(0,0), vec2(0,0), vec2(0,0)]

        self.sz = sz

        self.mass = density * self.sz.x * self.sz.y
        self.coefficient_of_restitution = 0.6
        self.friction = 0.3
        self.one_over_mass = 1.0 / self.mass

        # integrate over the body to find the moment of inertia
        self.one_over_cm_moment_of_inertia = 1.0 / ((self.mass / 12.0) *
            (self.sz.x * self.sz.x + self.sz.y * self.sz.y))

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

        self.vertices = []
        self.collision_state = Clear
        self.collision_normal = vec2(0,0)
        self.colliding_body_index = 0
        self.colliding_corner_index = 0
        self.target_configuration_index = 0

        self.aWalls = []

        w1 = Wall()
        w2 = Wall()
        w3 = Wall()
        w4 = Wall()
        w5 = Wall()

        # initialize walls
        w1.normal = vec2(0.0,-1.0)
        w1.c = HEIGHT/2.0 - 3.0

        w2.normal = vec2(0.0,1.0)
        w2.c = HEIGHT/2.0 - 3.0

        w3.normal = vec2(-1.0,0.0)
        w3.c = WIDTH/2.0 - 3.0

        w4.normal = vec2(1.0,0.0)
        w4.c = WIDTH/2.0 - 3.0

        w5.normal = normalize(vec2(0.5,1.0))
        w5.c = WIDTH/2.0

        self.aWalls.append(w1)
        self.aWalls.append(w2)
        self.aWalls.append(w3)
        self.aWalls.append(w4)
        self.aWalls.append(w5)

        # generate the wall lines
        for counter,wall in enumerate(self.aWalls):
        
            # make a big line in the direction of the wall

            PointOnWall = wall.normal * -wall.c
            v = perp(wall.normal)
            t0 = -sys.float_info.max
            t1 =  sys.float_info.max

            # now clip the line to the walls

            for wall_index,ClipWall in enumerate(self.aWalls):
            
                if (wall_index != counter):
                
                    Denominator = dot(v,ClipWall.normal)

                    if(math.fabs(Denominator) > Epsilon):
                    
                        # not coplanar

                        t = -(ClipWall.c + dot(PointOnWall,ClipWall.normal)) / Denominator

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

        self.l1 = Line2D(vec2(0,0), vec2(0,0), WIDTH, HEIGHT)
        self.l2 = Line2D(vec2(0,0), vec2(0,0), WIDTH, HEIGHT)
        self.l3 = Line2D(vec2(0,0), vec2(0,0), WIDTH, HEIGHT)
        self.l4 = Line2D(vec2(0,0), vec2(0,0), WIDTH, HEIGHT)
        self.l5 = Line2D(vec2(0,0), vec2(0,0), WIDTH, HEIGHT)

        self.vertices = []

        # draw walls
        for wall in self.aWalls:
            self.vertices.append(wall.start_point.x)
            self.vertices.append(wall.start_point.y)
            self.vertices.append(wall.end_point.x)
            self.vertices.append(wall.end_point.y)

        v = self.vertices

        self.l1.set_endpoints(vec2(v[0]+WIDTH/2, HEIGHT/2+v[1]), vec2(v[2]+WIDTH/2, HEIGHT/2+v[3]))
        self.l2.set_endpoints(vec2(v[4]+WIDTH/2, HEIGHT/2+v[5]), vec2(v[6]+WIDTH/2, HEIGHT/2+v[7]))
        self.l3.set_endpoints(vec2(v[8]+WIDTH/2, HEIGHT/2+v[9]), vec2(v[10]+WIDTH/2, HEIGHT/2+v[11]))
        self.l4.set_endpoints(vec2(v[12]+WIDTH/2, HEIGHT/2+v[13]), vec2(v[14]+WIDTH/2, HEIGHT/2+v[15]))
        self.l5.set_endpoints(vec2(v[16]+WIDTH/2, HEIGHT/2+v[17]), vec2(v[18]+WIDTH/2, HEIGHT/2+v[19]))


        self.start_time = 0.0
        self.r = RigidBody(vec2(100,100), vec2(40,40), vec2(20,0), math.radians(0.0), 0.0, 1.0, 0.1, 1.0)

    def get_time(self):
        self.start_time += 0.005
        return self.start_time

    def calculate_vertices(self, configuration_index):
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


    def check_for_collisions(self, configuration_index):
        self.collision_state = Clear
        depth_epsilon = 1.0
        half_width = WIDTH / 2.0
        half_height = HEIGHT / 2.0
        for i in range(len(RigidBody.aBodies)):
            body = RigidBody.aBodies[i]
            if self.collision_state is not Penetrating:

                for counter in range(4):
                    if self.collision_state is not Penetrating:
                        position = body.a_bounding_vertices[counter]
                        CMToCornerPerp = perp(position - body.cm_position)
                        Velocity = body.cm_velocity + CMToCornerPerp * body.angular_velocity
                        for wall_index,wall in enumerate(self.aWalls):
                            if self.collision_state is not Penetrating:
                                axbyc = dot(position, wall.normal) + wall.c
                                if axbyc < -depth_epsilon:
                                    self.collision_state = Penetrating
                                elif axbyc < depth_epsilon:
                                    relative_velocity = dot(wall.normal, Velocity)
                                    if relative_velocity < 0.0:
                                        self.collision_state = Colliding
                                        self.collision_normal = wall.normal
                                        self.colliding_corner_index = counter
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

    def resolve_collisions(self, configuration_index):
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
        normal_impulse = max(0, normal_impulse)
        body.cm_velocity += impulse_direction * normal_impulse * body.one_over_mass
        body.angular_velocity += normal_impulse * body.one_over_cm_moment_of_inertia * perp_dot

        # friction (tangent) impulse
        impulse_direction = perp(self.collision_normal)
        CMToCornerPerp = perp(position - body.cm_position)
        Velocity = body.cm_velocity + CMToCornerPerp * body.angular_velocity
        impulse_numerator = -1.0 * dot(Velocity,impulse_direction)
        perp_dot = dot(CMToCornerPerp, impulse_direction)
        impulse_denominator = body.one_over_mass + body.one_over_cm_moment_of_inertia * perp_dot * perp_dot
        tangent_impulse = impulse_numerator / impulse_denominator
        tangent_impulse = max(-1.0*math.sqrt(2)*body.friction * normal_impulse, tangent_impulse)
        tangent_impulse = min(tangent_impulse, body.friction * normal_impulse * math.sqrt(2))
        body.cm_velocity += impulse_direction * tangent_impulse * body.one_over_mass
        body.angular_velocity += tangent_impulse * body.one_over_cm_moment_of_inertia * perp_dot

    def simulate(self, deltaTime):
        CurrentTime = 0.0
        TargetTime = deltaTime
        while CurrentTime < deltaTime:
            self.compute_forces()
            self.integrate(TargetTime-CurrentTime)
            self.calculate_vertices(0)
            self.check_for_collisions(0)
            if self.collision_state == Penetrating:
                TargetTime = (CurrentTime + TargetTime) / 2.0
                assert(math.fabs(TargetTime - CurrentTime) > Epsilon)
            else:
                if self.collision_state == Colliding:
                    Counter = 0
                    while True:
                        self.resolve_collisions(0);
                        Counter += 1

                        if (not (self.check_for_collisions(0) == Colliding) and (Counter < 100)):
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
            self.r.icon_rect.position =  self.r.cm_position + vec2(WIDTH/2, HEIGHT/2)
            self.r.icon_rect.orientation = math.degrees(self.r.orientation)

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
