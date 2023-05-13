import sys
 
import pygame
from pygame.locals import *
import math
import glm
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
 
current_time = pygame.time.get_ticks()
accumulator = 0.0
 
 
def reflect(I, N):
    return I  + N  * -2.0 * glm.dot(N, I) 
 
def perp(v):
    return glm.vec2(-v.y, v.x)
 
class Circle():
    def __init__(self, pos, vel, orient, ang_vel, r, mass):
 
        self.position = pos
        self.prev_position = pos
 
        self.velocity = vel
        self.prev_velocity = vel
 
        self.orientation = orient
        self.prev_orientation = orient
 
        self.angular_velocity = ang_vel
        self.prev_angular_velocity = ang_vel
 
        self.collision_point = glm.vec2(0,0)
        self.seperating_normal = glm.vec2(0,0)
 
        self.colliding = False
 
        self.color = red
 
        self.r = r
 
        # TO DO real MOI doesnt work
        self.inverse_moment_of_inertia = 100.0
 
        if (mass != math.inf):
            self.mass = mass
            self.inverse_mass = 1.0/mass
        else:
            self.mass = math.inf
            self.inverse_mass = 0.0
 
    def update(self, force, dt):
 
        self.prev_position = self.position
        self.prev_velocity = self.velocity
        self.prev_orientation = self.orientation
        self.prev_angular_velocity = self.angular_velocity
        accel = force * (self.inverse_mass)
 
        self.velocity += accel  * dt
 
        self.position += self.velocity * dt
        if (self.colliding):
            r_ap = glm.normalize(perp(self.collision_point - self.position))
            torque = glm.dot(self.seperating_normal, accel)
            self.angular_velocity += torque * (self.inverse_moment_of_inertia) * dt
 
        self.orientation -= self.angular_velocity * dt
 
    def interpolate(self, accumulator, dt):
        alpha = accumulator / dt
        self.position = self.position * alpha + self.prev_position * ( 1.0 - alpha )
        self.velocity = self.velocity * alpha + self.prev_velocity * ( 1.0 - alpha )
        self.orientation = self.orientation * alpha + self.angular_velocity * ( 1.0 - alpha )
 
    def draw(self):
        c = pygame.draw.circle(screen, self.color, (self.position.x, self.position.y), self.r)
        x,y = self.r,0
        _x = x * math.cos(math.radians(self.orientation)) - y * math.sin(math.radians(self.orientation))
        _y = x * math.sin(math.radians(self.orientation)) + y * math.cos(math.radians(self.orientation))
        l = pygame.draw.line(screen, black, (self.position.x, self.position.y), (_x +self.position.x, _y + self.position.y), 3)
 
def check_collision(circle1, circle2):
    d = (circle1.r + circle2.r) - glm.distance(circle1.position, circle2.position)
    collision_point = glm.vec2(0,0)
    if (d > 0):
        circle1.colliding = True
        circle2.colliding = True
        collision_normal = glm.normalize(circle2.position - circle1.position)
        collision_point = circle1.position + collision_normal * circle1.r
 
        pygame.draw.circle(screen, white, (collision_point.x, collision_point.y), 5)
        vel_along_normal = glm.dot( circle1.velocity, collision_normal )
 
        correction = circle1.r + circle2.r - glm.length(circle1.position - circle2.position)
        factor1 = (circle1.inverse_mass / (circle1.inverse_mass + circle2.inverse_mass)) 
        factor2 = (circle2.inverse_mass / (circle1.inverse_mass + circle2.inverse_mass)) 
        circle1.position -= collision_normal * correction * factor1
        circle2.position += collision_normal * correction * factor2
 
        e = 1.0
        circle1.collision_point = collision_point
        circle2.collision_point = collision_point
 
        r_ap = glm.normalize(perp(collision_point - circle1.position))
        r_bp = glm.normalize(perp(collision_point - circle2.position))
 
        circle1.seperating_normal = r_ap
        circle2.seperating_normal = r_ap
 
        a = glm.dot(r_ap, collision_normal) ** 2 * circle1.inverse_moment_of_inertia
        b = glm.dot(r_bp, collision_normal) ** 2 * circle2.inverse_moment_of_inertia
 
        j = -(1 + e) * vel_along_normal / (glm.dot(collision_normal, collision_normal) * (circle1.inverse_mass + circle2.inverse_mass) + a + b);
        impulse1 =  collision_normal * j  * circle1.inverse_mass;
        impulse2 = -collision_normal * j  * circle2.inverse_mass;
 
        circle1.velocity += impulse1;
        circle2.velocity += impulse2;
 
        # v_ap = circle1.velocity + circle1.angular_velocity * r_ap
        # v_bp = circle2.velocity + circle2.angular_velocity * r_bp
 
        # circle1.angular_velocity += glm.dot(r_ap, collision_normal * j) 
        # circle2.angular_velocity += glm.dot(r_bp, collision_normal * j)
 
    else:
        circle1.colliding = False
        circle2.colliding = False
 
all_circles = []
circle1 = Circle(glm.vec2(width/2, height/2 - 100), glm.vec2(2,2), 0.0, 10.0, 20.0, 20.0)
circle2 = Circle(glm.vec2(width/2, height - 200), glm.vec2(1,-1), 0.0, 0.0, 50.0, 50.0)
circle2.color = blue
all_circles.append(circle1)
all_circles.append(circle2)
# gravity
total_force = glm.vec2(0,10)
 
time_factor = 1.0
 
# Game loop.
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
            if event.key == K_p:
                time_factor = 0.1
 
            if event.key == K_c:
                x, y = pygame.mouse.get_pos()
                value = random.randrange(50,100,10)
                angular_vel = random.randrange(1,10,1)
                c = Circle(glm.vec2(x,y), glm.vec2(2,2), 0.0, angular_vel, value, value)
                c.color = random.choice([white, blue, red, green])
                all_circles.append(c)
 
            if event.key == K_v:
                x, y = pygame.mouse.get_pos()
                value = random.randrange(10,30,10)
                angular_vel = random.randrange(1,10,1)
                c = Circle(glm.vec2(x,y), glm.vec2(2,2), 0.0, angular_vel, value, value)
                c.color = random.choice([white, blue, red, green])
                all_circles.append(c)
        elif event.type == KEYUP:
            if event.key == K_p:
                time_factor = 1.0
 
    frame_time = fpsClock.get_time()
    if ( frame_time > 0.25 ):
        frame_time = 0.25
 
    accumulator += frame_time
 
    dt = time_factor * dt
    while ( accumulator >= dt ):
        # Update.
        for c in all_circles:
            c.update(total_force, dt)
        t += dt
        accumulator -= dt
 
    # interpolate position between frames
    for c in all_circles:
        c.interpolate(accumulator, dt)
 
    for c1 in all_circles:
        for c2 in all_circles:
            if (c1 != c2):
                d = check_collision(c1, c2)
 
    # Draw.
    for c in all_circles:
        c.draw()
 
    if (len(all_circles) > 10):
        all_circles = all_circles[1:]
 
 
    pygame.display.flip()
    fpsClock.tick(fps)