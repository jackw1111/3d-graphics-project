/*
 * Application uses an unorthodox pattern where each callback has two functions
 * an internal implementation that simply calls a purely abstact function of the same callback.
 * Then in the inherited class, the abstract function is overridden for the specific needs of the inheritor.
 * Notice also that in main.cpp, static functions call the internal class, which is abit of a hack
 * but it had proven to be difficult to call these static GLFW window callbacks another way.
 * It's abit overkill, but it works.
 */

#include "app.h"
#include <chrono>

#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sstream>
#include <vector>
#include <iomanip>
#include <set>

#include "engine.h"

std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();


// set default app size; is overwritten from Python call anyway
unsigned int WIDTH = 800;
unsigned int HEIGHT = 600;


// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

#define TEST_ERROR(_msg)        \
ALCenum error = alGetError();       \
if (error != AL_NO_ERROR) { \
    fprintf(stderr, _msg "\n"); \
}

struct getTime
{
    using duration   = std::chrono::steady_clock::duration;
    using rep        = duration::rep;
    using period     = duration::period;
    using time_point = std::chrono::time_point<getTime>;
    static constexpr bool is_steady = true;

    static time_point now() noexcept
    {
        using namespace std::chrono;
        static auto epoch = steady_clock::now();
        return time_point{steady_clock::now() - epoch};
    }
};


Scene::Scene() {

}

Application::Application() {

};


float Application::getFPS() {
    return 1.0f/(deltaTime + 0.0001);
}

Application::Application(std::string title, unsigned int _WIDTH, unsigned int _HEIGHT, bool fullscreen, bool _MSAA) {

    WIDTH = _WIDTH;
    HEIGHT = _HEIGHT;

    MSAA = _MSAA;

    glfwInit();
    std::cout << "initialized glfw" << std::endl;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    if (glfwGetCurrentContext() == NULL) {

        if (!MSAA) {
            glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE );
        }
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        if (MSAA) {
            glfwWindowHint(GLFW_SAMPLES, 4);
        }
        if (!fullscreen) {
            window = glfwCreateWindow(WIDTH, HEIGHT, title.c_str(), NULL, NULL);
            std::cout << window << std::endl;
            assert(window != NULL);
        } else {
            window = glfwCreateWindow(WIDTH, HEIGHT, title.c_str(), glfwGetPrimaryMonitor(), NULL);
            std::cout << window << std::endl;
            assert(window != NULL);

        }

        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
        }
        std::cout << "got here1" << std::endl;

        glfwMakeContextCurrent(window);
        std::cout << "got here2" << std::endl;

        if (!MSAA) {
            glfwSwapInterval(0);
        }
        #ifdef GAME_TOOL
        //
        #else
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
        } 
        #endif
        GLint no_of_extensions = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &no_of_extensions);

        std::set<std::string> ogl_extensions;
        for (int i = 0; i < no_of_extensions; i++) {
            ogl_extensions.insert((const char*)glGetStringi(GL_EXTENSIONS, i));
        }
        bool texture_storage = 
            ogl_extensions.find("GL_ARB_arrays_of_arrays") != ogl_extensions.end();
        if (texture_storage) {
             std::cout << "GL_ARB_arrays_of_arrays not supported!" << std::endl; 
        }

        texture_storage = 
            ogl_extensions.find("GL_ARB_shader_storage_buffer_object") != ogl_extensions.end();
        if (texture_storage) {
             std::cout << "GL_ARB_shader_storage_buffer_object not supported!" << std::endl; 
        }

        texture_storage = 
            ogl_extensions.find("GL_EXT_multi_draw_arrays") != ogl_extensions.end();
        if (texture_storage) {
             std::cout << "GL_EXT_multi_draw_arrays not supported!" << std::endl; 
        }

        texture_storage = 
            ogl_extensions.find("GL_ARB_multi_draw_indirect") != ogl_extensions.end();
        if (texture_storage) {
             std::cout << "GL_ARB_multi_draw_indirect not supported!" << std::endl; 
        }   
        texture_storage = 
            ogl_extensions.find("GL_OES_depth_texture extension") != ogl_extensions.end();
        if (texture_storage) {
             std::cout << "GL_OES_depth_texture extension not supported!" << std::endl; 
        }   
        // if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        // {
        //     std::cout << "Failed to initialize GLAD" << std::endl;
        // } 
        std::cout << glGetString ( GL_SHADING_LANGUAGE_VERSION ) << std::endl;
        std::cout << "got here2" << std::endl;

        // if (!GLAD_GL_ARB_arrays_of_arrays) {
        //     std::cout << "GL_ARB_arrays_of_arrays not supported!" << std::endl; 
        // }
        // if (!GLAD_GL_ARB_shader_storage_buffer_object) {
        //     std::cout << "GL_ARB_shader_storage_buffer_object not supported!" << std::endl; 
        // }
        // if (!GLAD_GL_EXT_multi_draw_arrays) {
        //     std::cout << "GL_EXT_multi_draw_arrays not supported!" << std::endl; 
        // }
        // if (!GLAD_GL_ARB_multi_draw_indirect) {
        //     std::cout << "GLAD_GL_ARB_multi_draw_indirect not supported!" << std::endl; 
        // }
        // std::cout << "got here3" << std::endl;
    }
    
    setup(title, WIDTH, HEIGHT, fullscreen, _MSAA);

}

void Application::setBackgroundColor(vec3 color) {
    backgroundColor = color;
    glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0);
}

void Application::createShadowFBO() {
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture

    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    //glTexStorage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapResolution, shadowMapResolution);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, shadowMapResolution, shadowMapResolution, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    const GLenum attachments[] = {GL_NONE};
    glDrawBuffers(1, attachments);    // OK
    //glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


// Use in conjunction with Application() to create an instance but do not create a new window
// useful for drawing to different contexts/windows of OpenGL

int Application::setup(std::string title, unsigned int WIDTH, unsigned int HEIGHT, bool fullscreen, bool _MSAA) {
    lastX = WIDTH / 2.0f;
    lastY = HEIGHT / 2.0f;
    std::cout << "constructor in C++" << std::endl;
    MSAA = _MSAA;

    if (MSAA){
        glEnable(GL_MULTISAMPLE);
    }

    // glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // glEnable(GL_LINE_SMOOTH);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

    glCullFace(GL_BACK);

    setBackgroundColor(vec3(0.0f, 0.0f, 0.0f));
    active_camera._setup(vec3(0.0, 0.0, 10.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0), -90.0f, 0.0f);
    active_camera.projection_matrix = perspective(45.0f, float(WIDTH)/float(HEIGHT), active_camera.nearPlane, active_camera.farPlane);

    sky_box = Skybox();

    const char* skybox_vShader =
    "#version 300 es\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aNormal;\n"
    "layout (location = 2) in vec3 aTexCoords;\n"
    "out vec3 TexCoords;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "    TexCoords = aPos;\n"
    "    vec4 pos = projection * view * model * vec4(aPos, 1.0);\n"
    "    gl_Position = pos.xyww;\n"
    "}\n";  

    const char* skybox_fShader =
    "#version 300 es\n"
    "precision mediump float;\n"
    "out vec4 FragColor;\n"
    "in vec3 TexCoords;\n"
    "uniform samplerCube skybox;\n"
    "void main()\n"
    "{    \n"
    "    FragColor = texture(skybox, TexCoords);\n"
    "}\n";

    sky_box_shader.setup(skybox_vShader, skybox_fShader);
    std::cout << "skybox loading finished." << std::endl;

    std::vector<std::string> faces = {
        "./data/skybox/right.jpg",
        "./data/skybox/left.jpg",
        "./data/skybox/top.jpg",
        "./data/skybox/bottom.jpg",
        "./data/skybox/front.jpg",
        "./data/skybox/back.jpg"
    };
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    sky_box.load(faces);

    std::cout << "constructor finished." << std::endl;
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time difference (sec) = " <<  (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) /1000000.0  <<std::endl;

    createShadowFBO();

    // dither map
    //ditherMap = TextureFromFile("dither.png", "/home/me/Documents/3d-graphics-project/data");

    lastFrame = 0.0f;
    
    return 0;
}

void Application::drawScene(int shadowPass) {


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

    if (Light::lights.size() > 0) {

        Light *light = Light::lights.at(0);

        lightView = glm::lookAt(normalize(light->position), vec3(0,0,0), vec3(0,0,1));

        glm::vec3 _bottomRightNear = frustum.bottomRightNear;
        glm::vec3 _topRightNear = frustum.topRightNear;
        glm::vec3 _bottomLeftNear = frustum.bottomLeftNear;
        glm::vec3 _topLeftNear = frustum.topLeftNear;

        glm::vec3 _bottomRightFar = frustum.bottomRightFar;
        glm::vec3 _topRightFar = frustum.topRightFar;
        glm::vec3 _bottomLeftFar = frustum.bottomLeftFar;
        glm::vec3 _topLeftFar = frustum.topLeftFar;

        std::array<glm::vec3, 8> frustumToLightView =
        {
            lightView * glm::vec4(_bottomRightNear, 1.0f),
            lightView * glm::vec4(_topRightNear, 1.0f),
            lightView * glm::vec4(_bottomLeftNear, 1.0f),
            lightView * glm::vec4(_topLeftNear, 1.0f),
            lightView * glm::vec4(_bottomRightFar, 1.0f),
            lightView * glm::vec4(_topRightFar, 1.0f),
            lightView * glm::vec4(_bottomLeftFar, 1.0f),
            lightView * glm::vec4(_topLeftFar, 1.0f)
        };

        glm::vec3 min{ INFINITY, INFINITY, INFINITY };
        glm::vec3 max{ -INFINITY, -INFINITY, -INFINITY };
        for (unsigned int i = 0; i < frustumToLightView.size(); i++)
        {
            if (frustumToLightView[i].x < min.x)
                min.x = frustumToLightView[i].x;
            if (frustumToLightView[i].y < min.y)
                min.y = frustumToLightView[i].y;
            if (frustumToLightView[i].z < min.z)
                min.z = frustumToLightView[i].z;
     
            if (frustumToLightView[i].x > max.x)
                max.x = frustumToLightView[i].x;
            if (frustumToLightView[i].y > max.y)
                max.y = frustumToLightView[i].y;
            if (frustumToLightView[i].z > max.z)
                max.z = frustumToLightView[i].z;
        }

        float l = min.x;
        float r = max.x;
        float b = min.y;
        float t = max.y;
        // because max.z is positive and in NDC the positive z axis is towards us so need to set it as the near plane flipped same for min.z.
        float n = -max.z;
        float f = -min.z;

        lightProjection = glm::ortho(l,r,b,t,n,f);
        lightSpaceMatrix = lightProjection * lightView;
    }
    mat4 proj_view = active_camera.projection_matrix * active_camera.view_matrix;

    if (!shadowPass) {    

        if (sky_box.loadSkybox) {
            // draw skybox
            sky_box_shader.use();
            mat4 sky_view = active_camera.view_matrix;
            // remove translation from the view matrix
            sky_view[3] = vec4(0,0,0,0);
            sky_box_shader.setMat4("projection", active_camera.projection_matrix);
            sky_box_shader.setMat4("view", sky_view);
            sky_box_shader.setMat4("model", mat4(1.0));
            sky_box.Draw(sky_box_shader);      
        }   
        /*Particle::setCamera(proj_view);
        Particle::drawAllParticles(active_camera, 0);     
        */ 
        if (StaticObject::modelRegistry.size() > 0) {

            if (frustumCullingEnabled) {
                // frustum cull objects
                for (unsigned int i = 0; i < StaticObject::modelRegistry.size(); i++) {
                    for (unsigned int j = 0; j < StaticObject::modelRegistry[i].size(); j++) {
                        frustum.cullStaticObject(StaticObject::modelRegistry[i][j]);
                    }
                }
            }
            StaticModel::shader.use();
            StaticModel::shader.setMat4("proj_view", proj_view);
            StaticModel::shader.setVec3("lightPos", Light::lights.at(0)->position); 
            StaticModel::shader.setVec3("viewPos", active_camera.Position); 
            StaticModel::shader.setVec3("backgroundColor", backgroundColor); 
            StaticModel::shader.setMat4("lightSpaceMatrix", lightSpaceMatrix); 
            StaticModel::shader.setFloat("nearPlane", active_camera.nearPlane); 
            StaticModel::shader.setFloat("farPlane", active_camera.farPlane); 

            glActiveTexture(GL_TEXTURE13);
            glBindTexture(GL_TEXTURE_CUBE_MAP, sky_box.cubemapTexture);
            StaticModel::shader.setInt("skybox", 13); 


            //glActiveTexture(GL_TEXTURE14);
            //glBindTexture(GL_TEXTURE_2D, ditherMap);
            //StaticModel::shader.setInt("ditherMap", 14); 

            glActiveTexture(GL_TEXTURE15);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            StaticModel::shader.setInt("depthMap", 15); 

            StaticObject::drawAllObjects(active_camera, StaticModel::shader); 
        }     

        if (AnimatedObject::modelRegistry.size() > 0) {

            if (frustumCullingEnabled) {
                // frustum cull objects
                for (unsigned int i = 0; i < AnimatedObject::modelRegistry.size(); i++) {
                    for (unsigned int j = 0; j < AnimatedObject::modelRegistry[i].size(); j++) {
                        frustum.cullAnimatedObject(AnimatedObject::modelRegistry[i][j]);
                    }
                }
            }

            AnimatedModel::shader.use();
            AnimatedModel::shader.setMat4("proj_view", proj_view);            
            AnimatedModel::shader.setVec3("lightPos", Light::lights.at(0)->position); 
            AnimatedModel::shader.setVec3("viewPos", active_camera.Position); 
            AnimatedModel::shader.setInt("shadowPass", shadowPass); 
            AnimatedModel::shader.setVec3("backgroundColor", backgroundColor); 
            AnimatedModel::shader.setFloat("nearPlane", active_camera.nearPlane); 
            AnimatedModel::shader.setFloat("farPlane", active_camera.farPlane); 

            glActiveTexture(GL_TEXTURE13);
            glBindTexture(GL_TEXTURE_CUBE_MAP, sky_box.cubemapTexture);
            AnimatedModel::shader.setInt("skybox", 13); 
            
            AnimatedObject::drawAllObjects(active_camera, currentFrame, shadowPass); 
           
        }

        Line3D::setCamera(proj_view);
        Line3D::drawAllLines();

        for (unsigned int i = 0; i < Line2D::lines.size(); i++) {
            Line2D *l = Line2D::lines.at(i);
            l->draw();
        }


        Particle::setCamera(proj_view);
        Particle::drawAllParticles(active_camera, 1);  

        
    } else {
        // TO DO cull from light position (implement frustum cull for ortho)
        for (unsigned int i = 0; i < AnimatedObject::modelRegistry.size(); i++) {
            for (unsigned int j = 0; j < AnimatedObject::modelRegistry[i].size(); j++) {
                frustum.reset(AnimatedObject::modelRegistry[i][j]);
            }
        }

        if (AnimatedObject::modelRegistry.size() > 0) {

            // frustum cull objects
            for (unsigned int i = 0; i < AnimatedObject::modelRegistry.size(); i++) {
                for (unsigned int j = 0; j < AnimatedObject::modelRegistry[i].size(); j++) {
                    //frustum.cullAnimatedObject(AnimatedObject::modelRegistry[i][j]);
                }
            }

            // render scene from light's point of view
            AnimatedModel::shader.use();
            AnimatedModel::shader.setInt("shadowPass", shadowPass); 
            AnimatedModel::shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

            AnimatedObject::drawAllObjects(active_camera, currentFrame, shadowPass);     
        }
    }
    
    
}

unsigned int Application::getShadowMapResolution() {
    return shadowMapResolution;
}

void Application::setShadowMapResolution(unsigned int r) {
    shadowMapResolution = r;
    createShadowFBO();
}

void Application::drawUI() {
  
    // drawUI
    glm::mat4 ortho_proj = glm::ortho(0.0f, (float)WIDTH, 0.0f, (float)HEIGHT, -1.0f, 1.0f);  
    Rect2D::setCamera(ortho_proj);
    Rect2D::drawAllRects(active_camera, 1); 


    glDisable(GL_BLEND);

    // draw all labels
    for (unsigned int i = 0; i < Label2D::labels.size(); i++) {
        Label2D *currentLabel = Label2D::labels.at(i);
        if (currentLabel->toDraw == true) {
            currentLabel->draw();
        }
    }
    mat4 proj_view = active_camera.projection_matrix * active_camera.view_matrix;

    // draw all label3d's
    for (unsigned int i = 0; i < Label3D::labels.size(); i++) {
        Label3D *currentLabel = Label3D::labels.at(i);
        if (currentLabel->toDraw == true) {
            currentLabel->shader.use();
            glUniformMatrix4fv(glGetUniformLocation(currentLabel->shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(proj_view));
            currentLabel->draw();
        }
    }

    glEnable(GL_BLEND);
  
}

void Application::update() {
    std::cout << "update from C++" << std::endl;
}

int Application::gameLoop() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    currentFrame = (float)getTime::now().time_since_epoch().count()/1000000000.0f; // convert nanoseconds to seconds
    if (lastFrame == 0.0f) {
        deltaTime = 0.0f;
    } else {
        deltaTime = (float)(currentFrame - lastFrame);
    }
    lastFrame = currentFrame;
    
    update();

    // // update scene cameras view matrix (ie. GetViewMatrix())
    if (!useCustomViewMatrix) {
        active_camera.view_matrix = lookAt(active_camera.Position, active_camera.Position + active_camera.Front, vec3(0,1,0));
    }
    frustum = Frustum(active_camera.fov, active_camera.nearPlane, active_camera.farPlane, WIDTH, HEIGHT, active_camera);

    // shadow pass
    glViewport(0, 0, shadowMapResolution, shadowMapResolution);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
        glCullFace(GL_FRONT);
        //glEnable(GL_DEPTH_CLAMP);
        drawScene(1);
        if (!MSAA) {
            glFlush();
        }
        glCullFace(GL_BACK);
        //glDisable(GL_DEPTH_CLAMP);
        glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // draw scene
    glViewport(0, 0, WIDTH, HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawScene(0);
        drawUI();
        if (!MSAA) {
            glFlush();
        }
 
    return 1;
}
int Application::draw() {

    return 1;
}

Application::~Application() {
    std::cout << "deleting app" << std::endl;
    // StaticObject::modelRegistry = {};
    // StaticObject::uniqueObjectCount = 0;
    // glfwTerminate();
}

void Application::onKeyPressed(int key, int scancode, int action, int mods){

    // for (unsigned int i = 0; i < StaticModel::models.size(); i++) {
    //     StaticModel *statModel = StaticModel::models.at(i);
    //     statModel->on_key_pressed(key, scancode, action, mods);
    // }
};

void Application::onCharPressed(unsigned int _char){

    // for (unsigned int i = 0; i < StaticModel::models.size(); i++) {
    //     StaticModel *statModel = StaticModel::models.at(i);
    //     statModel->on_key_pressed(key, scancode, action, mods);
    // }
}
