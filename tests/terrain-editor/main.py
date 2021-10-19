import sys
sys.path.append("../../engine/bin")
sys.path.append("../../engine/utils")
from engine.graphics import *
from player import *
from keys import *
import random
import time

WIDTH = 800
HEIGHT = 600

class App(Application):

    player = Player()
    map_shader = StaticShader()

    model = translate(mat4(1.0), vec3(0.0, 0.0, -5.0))
    model = rotate(model, math.radians(-90.0), vec3(1.0, 0.0, 0.0))
    view = mat4(1.0)
    projection = mat4(1.0)

    deltaTime = 0.0
    lastFrame = 0.0
    currentFrame = 0.0
    models_to_draw = []
    def __init__(self, *args, **kwargs):
        pass

    def init(self, *args, **kwargs):
        Application.__init__(self, *args, **kwargs)
        make_context_current(self.window)
        set_cursor_visible(self.window, False)
        self.setup()

    def setup(self):
        self.light = Light(vec3(0, 100, 0), vec3(1,1,1))

        self.map_shader.setup("./data/shader.vs","./data/shader.fs")
        self.testModel = StaticObject("./data/grass_block2.obj")
        _mesh = self.testModel.model.meshes[0]
        for i in _mesh.textures:
            print (i, i.id, i.type, i.path)
        #if texture hasn't been loaded already, load it
        texture = Texture()
        texture.id = texture_from_file("sand.png", "./data");
        texture.type = "texture_diffuse2"
        texture.path = "sand.png"
        _mesh.textures.append(texture);
        self.testModel.model.textures_loaded.append(texture); # store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        _mesh.setupMesh()

        #if texture hasn't been loaded already, load it
        texture2 = Texture()
        texture2.id = texture_from_file("water.png", "./data");
        texture2.type = "texture_diffuse3"
        texture2.path = "water.png"
        _mesh.textures.append(texture2);
        self.testModel.model.textures_loaded.append(texture2); # store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        _mesh.setupMesh()

        for i in _mesh.textures:
            print (i, i.id, i.type, i.path)
        v = [self.testModel.model]

        collisionObjects = []
        m = mat4(1.0)
        m = scale(m, vec3(1.0, 1.0, 1.0))
        #m = translate(m, vec3(0.0, 0.0, 2.0))

        collisionObjects.append(m)
        #self.blockGenerator.addBlock(vec3(4.0, 0.0, 0.0), self.player.entity)
        self.player.setup(v, collisionObjects, vec3(0.7, 1.2, 0.7))

        self.fps = Label("", vec2(20.0, 550.0), "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf", 0.5)

        self.position_label = Label("", vec2(20.0, 500.0), "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf", 0.5)

        self.front_label = Label("", vec2(20.0, 450.0), "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf", 0.5)

        self.console = Label("", vec2(20.0, 400.0), "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf", 0.5)

        self.projection = perspective(45.0, float(WIDTH)/float(HEIGHT), 0.1, 100.0)
        self.player.entity.position.y = 0.0
        self.player.camera.position.y = 0.0


        self.crosshair = Rect(vec2(WIDTH/2, HEIGHT/2), vec2(25, 25), "./data/crosshair.png")
        self.debug_mode = False
        self.show_console = False
        self.cmd = ""

    def update(self):
        self.processInput(self.window)
        self.map_shader.use()
        self.set_view(self.player.view)
        self.draw()

    def set_view(self, mat):
        self.map_shader.setMat4("view", mat)

    def draw(self):
        self.currentFrame = time.time()
        self.deltaTime = (self.currentFrame - self.lastFrame)
        self.lastFrame = self.currentFrame

        # draw
        if (self.player.cameraType == "FirstPersonCamera"):
            self.player.updateGravity(self.deltaTime)
            self.player.camera.Position = self.player.entity.position

        self.player.Update(self.deltaTime, self.currentFrame)
        if (self.player.cameraType == "ThirdPersonCamera"):
            self.player.entity.velocity *= 0.7
            self.player.entity.update()
            #self.player.camera.Position = self.player.entity.position

        # grass block
        self.map_shader.use()
        
        #m = scale(mat4(1.0), vec3(1.0, 1.0, 1.0))
        m = translate(mat4(1.0), vec3(1.0, 0.0, -1.0))
        self.map_shader.setMat4("model", m)
        self.map_shader.setMat4("view", self.player.view)
        self.map_shader.setMat4("projection", self.player.projection)
        print (vec2(-self.player.texture_coords.z/2, self.player.texture_coords.x/2))
        self.map_shader.setVec2("mousePosition", vec2(-self.player.texture_coords.z/2, self.player.texture_coords.x/2))

        #self.testModel.model.Draw(self.map_shader)
    
        #fps
        if (self.debug_mode):
            self.fps.text = str(int(1.0/(self.deltaTime)))
            position_string = vec3(int(self.player.entity.position.x), int(self.player.entity.position.y), int(self.player.entity.position.z)) 
            self.position_label.text = "position:" + str(position_string)
            self.front_label.text = "front:" + str(self.player.camera.front)

        if self.show_console == True:
            set_alpha_transparency(1)
            if (int(self.currentFrame) % 2 == 0):
                self.console.text = ">" + self.cmd +" "
            else:
                self.console.text = ">" + self.cmd + "_"

    def processInput(self, window):
        if (not self.show_console):
            if (get_key(window, KEY_W) == PRESS):
                self.active_camera.ProcessKeyboard(0, self.deltaTime)
            if (get_key(window, KEY_S) == PRESS):
                self.active_camera.ProcessKeyboard(1, self.deltaTime)
            if (get_key(window, KEY_A) == PRESS):
                self.active_camera.ProcessKeyboard(2, self.deltaTime)
            if (get_key(window, KEY_D) == PRESS):
                self.active_camera.ProcessKeyboard(3, self.deltaTime)
            if (get_key(window, KEY_F) == PRESS):
                self.active_camera.model_rotation += 1
            if (get_key(window, KEY_V) == PRESS):
                self.active_camera.model_rotation -= 1
                

    def onMouseMoved(self, xpos, ypos):
        if (not self.show_console):
            xoffset = xpos - self.lastX
            yoffset = self.lastY - ypos #reversed since y-coordinates go from bottom to top

            self.lastX = xpos
            self.lastY = ypos
            self.active_camera.ProcessMouseMovement(xoffset, yoffset, True)

    def onMouseScrolled(self, xpos, ypos):
        self.player.model_height -= ypos

    def onMouseClicked(self, button, action, mods):
        print (button, action)
        if (button == 0 and action == 1):
            print ("got here")
            m = translate(self.player.model, vec3(0, self.player.model_height, 0))
            m = rotate(m, math.radians(self.player.model_rotation), vec3(0, 1, 0))
            self.player.model_transforms.append(m)
        if (button == 2 and action == 1):
            set_cursor_visible(self.window, False)
        if (button == 2 and action == 0):
            self.lastX = WIDTH/2
            self.lastY = HEIGHT/2
            set_cursor_visible(self.window, True) 
        if (button == 1 and action == 1):
            verts = self.testModel.meshes[0].vertices
            tex_position = self.player.texture_coords + vec3(-1, 0.0, 1)
            #speed ups to be had with numpy
            #verts = filter(lambda v: Vertex(v.Position.x, v.Position.y+0.02, v.Position.z) if length(tex_position - v.Position) < 0.3 else v, verts)
            #print (len(list(verts)))
            #verts2 = list(verts)[:]
            radius =  0.3
            radius_2 =  radius*radius
            for v in verts:
                if (v.Position.x < tex_position.x - radius):
                    continue
                elif (v.Position.x > tex_position.x + radius):
                    continue
                elif (v.Position.y < tex_position.y - radius):
                    continue
                elif (v.Position.y > tex_position.y + radius):
                    continue
                elif ((tex_position.x - v.Position.x)**2 + (tex_position.z - v.Position.z)**2 < radius_2):
                    v.Position.y += 0.02
            #print (verts)
            #print (verts2)

            self.testModel.meshes[0].vertices = verts
            self.testModel.meshes[0].setupMesh()

    def onWindowResized(self, width, height):
        pass

    def onKeyPressed(self, key, scancode, action, mods):
        print (scancode)
        if (self.show_console == True and action == 1):
            try:
                ch = chr(ascii_map[scancode])
                if (mods == MOD_SHIFT):
                    ch = ch.capitalize()

                self.cmd += ch
            except(KeyError):
                pass

        if (self.show_console == True and key == KEY_BACKSPACE and action == 1):
            self.cmd = self.cmd[:len(self.cmd)-1]

        if (self.show_console == True and key == KEY_ENTER and action == 1):
            if (self.cmd == "reset"):
                self.player.entity.position = vec3(0.0, 4.0, 0.0)

            elif (self.cmd == "quit"):
                exit()

            elif (self.cmd == "clearcolor"):
                set_clear_color(random.uniform(0.0,1.0), random.uniform(0.0,1.0),random.uniform(0.0,1.0))

            elif (self.cmd == "toggle debugmode"):
                if self.debug_mode:
                    self.debug_mode = False
                else:
                    self.debug_mode = True
            else:
                output = run_command(self.cmd)
                print (str(output))
                self.cmd = str(output)


        if (key == KEY_GRAVE_ACCENT and action == 1):
            if self.show_console == True:
                self.show_console = False
                self.cmd = ""
            else:
                self.show_console = True
        if (key == KEY_ESCAPE and action == 1):
            exit()
        if (not self.show_console):
            if (key == KEY_SPACE and action == 1):
                self.player.jump(self.deltaTime)
            if (key == KEY_C and action == 1):
                if (self.player.cameraType == "FirstPersonCamera"):
                    #glfw.set_input_mode(self.window, glfw.CURSOR, glfw.CURSOR_DISABLED)
                    self.player.cameraType = "ThirdPersonCamera"

                elif (self.player.cameraType == "ThirdPersonCamera"):
                    #glfw.set_input_mode(self.window, glfw.CURSOR, glfw.CURSOR_DISABLED)
                    self.player.cameraType = "FirstPersonCamera"

if __name__ == "__main__":
    app = App("test", WIDTH, HEIGHT, False)
    app.init("test", WIDTH, HEIGHT, False)
    run(app)


