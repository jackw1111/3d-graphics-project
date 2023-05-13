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
ref = 0

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

class Configuration:
    def __init__(self):
        self.CMPosition = vec2(0,0)
        self.Orientation = 0
        self.CMVelocity = vec2(0,0)
        self.AngularVelocity = 0
        self.CMForce = vec2(0,0)
        self.Torque = 0
        self.bounding_box = BoundingBox()
        self.aBoundingVertices = [vec2(0,0), vec2(0,0), vec2(0,0), vec2(0,0)]

class RigidBody:
    aBodies = []

    def __init__(self, pos, sz, vel, orient, ang_vel, restitution, density, m):
        self.icon_rect = Rect2D(pos, sz, "./data/block_atlas.png", 5, 1)
        self.icon_rect.frame = 0
        self.icon_rect.ordering = 1

        self.aConfigurations = []

        self.sz = sz
        self.pos = vec2(0,0)

        self.Mass = density * self.sz.x * self.sz.y

        self.coefficient_of_restitution = restitution

        self.OneOverMass = 1.0 / self.Mass

        # integrate over the body to find the moment of inertia
        self.OneOverCMMomentOfInertia = 1.0 / ((self.Mass / 12.0) *
            (self.sz.x * self.sz.x + self.sz.y * self.sz.y))

        self.aConfigurations.append(Configuration())
        self.aConfigurations.append(Configuration())

        # 0-out non-vector quantities
        self.aConfigurations[0].Orientation = orient
        self.aConfigurations[0].AngularVelocity = ang_vel
        self.aConfigurations[0].Torque = 0.0

        self.aConfigurations[0].CMPosition = vec2(pos.x, pos.y)
        self.aConfigurations[0].CMVelocity = vec2(vel.x, vel.y)
        self.aConfigurations[0].AngularVelocity = ang_vel

        self.CMPosition = vec2(pos.x, pos.y)
        self.CMVelocity = vec2(vel.x, vel.y)
        self.angularVelocity = ang_vel

        id = len(RigidBody.aBodies)

        RigidBody.aBodies.append(self)

        self.cm_position = self.prev_cm_position = pos
        self.orientation =  self.prev_orientation = orient
        self.cm_velocity = self.prev_cm_velocity  = vel
        self.angular_velocity = self.prev_angular_velocity = ang_vel
        self.cm_force = self.prev_cm_force = vec2(0,0)
        self.torque = self.prev_torque = 0.0

    def update(self, config_index):
        self.position = self.aConfigurations[config_index].CMPosition + vec2(WIDTH/2, HEIGHT/2)
        self.orientation = math.degrees(self.aConfigurations[config_index].Orientation)
        self.icon_rect.position = self.position
        self.icon_rect.orientation = self.orientation

    def swap_values(self):
        self.prev_cm_position = self.cm_position
        # to do : somehow mat3(all 0's) doesnt work
        self.prev_orientation =  self.orientation
        self.prev_cm_velocity = self.cm_velocity
        self.prev_angular_velocity = self.angular_velocity
        self.prev_cm_force = self.cm_force
        self.prev_torque = self.torque

class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        make_context_current(self.window)
        #set_cursor_visible(self.window, False)
        self.sky_box.load_skybox = False
        self.light = Light(vec3(1,1,1), vec3(1,1,1))

        self.gravity = vec2(0,0)

        self.all_rects = []

        self.vertices = []
        self.collision_state = Clear
        self.collision_normal = vec2(0,0)
        self.colliding_body_index = 0
        self.colliding_corner_index = 0
        self.source_configuration_index = 0
        self.target_configuration_index = 1

        self.number_of_walls = 5
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

        self.l1 = Line2D(vec2(0,0), vec2(0,0), WIDTH, HEIGHT)
        self.l2 = Line2D(vec2(0,0), vec2(0,0), WIDTH, HEIGHT)
        self.l3 = Line2D(vec2(0,0), vec2(0,0), WIDTH, HEIGHT)
        self.l4 = Line2D(vec2(0,0), vec2(0,0), WIDTH, HEIGHT)
        self.l5 = Line2D(vec2(0,0), vec2(0,0), WIDTH, HEIGHT)

        self.r = RigidBody(vec2(100,100), vec2(40,20), vec2(20,0), math.radians(0.0), 0.0, 1.0, 0.1, 1.0)
        self.all_rects.append(self.r)
        self.vertices = []

        # draw walls
        for counter in range(self.number_of_walls):
            wall = self.aWalls[counter]
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

    def get_time(self):
        self.start_time += 0.005
        return self.start_time

    def calculate_vertices(self, configuration_index):
        for counter in range(len(RigidBody.aBodies)):
            o = RigidBody.aBodies[counter].aConfigurations[configuration_index].Orientation
            position = RigidBody.aBodies[counter].aConfigurations[configuration_index].CMPosition
            width = RigidBody.aBodies[counter].sz.x / 2.0
            height = RigidBody.aBodies[counter].sz.y / 2.0

            v1 = position + mat2(math.cos(o), math.sin(o), -math.sin(o), math.cos(o)) * vec2( width, height)
            v2 = position + mat2(math.cos(o), math.sin(o), -math.sin(o), math.cos(o)) * vec2( width, -height)
            v3 = position + mat2(math.cos(o), math.sin(o), -math.sin(o), math.cos(o)) * vec2( -width, -height)
            v4 = position + mat2(math.cos(o), math.sin(o), -math.sin(o), math.cos(o)) * vec2( -width, height)

            config = RigidBody.aBodies[counter].aConfigurations[configuration_index]
            config.aBoundingVertices[0] = v1
            config.aBoundingVertices[1] = v2
            config.aBoundingVertices[2] = v3
            config.aBoundingVertices[3] = v4

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
        for body in range(len(RigidBody.aBodies)):
            if self.collision_state is not Penetrating:
                configuration = [RigidBody.aBodies[body].aConfigurations[configuration_index]]
                box = [configuration[ref].bounding_box]

                for counter in range(4):
                    if self.collision_state is not Penetrating:
                        position = configuration[ref].aBoundingVertices[counter]
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
            body = RigidBody.aBodies[counter]
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
        position = configuration[ref].aBoundingVertices[self.colliding_corner_index]
        CMToCornerPerp = perp(position - configuration[ref].CMPosition)
        Velocity = configuration[ref].CMVelocity + CMToCornerPerp * configuration[ref].AngularVelocity
        impulse_numerator = -(1.0 + body[ref].coefficient_of_restitution) * dot(Velocity,self.collision_normal)
        perp_dot = dot(CMToCornerPerp, self.collision_normal)
        impulse_denominator = body[ref].OneOverMass + body[ref].OneOverCMMomentOfInertia * perp_dot * perp_dot
        impulse = impulse_numerator / impulse_denominator

        configuration[ref].CMVelocity += self.collision_normal * impulse * body[ref].OneOverMass
        configuration[ref].AngularVelocity += impulse * body[ref].OneOverCMMomentOfInertia * perp_dot

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

                self.reset_bodies_data()

    def update(self):

        self.last_time = self.get_time()
        # use a fixed timestep until we implement a better integrator

        Time = self.last_time + 0.02

        self.simulate(Time - self.last_time)

        self.last_time = Time

        for r in self.all_rects:
            r.update(self.source_configuration_index)

    def on_key_pressed(self, key, scancode, action, mods):
        if key == KEY_ESCAPE:
            set_window_should_close(self.window, True)
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
