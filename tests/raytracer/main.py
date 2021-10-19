#!/usr/bin/python

import sys,os
sys.path.append("/home/me/Documents/3d-graphics-project/engine/bin")
sys.path.append("/home/me/Documents/3d-graphics-project/engine/utils")
from engine.graphics import *
from OpenGL.GL import *
from keys import *
from grid import *

import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Pango
from gi.repository import Gtk as gtk
from gi.repository import Gdk as gdk
from gi.repository import GObject as gobject
from gi.repository import GtkSource as gtksource
from gi.repository.GdkPixbuf import Pixbuf
from gi.repository import GtkSource
from gi.repository import GObject

import importlib
import re
import signal
import gc,os,stat
import threading
import cairo

import math
import random
import time

WIDTH = 420
HEIGHT = 236

import png

import numpy as np
import cv2

WIDTH = 25
HEIGHT = 25



# TO DO's:
# - threading the pixels
# - reflections / checkboard
# - make raytracing algorithm recursive
# - model loading
# - texturing
# - octree
# - skybox/mirrors
# - anti-aliasing
# - make rays check each object for shortest intersection
# - remove dependency of 3d engine, (make a true raytracer!)
# DONE
# - light position fixed
# - added shadows
# - added OOP abstractions
# - fix sphere positions
# - multiple lights/shadows

def lerp(input, input_start, input_end, output_start, output_end):
    return (output_end - output_start) / (input_end - input_start) * (input - input_start) + output_start

# limit it to range 0->255
def limit(color):
    color0 = max(0, min(color[0], 255))
    color1 = max(0, min(color[1], 255))
    color2 = max(0, min(color[2], 255))
    return [color0, color1, color2]

def reflect(I, N):
    return I  + N  * -2.0 * dot(N, I) 

class Ray():
    def __init__(self, origin, direction):
        self.origin = origin
        self.direction = direction

class Entity():
    scene_entities = []
    def __init__(self):
        Entity.scene_entities.append(self)

class Sphere(Entity):
    def __init__(self, position, radius, color):
        Entity.__init__(self)

        self.position = position
        self.radius = radius
        self.color = color
        self.normal = [0,0,0]
    # also black box method; how are a,b,c calculated from vectors??
    def intersect(self, ray):
        # using quadratic formula to solve equation for intersections of t
        a = 1
        b = 2 * dot(ray.origin - self.position, ray.direction)
        c = dot(ray.origin - self.position, ray.origin - self.position) - self.radius * self.radius
        # only interested in intersections infront of the camera
        if ((b*b - 4 * a * c) > 0):
            t1 = (-b + math.sqrt(b*b - 4 * a * c)) / 2 * a
            t2 = (-b - math.sqrt(b*b - 4 * a * c)) / 2 * a
            # return closest intersection
            if (t1 > t2):
                return t2
            else:
                return t1


    def get_normal(self, world_pos):
        self.normal = normalize(world_pos - self.position)
        return self.normal

def point_intersects_square(point, min_, max_):
    if point.x > min_.x and point.x < max_.x and point.y > min_.y and point.y < max_.y:
        return True
    else:
        return False

class Square(Entity):
    def __init__(self, position, normal, size, file):
        Entity.__init__(self)
        self.position = position
        self.normal = normal
        self.size = size
        self.color = [0,0,255]
        self.img = cv2.imread(file,1)

    def get_normal(self, world_pos):
        return self.normal

    def intersect(self, ray):

        min_max = [-1 * self.size,-1  * self.size,-1  * self.size,1  * self.size,1  * self.size,1  * self.size]
        min_ = vec3(min_max[0], min_max[1], min_max[2])
        max_ = vec3(min_max[3], min_max[4], min_max[5])

        min_xy = vec2(min_.x, min_.y)
        max_xy = vec2(max_.x, max_.y)

        _t1 = ray_intersect_plane(self.normal, self.position, ray.origin, ray.direction)
        if (_t1 > 0): # only raycast intersection in front of player
            t1 = ray.origin + ray.direction *  _t1
            # 2d plane intersection points
            t1_xy = vec2(t1.x, t1.y)

            if (point_intersects_square(t1_xy, min_xy, max_xy)):
                print (self.img.shape)
                rows,cols,_ = self.img.shape
                x_index = int(lerp(t1.x, -self.size, self.size, 0, 1) * cols)
                y_index = int(lerp(t1.y, -self.size, self.size, 0, 1) * rows)
                pixel= self.img[x_index, y_index]
                print (x_index, y_index)
                self.color = pixel
                return _t1

class Cube(Entity):
    def __init__(self, position, size):
        Entity.__init__(self)
        self.position = position
        self.size = size
        self.color = [0,0,255]
        self.normal = [0,0,0]

    def intersect(self, ray):
        intersect_normal = None
        intersection_normals = []
        t_intersects = []
        # AABB min and max
        min_max = [-1,-1,-1,1,1,1]
        min_ = vec3(min_max[0], min_max[1], min_max[2])
        max_ = vec3(min_max[3], min_max[4], min_max[5])

        min_xz = vec2(min_.x, min_.z)
        max_xz = vec2(max_.x, max_.z)

        _t1 = ray_intersect_plane(vec3(0,1,0), vec3(0,max_.y,0), ray.origin, ray.direction)
        if (_t1 > 0): # only raycast intersection in front of player
            t1 = ray.origin + ray.direction *  _t1
            # 2d plane intersection points
            t1_xz = vec2(t1.x, t1.z)

            if (point_intersects_square(t1_xz, min_xz, max_xz)):
                intersect_normal = vec3(0,1,0)
                intersection_normals.append(intersect_normal)
                t_intersects.append(_t1)

        _t2 = ray_intersect_plane(vec3(0,-1,0), vec3(0,min_.y,0), ray.origin, ray.direction)
        if (_t2 > 0):
            t2 = ray.origin + ray.direction *  _t2
            # 2d plane intersection points
            t2_xz = vec2(t2.x, t2.z)
            if (point_intersects_square(t2_xz, min_xz, max_xz)):
                intersect_normal = vec3(0,-1,0)
                intersection_normals.append(intersect_normal)
                t_intersects.append(_t2)


        min_yz = vec2(min_.y, min_.z)
        max_yz = vec2(max_.y, max_.z)
        _t3 = ray_intersect_plane(vec3(1,0,0), vec3(max_.x,0,0), ray.origin, ray.direction)
        if (_t3 > 0):
            t3 = ray.origin + ray.direction *  _t3
            # 2d plane intersection points
            t3_yz = vec2(t3.y, t3.z)

            if (point_intersects_square(t3_yz, min_yz, max_yz)):
                intersect_normal = vec3(1,0,0)
                intersection_normals.append(intersect_normal)
                t_intersects.append(_t3)

        _t4 = ray_intersect_plane(vec3(-1,0,0), vec3(min_.x,0,0), ray.origin, ray.direction)
        if (_t4 > 0):
            t4 = ray.origin + ray.direction *  _t4
            # 2d plane intersection points
            t4_yz = vec2(t4.y, t4.z)
            if (point_intersects_square(t4_yz, min_yz, max_yz)):
                intersect_normal = vec3(-1,0,0)
                intersection_normals.append(intersect_normal)
                t_intersects.append(_t4)

        min_xy = vec2(min_.x, min_.y)
        max_xy = vec2(max_.x, max_.y)

        _t5 =  ray_intersect_plane(vec3(0,0,1), vec3(0,0,max_.z), ray.origin, ray.direction)
        if (_t5 > 0):
            t5 = ray.origin + ray.direction * _t5
            # 2d plane intersection points
            t5_xy = vec2(t5.x, t5.y)

            if (point_intersects_square(t5_xy, min_xy, max_xy)):
                intersect_normal = vec3(0,0,1)
                intersection_normals.append(intersect_normal)
                t_intersects.append(_t5)

        _t6 = ray_intersect_plane(vec3(0,0,-1), vec3(0,0,min_.z), ray.origin, ray.direction)
        if (_t6 > 0):
            t6 = ray.origin + ray.direction *  _t6
            # 2d plane intersection points
            t6_xy = vec2(t6.x, t6.y)
            if (point_intersects_square(t6_xy, min_xy, max_xy)):
                intersect_normal = vec3(0,0,-1)
                intersection_normals.append(intersect_normal)
                t_intersects.append(_t6)

        closest_face = None
        t_intersect = None
        model_matrix = vec4(0,0,0,0)
        model_position = vec3(model_matrix[0], model_matrix[1], model_matrix[2])
        for i,intersection_normal in enumerate(intersection_normals):
                if (closest_face == None):
                    closest_face = intersection_normal
                    t_intersect = t_intersects[i]
                else:
                    current_face_dist = distance(ray.origin, model_position + intersection_normal)
                    closest_face_dist = distance(ray.origin, model_position + closest_face)

                    if (current_face_dist < closest_face_dist):
                        closest_face = intersection_normal
                        t_intersect = t_intersects[i]

        self.normal = closest_face
        return t_intersect

    def get_normal(self, world_pos):
        return self.normal

class Plane(Entity):
    def __init__(self, position, normal, color):
        Entity.__init__(self)

        self.position = position
        self.normal = normal
        self.color = color
        self.size = 10

    def intersect(self, ray):

        # scratch a pixel
        denom = dot(ray.direction, self.normal)
        p0l0 = self.position - ray.origin
        t = dot(p0l0, self.normal) / (denom + 0.0000001)
        p = ray.origin + ray.direction * t
        x = int(p[0])
        y = int(p[1])
        z = int(p[2])
        if (math.fabs(x) < self.size and math.fabs(z) < self.size):
            if (int(math.fabs(x)) % 2 == int(math.fabs(z)) % 2):
                self.color = [255,69,0]
            else:
                self.color = [87,8,97]
            return t

    def get_normal(self, world_pos):
        return self.normal

class Lamp():
    scene_lights = []
    def __init__(self, position):
        Lamp.scene_lights.append(self)

        self.position = position
        self.intensity = 0.1
        self.constant = 1.0
        self.linear = 0.09
        self.quadratic = 0.032


class App(Application):

    def __init__(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)

    def setup(self, title, WIDTH, HEIGHT, fullscreen):

        super().setup(title, WIDTH, HEIGHT, fullscreen)
        self.sky_box.load_skybox = False

        # make_context_current(self.window)
        # set_cursor_visible(self.window, False)
        self.set_background_color(vec3(1.0, 1.0, 1.0))
        self._setup()

    def _setup(self):

        self.show_shadows = False
        self.show_ssao = False
        self.light = Light(vec3(0, 1, 4), vec3(1,1,1))
        self.start_time = time.time()
        self.axis_3d = StaticObject("../../data/3d_axis.obj")
        #self.grid = Grid()

        self.sky_box.load_skybox = False
        self.light = Light(vec3(0, 1, 4), vec3(1,1,1))

        self.ball = StaticObject("./data/ball.obj")

        self.ball2 = StaticObject("./data/ball.obj")
        self.ball2.model_matrix = translate(scale(mat4(1.0), vec3(0.05, 0.05, 0.05)), vec3(-1,0,0))
        self.ball3 = StaticObject("./data/ball.obj")
        self.ball3.model_matrix = scale(mat4(1.0), vec3(0.05, 0.05, 0.05))

        self.plane1 = StaticObject("./data/plane.obj")
        self.plane1.model_matrix = translate(mat4(1.0), vec3(0,-1,0))

        self.box = StaticObject("./data/cube.obj")

        # self.start_pos = vec3(2,2,2)
        # self.ray_dir = vec3(0,0,-1)
        # self.end_pos = self.start_pos + self.ray_dir * 4
        # self.ray = Line3D(self.start_pos, self.end_pos)
        # self.ray.color = vec3(1,1,0)
        # self.active_camera.position = vec3(0,0,4)
        # self.ray2 = Line3D(vec3(0,0,0), vec3(1,0,0))

        self.s = 'P6\n\n' + str(WIDTH) + " " + str(HEIGHT) + ' \n255\n'
        self.header = bytes(self.s, 'ascii')
        self.pixels = self.init_pixels()

        self.scene_square = Square(vec3(0,0,-10), vec3(0,0,1), 50, "/home/me/Documents/3d-graphics-project/tests/ray-sphere-intersection/data/front.jpg")
        self.scene_plane = Plane(vec3(0,-1, 0), vec3(0,1,0), [0,0,0])
        self.scene_sphere_large = Sphere(vec3(2.5,0,0), 1.2, [255,0,0])
        self.scene_box = Cube(vec3(0,0,0), 1)
        self.scene_sphere = Sphere(vec3(-2,0,0), 1, [0,255,0])
        self.scene_light = Lamp(vec3(3,2,3))
        self.scene_light = Lamp(vec3(-3,2,3))

        self.shadow = [0.2, 0.2, 0.2]

        self.mouse_down = False
        self.scrolled = False
        self.dist_from_centre = 1.0

        self.action = 0
        self.key = 0


    def init_pixels(self):
        pixels = []
        for j in range(HEIGHT):
            w = []
            for i in range(WIDTH):
                w.append(bytes([0,0,0]))
            pixels.append(w)
        return pixels

    def write_pixel(self, x, y, rgb):

        x = min(x, WIDTH)
        y = min(y, HEIGHT)
        r = min(max(int(rgb[0]), 0), 255)
        g = min(max(int(rgb[1]), 0), 255)
        b = min(max(int(rgb[2]), 0), 255)
        self.pixels[y][x] = bytes([r,g,b])

    def shadow_check(self, obj, world_position, light):
        pixel_to_light_dir = normalize(light.position - world_position)
        shadow_ray = Ray(world_position, pixel_to_light_dir)
        for current_object in Entity.scene_entities:
            if (obj == current_object):
                pass
            else:
                t = current_object.intersect(shadow_ray)
                if (t is not None):
                    return True
        return False


    def light_pixel(self, normal, world_position, color, light):
        # TO DO fix lighting
        r,g,b = color
        light_direction = normalize(light.position - world_position)
        diffuse = max(dot(normal, light_direction), 0.0)
        ambient = 0.05

        distance = length(light.position - world_position);
        attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance))    
 
        view_direction = normalize(self.active_camera.position - world_position);
        reflect_direction = reflect(view_direction * -1, normal);  
        specular = pow(max(dot(view_direction, reflect_direction), 0.0), 32);
        diffuse   *= attenuation
        specular *= attenuation  
        r *= (ambient + diffuse + specular)
        g *= (ambient + diffuse + specular)
        b *= (ambient + diffuse + specular)

        return [int(r), int(g), int(b)]

    def render(self):

        print ("rendering")

        for j in range(HEIGHT):
            for i in range(WIDTH):

                fovx = 90.0
                fovy = 90.0
                b = math.tan(math.radians(fovy/2)) * (j - HEIGHT/2)/(HEIGHT/2);
                a = math.tan(math.radians(fovx/2)) * (i - WIDTH/2)/(WIDTH/2);

                pixelRayDirection = normalize(self.active_camera.right * a + self.active_camera.up * b + self.active_camera.front)
                pixelRay = Ray(self.active_camera.position, pixelRayDirection)
                #print (i,j)
                # shoot a ray from the each pixel into the screen
                pixel_screen_direction = ray_cast(i, j, self.active_camera.projection_matrix, self.active_camera.view_matrix)
                screen_ray = Ray(self.active_camera.position, pixel_screen_direction)
                print (pixelRayDirection, pixel_screen_direction)

                nearest_object = None
                nearest_t = None
                for current_object in Entity.scene_entities:

                    t = current_object.intersect(screen_ray)
                    if (t is not None and t > 0):
                        if (nearest_object == None or t < nearest_t):
                            nearest_object = current_object
                            nearest_t = t

                if (nearest_object != None):
                    current_object = nearest_object
                    t = nearest_t
                    # shade pixel
                    if (t is not None and t > 0):
                        world_pos = screen_ray.origin + screen_ray.direction * t
                        normal = current_object.get_normal(world_pos)

                        total_shadow_pixel = [0,0,0]
                        total_lit_pixel = [0,0,0]

                        for scene_light in Lamp.scene_lights:

                            # accumulate lighting for each light
                            lit_pixel = self.light_pixel(normal, world_pos, current_object.color, scene_light)
                            total_lit_pixel = np.add(total_lit_pixel, lit_pixel)

                            # accumulate shadow for each light
                            if (self.shadow_check(current_object, world_pos, scene_light)):
                                total_shadow_pixel = np.add(total_shadow_pixel, self.shadow)

                            # accumulate reflection for each light
                            reflection_ray = Ray(world_pos, normal)
                            closest_object = None
                            shortest_dist = None
                            for scene_object in Entity.scene_entities:
                                if type(scene_object) == Square:
                                    continue
                                r = scene_object.intersect(reflection_ray)
                                normal = scene_object.get_normal(world_pos)

                                if (r is not None and r > 0):
                                    if (closest_object == None or r < shortest_dist):
                                        shortest_dist = r
                                        closest_object = scene_object

                            if (closest_object != None):
                                # calculate reflection off closest object
                                lit_pixel = self.light_pixel(normal, world_pos, closest_object.color, scene_light)
                                total_lit_pixel = np.add(total_lit_pixel, lit_pixel)


                        # invert shadow scalar so larger inputs values darken when multiplied with lighting
                        # and [0,0,0] default shadow value has no effect (ie. multiplying by [1,1,1])
                        total_shadow_pixel = np.subtract([1,1,1], total_shadow_pixel)
                        total_lit_shadow_pixel = np.multiply(total_lit_pixel, total_shadow_pixel)

                        self.write_pixel(i,j, total_lit_shadow_pixel)

    def save_file(self, file_name):
        # save pixel data to a file
        with open(file_name, 'wb') as f:
            f.write(self.header)
            # one-liner to write data
            f.write(b''.join(sum(self.pixels, [])))

    def update(self):
        #self.fbo.Draw(self.fbo_shader)
        self.processInput(self.window)

    def processInput(self, window):
        print ("action:", self.action)
        if (self.action == 1):
            if (self.key == 119):
                self.active_camera.ProcessKeyboard(0, self.deltaTime)

            if (self.key == 115):
                self.active_camera.ProcessKeyboard(1, self.deltaTime)

            if (self.key == 97):
                self.active_camera.ProcessKeyboard(2, self.deltaTime)

            if (self.key == 100):
                self.active_camera.ProcessKeyboard(3, self.deltaTime)
            if (self.key == 114):
                self.render()
                self.save_file("test.pnm")
                output_image = gtk.Image.new_from_file("test.pnm")    
                w = gtk.Window()
                w.add(output_image)
                w.show_all()


    def onMouseMoved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos
        self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)

    def onMouseClicked(self, button, action, mods):
        if (button == MOUSE_BUTTON_1 and action == 1):
            self.mouse_down = True
        if (button == MOUSE_BUTTON_1 and action == 0):
            self.mouse_down = False
        print (button, action)

    def onMouseScrolled(self, xpos, ypos):
        self.dist_from_centre -= ypos * 0.1
        modelHeight = 12.0 

        # algorithm from ThinMatrix video on third person cameras
        horizDist = modelHeight * math.cos(math.radians(self.active_camera.pitch))
        vertDist = modelHeight * math.sin(math.radians(self.active_camera.pitch))
        theta = self.active_camera.yaw - 90.0
        offsetx = horizDist * math.sin(math.radians(-theta))
        offsetz = horizDist * math.cos(math.radians(theta))
        self.active_camera.position = vec3(-offsetx, vertDist, -offsetz) * self.dist_from_centre

        # override behaviour of active_camera.ProcessMouseMovement
        # make sure you calculate front AND Right
        # otherwise the app's frustum culling won't function properly

        self.active_camera.front = self.active_camera.position * -1.0
        self.active_camera.Right = normalize(cross(self.active_camera.front, vec3(0,1,0)))
    def onWindowResized(self, width, height):
        pass

    def onKeyPressed(self, key, scancode, action, mods):
        print (key, self.deltaTime, self.currentFrame, self.lastFrame)
        self.key = key
        self.action = action


class DrawArea(gtk.GLArea):

    app = App()

    def __init__(self, WIDTH, HEIGHT):
        gtk.GLArea.__init__(self)
        self.connect("realize", self.on_realize)
        self.connect("render", self.render)
        self.connect("motion-notify-event", self.on_drawing_area_mouse_motion)
        self.connect("button-press-event", self.on_drawing_area_button_press)
        self.connect("button-release-event", self.on_drawing_area_button_release)
        self.connect("scroll-event", self.on_drawing_area_scroll_event)
        self.connect("key-press-event", self.on_drawing_area_key_press)
        self.connect("key-release-event", self.on_drawing_area_key_release)

        self.set_has_depth_buffer(True)

        self.lastX = 0
        self.lastY = 0

        self.set_size_request(400, 400)
        self.set_can_focus(True)



    def on_drawing_area_scroll_event(self, widget, event):
        did_scroll, dx, dy = event.get_scroll_deltas()
        self.app.onMouseScrolled(dx, dy)

    def on_drawing_area_mouse_motion(self, widget, event):
        xpos, ypos = event.x, event.y
        print (xpos, ypos)
        self.app.onMouseMoved(xpos, ypos)

    def on_drawing_area_button_release(self, widget, event): 
        self.app.onMouseClicked(0, 0, None)

    def on_drawing_area_button_press(self, widget, event): 
        self.app.onMouseClicked(0, 1, None)

    
    def on_drawing_area_key_press(self, widget, event):
        self.app.onKeyPressed(event.keyval, 0, 1, 0)

     
    def on_drawing_area_key_release(self, widget, event):
        self.app.onKeyPressed(event.keyval, 0, 0, 0)
   
    def on_realize(self, area):
        print ("on realize")
        ctx = self.get_context()
        ctx.make_current()
        print (ctx)
        err = self.get_error()
        if err:
            print("The error is {}".format(err))



        # load GLAD
        load_GL()

        self.app.setup("dont load this", WIDTH, HEIGHT, False)

        display = area.get_display()
        cursor = gdk.Cursor.new_for_display(display, gdk.CursorType.BLANK_CURSOR)
        area.get_window().set_cursor(cursor)

    def render(self, area, ctx):

        WIDTH = self.get_allocation().width
        HEIGHT = self.get_allocation().height
        #glViewport(0,0, WIDTH, HEIGHT);

        self.queue_render()
        self.app.gameLoop()

class Editor:

    def __init__(self,project_path, tabs=[]):

        # renders windows out of focus
        signal.signal(signal.SIGINT, self.signal_handler)

        self.window = gtk.Window(gtk.WindowType.TOPLEVEL)
        self.theme = "dark"    # [light/dark] type of gtk theme in use
        self.syntax_theme = "oblivion"
        self.window.connect("delete_event", self.delete_event_cb)
        self.window.connect("destroy", self.destroy_cb)
        self.window.connect("draw", self.on_window_draw)
        self.window.set_default_size(1000, 600);
        #self.window.maximize()
        self.sub_frame = gtk.VBox()
        self.main_frame = gtk.Grid(orientation=gtk.Orientation.VERTICAL)


        self.area = DrawArea(600, 500)
        self.area.set_can_focus(True)

        self.vertical_panel = gtk.HPaned()
        self.vertical_panel2 = gtk.HPaned()
        self.horizontal_panel = gtk.VPaned()
        self.horizontal_panel.set_border_width(5)
        self.vertical_panel.set_border_width(5)
        self.vertical_panel2.set_border_width(5)


        self.vertical_panel.set_position(0)

        self.horizontal_panel.pack1(self.area, True, False)
        self.horizontal_panel.set_position(200)
        self.vertical_panel2.pack1(self.horizontal_panel, True, False)
        self.vertical_panel2.set_position(1000)
        self.ui_panel = gtk.HPaned()
        self.entry = gtk.Entry()
        self.entry.set_text("0.125*math.sin(4*x)-math.cos(5*y)")
        self.button = gtk.Button.new_with_label("Show")
        self.ui_panel.pack1(self.entry, True, False)
        self.ui_panel.pack2(self.button, True, False)
        self.button.connect("clicked", self.on_button_clicked)
        #hbox.pack_start(button, True, True, 0)

        self.main_frame.add(self.vertical_panel2)
        self.main_frame.add(self.ui_panel)
        self.window_frame = gtk.Box(orientation=gtk.Orientation.VERTICAL)
        self.window_frame.add(self.main_frame)

        self.area.set_hexpand(True)
        self.area.set_vexpand(True)
        self.sub_frame.set_hexpand(False)
        self.sub_frame.set_vexpand(False)
        self.main_frame.set_hexpand(True)
        self.main_frame.set_vexpand(True)
        self.window_frame.set_hexpand(True)
        self.window_frame.set_vexpand(True)
        self.window.add(self.window_frame)

        self.area.set_events(gdk.EventMask.ALL_EVENTS_MASK)
        #self.area.add_events(gdk.EventMask.BUTTON_PRESS_MASK)
        self.area.connect("key-press-event", self.on_window_key_press, None)
        self.area.connect("button-press-event", self.on_window_clicked, None)
        self.area.connect("focus-in-event", self.focus_in)

        self.window.add_events(gdk.EventMask.ALL_EVENTS_MASK)
        self.window.connect("set-focus", self.set_focus)

        self.set_theme(self.theme)
        self.window.set_title("grapher")
        self.window.show_all()
        self.check_sigint_timer(1)
        gtk.main()

    def on_button_clicked(self, button):
        pass

    def on_window_key_press(self, widget, event, data):
        if (widget == self.area):
            modal_drawarea_window = None

        #if event.keyval == gdk.KEY_Escape:
            #modal_drawarea_window.destroy()
            #self.add_drawarea()

    def quit_activated(self):
        dialog = gtk.MessageDialog(parent=self.window, type=gtk.MessageType.QUESTION, buttons=gtk.ButtonsType.YES_NO)   
        dialog.set_title("Question")
        dialog.set_position(gtk.WindowPosition.CENTER_ALWAYS)
        dialog.set_markup("Are you sure you want to quit?")
        response = dialog.run()
        if response == gtk.ResponseType.YES:
            dialog.destroy()
            gtk.main_quit()
        elif response == gtk.ResponseType.NO:
            dialog.destroy()

    def delete_event_cb(self, widget, data=None):
        print("delete_event signal occurred")
        self.quit_activated()
        return True

    def destroy_cb(self, widget, data=None):
        print("destroy signal occurred")
        self.quit_activated()

    def signal_handler(self, signal, frame):
        print('\nYou pressed Ctrl+C!, exiting')
        # gtk.main_quit()
        engine.terminate()
        sys.quit()
        # gtk.Window.present(self.window)
        # self.window.grab_focus()


    def set_syntax_theme(self, buffer=None):
        manager = gtksource.StyleSchemeManager().get_default()
        #print (manager.get_scheme_ids())
        scheme = manager.get_scheme(self.syntax_theme)
        if (buffer):
            manager = gtksource.StyleSchemeManager().get_default()
            scheme = manager.get_scheme(self.syntax_theme)
            buffer.set_style_scheme(scheme)


    def set_theme(self, t):

        settings = gtk.Settings.get_default()
        settings.set_property("gtk-application-prefer-dark-theme", True)  # if you want use dark theme, set second arg to True
        self.syntax_theme = "oblivion"
        self.set_syntax_theme()


    def check_sigint_timer(self,timeout):
        gobject.timeout_add_seconds(timeout, self.check_sigint)

    def check_sigint(self):
        return True
    def on_selection_changed(self, button):
        self.selected_file = button.get_current_folder()

    def on_selection_changed2(self, button):
        self.selected_file = button.get_filename()


    def remove_drawarea(self):
        self.horizontal_panel.remove(self.area)

    def delete_drawarea(self):
        self.horizontal_panel.get_child1().destroy()

    def add_drawarea(self):
        self.area.set_hexpand(True)
        self.area.set_vexpand(True)
        self.horizontal_panel.pack1(self.area, True, False)


    def can_focus(self, widget):
        pass

    def set_focus(self, window, widget):
        print (widget, widget.has_focus())

    def on_window_clicked(self, widget, event, user_param):
        pass
    def focus_in(self, widget, data):
        print ("focus")

    def on_window_draw(self, window, cr):
        widget = window.get_focus()

if __name__ == "__main__":
        ed = Editor("", [])
