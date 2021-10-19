import sys
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from engine.graphics import *
from keys import *
from player import *
import random
import time
import math
from OpenGL.GL import *

WIDTH = 800
HEIGHT = 600

class App(Application):
    player = Player()
    map_model = StaticModel()
    arrow_model = StaticModel()
    arrow_entity = CharacterEntity()
    map_shader = StaticShader()

    outline_shader = StaticShader()

    other_player = StaticModel()

    sky_box = Skybox()
    sky_box_shader = StaticShader()

    crossHair = Rect()
    fps = Label()
    console = Label()
    camera = Camera(vec3(0.0, 0.0, 10.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0), -90.0, 0.0)
    model = mat4(1.0)
    view = lookAt(vec3(0.0, 0.0, 10.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0))

    projection = mat4(1.0)
    deltaTime = 0.0
    lastFrame = 0.0
    currentFrame = 0.0

    show_fps = True
    show_console = False
    cmd = ""

    map_position = mat4(1.0)
    
    def __init__(self, *args, **kwargs):
        pass

    def init(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        make_context_current(self.window)
        set_cursor_visible(self.window, False)
        self.setup()
        self.camera.Position = vec3(0.0, 0.0, 10.0)
        self.view = lookAt(vec3(0.0, 0.0, 10.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0))

    def setup(self):
        self.projection = perspective(45.0, float(WIDTH)/float(HEIGHT), 0.1, 100.0)

        self.map_model.loadModel("./data/map.obj")
        self.map_shader.setup("./data/shader.vs","./data/shader.fs")
        self.arrow_model.loadModel("./data/arrow.dae")
        self.arrow_model_matrix = mat4(1.0)
        #self.arrow_model_matrix = rotate(self.arrow_model_matrix, math.radians(195.0), vec3(0.0, 1.0, 0.0))
        #self.arrow_model_matrix = scale(self.arrow_model_matrix, vec3(0.04, 0.04, 0.04))
        self.outline_shader.setup("./data/outline_shader.vs","./data/outline_shader.fs")

        #self.other_player.loadModel("./data/dj_skully/dj_scully.obj")

        self.crossHair.setup_with_image("./data/crosshair.png")
        self.fps.setup("./data/Minecraftia.ttf")
        self.console.setup("/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf")
        self.all_objects = []
        self.map_position = mat4(1.0)
        collision_objects = [self.map_position]
        v = [self.map_model]
        self.player.setup(v, collision_objects, vec3(0.7, 1.2, 0.7))
        self.arrow_entity.setup(v, collision_objects, vec3(0.7, 1.2, 0.7))

        self.show_fps = True
        self.player.renderScope = True
        self.outlined = False

        self.sky_box_shader.setup("./data/skybox_shader.vs","./data/skybox_shader.fs")
        faces = [
            "./data/skybox/right.jpg",
            "./data/skybox/left.jpg",
            "./data/skybox/top.jpg",
            "./data/skybox/bottom.jpg",
            "./data/skybox/front.jpg",
            "./data/skybox/back.jpg"
        ]
        #faces_path = "/home/me/3d-graphics-project/examples/rubiks-cube/data/skybox/"

        self.sky_box.load(faces)
        self.rlastX = 0
        self.rlastY = 0

        self.dist = 0
        self.fire_arrow = False
        self.arrow_has_been_shot = False
        self.arrow_direction = vec3(0,0,0)
        self.arrow_yaw = 0
        self.arrow_pitch = 0
        self.arrow_position = vec3(0,0,0)


    def set_view(self, mat):
        self.map_shader.setMat4("view", mat)

    def draw(self):
        self.currentFrame = time.time()
        self.deltaTime = (self.currentFrame - self.lastFrame)
        self.lastFrame = self.currentFrame


        glEnable(GL_STENCIL_TEST);
        glEnable(GL_DEPTH_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); 

        glEnable(GL_DEPTH_CLAMP)
        self.sky_box_shader.use();
        #self.sky_box_shader.setInt("skybox", 0);

        #draw skybox as last

        sky_view = self.player.view.copy()
        
        sky_view[3] = vec4(0,0,0,0)# remove translation from the view matrix

        self.sky_box_shader.setMat4("MVP", self.projection * sky_view);

        self.sky_box.Draw(self.sky_box_shader)
        
        # uncomment from here: for stencil outlining
        if (self.fire_arrow):
            self.arrow_direction = self.player.camera.Front.copy()
            self.arrow_has_been_shot = True
            self.arrow_yaw = self.player.camera.Yaw
            self.arrow_pitch = self.player.camera.Pitch
            self.arrow_position = self.player.camera.Position.copy()
            self.fire_arrow = False

        if (self.arrow_has_been_shot):
            arrow_pos =self.arrow_position +  self.arrow_direction * (2 + self.dist) + self.player.camera.Right * 0.3
            self.dist += 1
            self.arrow_model_matrix = translate(mat4(1.0), arrow_pos)
            self.arrow_model_matrix = scale(self.arrow_model_matrix, vec3(0.06, 0.06, 0.06))
            self.arrow_model_matrix = rotate(self.arrow_model_matrix, math.radians(90.0), vec3(1,0,0))
            self.arrow_model_matrix = rotate(self.arrow_model_matrix, math.radians(self.arrow_yaw + 90.0), vec3(0,0,1))
            self.arrow_model_matrix = rotate(self.arrow_model_matrix, math.radians(self.arrow_pitch), vec3(1,0,0))
            print ("arrow has been shot")
        else:
            arrow_pos =self.player.camera.Position +  self.player.camera.Front * 2 + self.player.camera.Right * 0.3
            self.arrow_model_matrix = translate(mat4(1.0), arrow_pos)
            self.arrow_model_matrix = scale(self.arrow_model_matrix, vec3(0.06, 0.06, 0.06))
            self.arrow_model_matrix = rotate(self.arrow_model_matrix, math.radians(90.0), vec3(1,0,0))
            self.arrow_model_matrix = rotate(self.arrow_model_matrix, math.radians(self.player.camera.Yaw + 90.0), vec3(0,0,1))
            self.arrow_model_matrix = rotate(self.arrow_model_matrix, math.radians(self.player.camera.Pitch), vec3(1,0,0))

        self.arrow_entity.velocity = arrow_pos
        self.arrow_entity.checkCollision()
        print (self.arrow_entity.collisionPackage.foundCollision)
        #self.arrow_model_matrix = rotate(self.arrow_model_matrix, math.radians(195.0), vec3(0.0, 1.0, 0.0))

        # glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); 
        # glStencilMask(0x00); # make sure we don't update the stencil buffer while drawing the floor
          
        self.map_shader.use();
        self.map_shader.setMat4("MVP", self.player.projection * self.player.view * self.map_position)     
        self.map_model.Draw(self.map_shader)
        self.map_shader.use()
        self.map_shader.setMat4("MVP", self.player.projection  * self.player.view * self.arrow_model_matrix)     
        self.arrow_model.Draw(self.map_shader)
        # glStencilFunc(GL_ALWAYS, 1, 0xFF); 
        # glStencilMask(0xFF);

        # to here

        # self.other_player_position = vec3(-25.0, -3.0, 40.0)
        # other_player_mat = translate(mat4(1.0), self.other_player_position )
        # self.map_shader.setMat4("model", other_player_mat)
        # self.map_shader.setMat4("MVP", self.player.projection * self.player.view * other_player_mat)     

        # self.other_player.Draw(self.map_shader)

        # if (self.outlined):       
        #     glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        #     glStencilMask(0x00); 
        #     glDisable(GL_DEPTH_TEST);
        #     self.outline_shader.use()
        #     self.outline_shader.setMat4("model", other_player_mat)
        #     self.outline_shader.setMat4("view", self.player.view)
        #     self.outline_shader.setMat4("projection", self.player.projection)       
        #     self.other_player.Draw(self.outline_shader)
        #     glStencilMask(0xFF);
        #     glStencilFunc(GL_ALWAYS, 0, 0xFF);   
        #     glEnable(GL_DEPTH_TEST);  

        self.player.player_shader.use()
        self.player.Update(self.deltaTime, self.currentFrame)
        #glDisable(GL_DEPTH_TEST)

        self.player.camera.Position = self.player.entity.position

        self.crossHair.render(vec2(WIDTH/2, HEIGHT/2), vec2(25, 25), vec3(1.0, 0.0, 0.0))
        
        if self.show_console == True:
            set_alpha_transparency(1)
            self.console.render(">" + self.cmd, 20.0, 400.0, 0.5)

        if (self.show_fps):
            f = 0
            try:
                f = 1.0/(self.deltaTime)
            except:
                pass
            self.fps.render(str(int(f)), 20.0, 500.0, 0.5)




    def update(self):
        speed = 45.0
        self.processInput(self.window)
        if (is_joystick_present()):
            axes = FloatVector()
            axes = get_joystick_axes()
            xpos = (axes[3] + 1)*WIDTH/2 
            ypos = (axes[4] + 1)*HEIGHT/2
            xoffset = xpos - self.lastX
            yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

            self.lastX = xpos
            self.lastY = ypos

            self.player.camera.ProcessMouseMovement(axes[3]*speed, axes[4]*speed, True)
            #print (axes[2], axes[3])

            rxpos = (axes[1] + 1)*WIDTH/2 
            rypos = (axes[0] + 1)*HEIGHT/2

            rxoffset = rxpos - self.rlastX
            ryoffset = rypos - self.rlastY #reversed since y-coordinates go from bottom to top

            self.rlastX = rxpos
            self.rlastY = rypos
            tolerance = 25.0
            #print("axes: ", axes[1], axes[0])
            #print ("rx,ry: ", rxpos, rypos)
            #print ("xoffset,yoffset: ", rxoffset, ryoffset)
            if (math.fabs(WIDTH/2 - rxpos) > tolerance):
                #print ("width: ", WIDTH/2 - rypos)
                self.player.ProcessKeyboard(0, self.deltaTime*(WIDTH/2 - rxpos)*0.003)
            if (math.fabs(HEIGHT/2 - rypos) > tolerance):
                #print ("height: ", HEIGHT/2 - rypos)
                self.player.ProcessKeyboard(2, self.deltaTime*(HEIGHT/2 - rypos)*0.003)
        self.map_shader.use()
        self.set_view(self.player.view)
        self.draw()

    def processInput(self, window):
        if (get_key(window, KEY_W) == PRESS):
            self.player.ProcessKeyboard(0, self.deltaTime)
        if (get_key(window, KEY_S) == PRESS):
            self.player.ProcessKeyboard(1, self.deltaTime)
        if (get_key(window, KEY_A) == PRESS):
            self.player.ProcessKeyboard(2, self.deltaTime)
        if (get_key(window, KEY_D) == PRESS):
            self.player.ProcessKeyboard(3, self.deltaTime)
        if (get_key(window, KEY_R) == PRESS):
            self.player.entity.position.y = 40.0
            self.player.camera.Position.y = 40.0
    def onMouseMoved(self, xpos, ypos):
        xoffset = xpos - self.lastX
        yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

        self.lastX = xpos
        self.lastY = ypos
        self.player.camera.ProcessMouseMovement(xoffset, yoffset, True)

    def onMouseClicked(self, button, action, mods):
        if (button == 1 and action == 1):
            self.dist = 0
            self.fire_arrow = False
            self.arrow_has_been_shot = False
            self.arrow_direction = vec3(0,0,0)
            self.arrow_yaw = 0
            self.arrow_pitch = 0
            self.arrow_position = vec3(0,0,0)
        else:
            # x = (2.0 * WIDTH/2) / WIDTH - 1.0
            # y = 1.0 - (2.0 * HEIGHT/2) / HEIGHT
            # z = 1.0
            # ray_nds = vec3(x, y, z)
            # ray_clip = vec4(ray_nds.x, ray_nds.y, -1.0, 1.0)

            # # # eye space to clip we would multiply by projection so
            # # # clip space to eye space is the inverse projection
            # ray_eye = inverse(self.player.projection) * ray_clip

            # # # convert point to forwards
            # ray_eye = vec4(ray_eye.x, ray_eye.y, -1.0, 0.0)
            # # # world space to eye space is usually multiply by view so
            # # # eye space to world space is inverse view
            # #print (self.player.view)
            # inv_ray_wor = (inverse(self.player.view) * ray_eye)
            # ray_wor = vec3(inv_ray_wor.x, inv_ray_wor.y, inv_ray_wor.z)
            # ray_wor = normalize(ray_wor)
            # #print (ray_wor)
            # if (intersect(self.player.entity.position, ray_wor, self.other_player_position, 10.0)):
            #     print ("player 2 hit!")
            #     self.outlined = True
            # else:
            #     self.outlined = False
            self.fire_arrow = True

    def onWindowResized(self, width, height):
        pass

    def onKeyPressed(self, key, scancode, action, mods):
        if (key == KEY_1 and action == 1):
            self.player.player_model = self.player.ak47_model
            self.player.gunType = "AK47"
            self.player.renderScope = False
        if (key == KEY_2 and action == 1):
            self.player.player_model = self.player.awp_model
            self.player.gunType = "AWP"

        if (self.show_console == True and action == 1):
            try:
                self.cmd += chr(ascii_map[scancode])
            except(KeyError):
                pass

        if (self.show_console == True and key == KEY_BACKSPACE and action == 1):
            self.cmd = self.cmd[:len(self.cmd)-1]

        if (self.show_console == True and key == KEY_ENTER):
            if (self.cmd == "reset"):
                self.player.entity.position.y = 40.0
                self.player.camera.Position.y = 40.0

            if (self.cmd == "clearcolor"):
                set_clear_color(random.uniform(0.0,1.0), random.uniform(0.0,1.0),random.uniform(0.0,1.0))

            self.cmd = ""

        if (key == KEY_GRAVE_ACCENT and action == 1):
            if self.show_console == True:
                self.show_console = False
                self.cmd = ""
            else:
                self.show_console = True
        if (key == KEY_ESCAPE and action == 1):
            set_window_should_close(self.window, True);
            
        if (self.show_console is not True):
            if (key == KEY_SPACE and action == 1):
                self.player.jump(self.deltaTime)
            

if __name__ == "__main__":
    app = App("test", WIDTH, HEIGHT, False)
    app.init("test", WIDTH, HEIGHT, False)
    run(app)
