import sys
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from engine.graphics import *
from keys import *
import random
import time
import math

WIDTH = 800
HEIGHT = 600

def perp(v):
    return vec2(-v.y, v.x)

def reflect(I, N):
    return I  + N  * -2.0 * dot(N, I) 

def aabb_collision(rect1, rect2):
    if rect2.position.y + rect1.size.y/2 > rect1.position.y - rect1.size.y/2 and \
       rect1.position.y + rect1.size.y/2 > rect2.position.y - rect1.size.y/2 and \
       rect2.position.x + rect1.size.x/2 > rect1.position.x - rect1.size.x/2 and \
       rect1.position.x + rect1.size.x/2 > rect2.position.x - rect1.size.x/2:
       return True
    else:
        return False

class Line2D(Line3D):

    def convert_to_screenspace(self, camera,  point):

        # TO DO get corners of frustum

        # nearDist = 0.1
        # ar = WIDTH/HEIGHT
        # Hnear = 2* math.tan(camera.fov/2) * nearDist
        # Wnear = Hnear * ar
        # Cnear = camera.position + normalize(camera.front) * nearDist
        # topRightNear = Cnear + (camera.up * (Hnear / 2)) + (camera.right * (Wnear / 2));
        # bottomLeftNear = Cnear - (camera.up * (Hnear /2)) - (camera.right * (Wnear / 2));

        # print (topRightNear.x, bottomLeftNear.y)

        W = 0.03731000000000075
        H = 0.027879999999999034

        x1 = point.x
        y1 = point.y

        p1x = vec2(0, -W)
        p2x = vec2(WIDTH, W)

        p1y = vec2(0, -H)
        p2y = vec2(HEIGHT, H)

        mx = (p2x.y - p1x.y) / (p2x.x - p1x.x)
        my = (p2y.y - p1y.y) / (p2y.x - p1y.x)

        # (x1, y1) = (0, -W)
        # y - y1 = m(x- x1)
        # y + W = m(x - 0)
        # y = mx - W
        _x = mx * point.x - W
        _y = my * point.y - H

        point.x = _x
        point.y = _y

        return point

    def __init__(self, start, end, camera):
        self.camera = camera
        start = self.convert_to_screenspace(camera, start)
        end = self.convert_to_screenspace(camera, end)
        Line3D.__init__(self, vec3(start.x, start.y, -0.01), vec3(end.x, end.y, -0.01))

    def set_endpoints(self, start, end):
        start = self.convert_to_screenspace(self.camera, start)
        end = self.convert_to_screenspace(self.camera, end)
        Line3D.set_endpoints(self, vec3(start.x, start.y, -0.01), vec3(end.x, end.y, -0.01))

    @property
    def color(self):
        return self.color

    @color.setter
    def color(self, c):
        self.color = c

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

class Circle():
    all_circles = []
    def __init__(self, pos, vel, orient, ang_vel, r, mass):
        self.icon_rect = Rect2D(pos, vec2(r*2, r*2), "./data/balls.png", 5, 1)
        self.icon_rect.frame = 1
        self.icon_rect.ordering = 2
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
 
        #self.orientation -= self.angular_velocity * dt
 
         # update graphics to match physics
        self.icon_rect.position = self.position
        self.icon_rect.orientation = self.orientation



class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        make_context_current(self.window)
        set_cursor_visible(self.window, True)
        self.sky_box.load_skybox = False
        self.set_background_color(vec3(0,0,0))

        self.light = Light(vec3(1,1,1), vec3(1,1,1))
        self.player = Rect2D(vec2(WIDTH/2, 75), vec2(75, 75), "./data/bubble_trouble_character_sprite_sheet.png", 8, 3)
        self.player.ordering = 2
        self.start_time = time.time()
        self.background = Rect2D(vec2(WIDTH/2, HEIGHT/2), vec2(WIDTH, HEIGHT), "./data/background.png", 1,1)
        self.background.ordering = 1

        self.gravity = vec2(0,-1000)
        self.total_force = self.gravity

        self.rope = []

        self.to_animate_rope = False
        self.rope_start_time = time.time()
        self.rope_end_time = time.time()
        self.rope_elapsed_time = 0

        for i in range(1,14*5+1,1):
            _i = i
            while _i > 14:
                _i -= 14

            rope_segment = Rect2D(vec2(WIDTH/2, 0) + vec2(0, 30 + i*9), vec2(16, 9), "./data/rope_segment" + str(_i) + ".png", 1,1)
            #rope_segment.ordering = 2
            self.rope.append(rope_segment)

        self.c = Circle(vec2(WIDTH/2, 3*HEIGHT/4), vec2(0,0), 50.0, 0, 50.0, 5.0)


    def animate_rope(self):
        self.rope_end_time = time.time()
        self.rope_elapsed_time = self.rope_end_time - self.rope_start_time

        if self.rope_elapsed_time > 2.0:
            for i,rope_segment in enumerate(self.rope):
                rope_segment.ordering = 0
            self.to_animate_rope = False
            return

        t = int(self.rope_elapsed_time*30)
        for i,rope_segment in enumerate(self.rope):
            if i == t:
                rope_segment.ordering = 2

    def reset(self):

        self.rope = []

        self.to_animate_rope = False
        self.rope_start_time = time.time()
        self.rope_end_time = time.time()
        self.rope_elapsed_time = 0

        for i in range(1,14*5+1,1):
            _i = i
            while _i > 14:
                _i -= 14

            rope_segment = Rect2D(vec2(WIDTH/2, 0) + vec2(0, 30 + i*9), vec2(16, 9), "./data/rope_segment" + str(_i) + ".png", 1,1)
            #rope_segment.ordering = 2
            self.rope.append(rope_segment)

        for c in Circle.all_circles:
            c.icon_rect.ordering = 0

        Circle.all_circles = []

        self.c = Circle(vec2(WIDTH/2, HEIGHT/2), vec2(0,0), 50.0, 0, 50.0, 5.0)

        self.player.position = vec2(WIDTH/2, 75)



    def update(self):
        self.process_input(self.window)
        self.end_time = time.time()
        self.elapsed_time = self.end_time - self.start_time

        if self.to_animate_rope:
            self.animate_rope()

        # update
        for c in Circle.all_circles:
            c.update(self.total_force, self.deltaTime)
     
        for c1 in Circle.all_circles:
            for c2 in Circle.all_circles:
                if (c1 != c2):
                    d = check_collision(c1, c2)

        for c in Circle.all_circles:
            if c.position.y - c.r < 40.0:
                c.velocity.y = -c.velocity.y

            if c.position.x - c.r > WIDTH:
                c.velocity.x = -c.velocity.x

            if c.position.x - c.r < 0.0:
                c.velocity.x = -c.velocity.x

            if aabb_collision(self.player, c):
                pass
                #self.reset()

        for i,rope_segment in enumerate(self.rope):
            for c in Circle.all_circles:

                if aabb_collision(rope_segment, c) and rope_segment.ordering == 2 and c.icon_rect.to_draw:
                    r_vx = random.randrange(-500,500,1)
                    r_vy = random.randrange(-500,500,1)
                    r_sz = random.randrange(10, 50, 10)
                    r_sz = random.randrange(10, 50, 10)
                    r_x, r_y = random.randrange(-30,30,10), random.randrange(-30,30,10)
                    if (c.r > 10.0):
                        self.c2 = Circle(c.position + vec2(r_x, r_y), vec2(0,0), c.r/2, 0, c.r/2, c.r/2)
                        c.icon_rect.size = c.icon_rect.size*0.5
                        c.r = c.r * 0.5
                    else:
                        c.icon_rect.ordering = 0
                        c.icon_rect.to_draw = False

                    for rope_segment in self.rope:
                        rope_segment.ordering = 0
                    self.to_animate_rope = False


    def process_input(self, window):
        if (get_key(window, KEY_ESCAPE) == PRESS):
            set_window_should_close(self.window, True)

        key_pressed = False

        if (get_key(window, KEY_A) == PRESS):
            #self.active_camera.ProcessKeyboard(2, self.deltaTime)
            self.player.position -= vec2(1.0, 0.0)
            v = self.elapsed_time
            v = int(5 * v) % 32 + 24

            if (v < 32):
                v += 8
            while (v > 39):
                v -= 8

            self.player.frame = v
            key_pressed = True

        if (get_key(window, KEY_D) == PRESS):
            #self.active_camera.ProcessKeyboard(3, self.deltaTime)
            self.player.position += vec2(1.0, 0.0)
            v = self.elapsed_time
            v = int(5 * v) % 16 + 16

            if (v < 24):
                v += 8

            self.player.frame = v
            key_pressed = True

        if not key_pressed:
            self.player.frame = 40

    def on_mouse_moved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = HEIGHT - ypos

    def on_mouse_clicked(self, button, action, mods):
        if button == MOUSE_BUTTON_1 and action == PRESS:
            r_vx = random.randrange(-500,500,1)
            r_vy = random.randrange(-500,500,1)
            r_sz = random.randrange(10, 50, 10)
            r_sz = random.randrange(10, 50, 10)
            self.c = Circle(vec2(self.lastX, self.lastY), vec2(0,0), r_sz, 0, r_sz, r_sz)

    def on_window_resized(self, width, height):
        pass

    def on_key_pressed(self, key, scancode, action, mods):
        if key == KEY_SPACE and action == PRESS:
            for rope_segment in self.rope:
                rope_segment.position = vec2(self.player.position.x, rope_segment.position.y)
                rope_segment.ordering = 0
                self.rope_start_time = time.time()
                self.to_animate_rope = True

if __name__ == "__main__":
    app = App("bubble-trouble", WIDTH, HEIGHT, False, False)
    run(app)
