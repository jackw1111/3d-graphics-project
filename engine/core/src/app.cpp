#include "app.h"
#include <chrono>

std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();


/*
 * Application uses an unorthodox pattern where each callback has two functions
 * an internal implementation that simply calls a purely abstact function of the same callback.
 * Then in the inherited class, the abstract function is overridden for the specific needs of the inheritor.
 * Notice also that in main.cpp, static functions call the internal class, which is abit of a hack
 * but it had proven to be difficult to call these static GLFW window callbacks another way.
 * It's abit overkill, but it works.
 */

// set default app size; is overwritten from Python call anyway
unsigned int WIDTH = 800;
unsigned int HEIGHT = 600;

#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/detail/operator_id.hpp>
#include <boost/make_shared.hpp>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

using namespace boost;
using namespace boost::python;


#include <sstream>
#include <vector>
#include <iomanip>

#include "/home/me/Documents/3d-graphics-project/engine/core/include/_engine.h"

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

Application::Application(std::string title, unsigned int _WIDTH, unsigned int _HEIGHT, bool fullscreen) {

    WIDTH = _WIDTH;
    HEIGHT = _HEIGHT;

    glfwInit();
    std::cout << "initialized glfw" << std::endl;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE );

    //glfwWindowHint(GLFW_SAMPLES, 4);
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
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    } 
    
    if (!GLAD_GL_ARB_arrays_of_arrays) {
        std::cout << "GL_ARB_arrays_of_arrays not supported!" << std::endl; 
    }

    setup(title, WIDTH, HEIGHT, fullscreen);

}

void Application::setBackgroundColor(vec3 color) {
    backgroundColor = color;
    glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0);
}

void Application::setFarPlane(float _farPlane) {
    farPlane = _farPlane;
    active_camera.projection_matrix = perspective(45.0f, float(WIDTH)/float(HEIGHT), 0.01f, farPlane);
}


// Use in conjunction with Application() to create an instance but do not create a new window
// useful for drawing to different contexts/windows of OpenGL

int Application::setup(std::string title, unsigned int WIDTH, unsigned int HEIGHT, bool fullscreen) {

    lastX = WIDTH / 2.0f;
    lastY = HEIGHT / 2.0f;
    std::cout << "constructor in C++" << std::endl;

    //glEnable(GL_MULTISAMPLE);
    //glEnable(GL_LINE_SMOOTH);
    //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glCullFace(GL_BACK);

    setBackgroundColor(vec3(0.0f, 0.0f, 0.0f));
    active_camera._setup(vec3(0.0, 0.0, 10.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0), -90.0f, 0.0f);
    active_camera.projection_matrix = perspective(45.0f, float(WIDTH)/float(HEIGHT), 0.01f, farPlane);

    sky_box = Skybox();
    sky_box_shader.setup("/home/me/Documents/3d-graphics-project/shaders/skybox_shader.vs","/home/me/Documents/3d-graphics-project/shaders/skybox_shader.fs");
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

    simpleDepthShader = new AnimatedShader("/home/me/Documents/3d-graphics-project/shaders/shadow_mapping_depth.vs", "/home/me/Documents/3d-graphics-project/shaders/shadow_mapping_depth.fs");
    debugDepthQuad = new StaticShader("/home/me/Documents/3d-graphics-project/shaders/debug_quad.vs", "/home/me/Documents/3d-graphics-project/shaders/debug_quad_depth.fs");



    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture

    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);  
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return 0;
}

void Application::drawScene(int shadowPass) {
    if (Light::lights.size() > 0) {

        Light *light = Light::lights.at(0);
        vec3 frustumCenter = (frustum.Cfar - frustum.Cnear)*0.25f;

        lightView = glm::lookAt(normalize(light->position), vec3(0,0,0), vec3(0,0,1));

        glm::vec3 _bottomRightNear = frustum.bottomRightNear;
        glm::vec3 _topRightNear = frustum.topRightNear;
        glm::vec3 _bottomLeftNear = frustum.bottomLeftNear;
        glm::vec3 _topLeftNear = frustum.topLeftNear;

        glm::vec3 _bottomRightFar = frustum.bottomRightFar;
        glm::vec3 _topRightFar = frustum.topRightFar;
        glm::vec3 _bottomLeftFar = frustum.bottomLeftFar;
        glm::vec3 _topLeftFar = frustum.topLeftFar;

        std::array<glm::vec3, 8> frustumToLightView
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

        if (StaticObject::modelRegistry.size() > 0) {

            // frustum cull objects
            for (unsigned int i = 0; i < StaticObject::modelRegistry.size(); i++) {
                for (unsigned int j = 0; j < StaticObject::modelRegistry[i].size(); j++) {
                    frustum.cullStaticObject(StaticObject::modelRegistry[i][j]);
                }
            }
            StaticModel::shader.use();
            StaticModel::shader.setMat4("proj_view", proj_view);
            StaticModel::shader.setVec3("lightPos", Light::lights.at(0)->position); 
            StaticModel::shader.setVec3("viewPos", active_camera.Position); 
            StaticModel::shader.setMat4("lightSpaceMatrix", lightSpaceMatrix); 


            glActiveTexture(GL_TEXTURE15);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            StaticModel::shader.setInt("depthMap", 15); 


            StaticObject::drawAllObjects(active_camera, StaticModel::shader); 
        }     

        if (AnimatedObject::modelRegistry.size() > 0) {

            // frustum cull objects
            for (unsigned int i = 0; i < AnimatedObject::modelRegistry.size(); i++) {
                for (unsigned int j = 0; j < AnimatedObject::modelRegistry[i].size(); j++) {
                    frustum.cullAnimatedObject(AnimatedObject::modelRegistry[i][j]);
                }
            }

            AnimatedModel::shader.use();
            AnimatedModel::shader.setMat4("proj_view", proj_view);            
            AnimatedModel::shader.setVec3("lightPos", Light::lights.at(0)->position); 
            AnimatedModel::shader.setInt("shadowPass", shadowPass); 

            AnimatedObject::drawAllObjects(active_camera, currentFrame, shadowPass); 
           
        }

        if (sky_box.loadSkybox) {
            // draw skybox
            sky_box_shader.use();
            mat4 sky_view = active_camera.GetViewMatrix(); 
            // remove translation from the view matrix
            sky_view[3] = vec4(0,0,0,0);
            sky_box_shader.setMat4("projection", active_camera.projection_matrix);
            sky_box_shader.setMat4("view", sky_view);
            sky_box_shader.setMat4("model", mat4(1.0));
            sky_box.Draw(sky_box_shader);      
        }     


        // draw all lines
        for (unsigned int i = 0; i < Line3D::lines.size(); i++) {
            Line3D *currentLine = Line3D::lines.at(i);
            currentLine->setCamera(proj_view);
            currentLine->draw();
        }        
    } else {
        // TO DO cull from light position (implement frustum cull for ortho)
        for (unsigned int i = 0; i < AnimatedObject::modelRegistry.size(); i++) {
            for (unsigned int j = 0; j < AnimatedObject::modelRegistry[i].size(); j++) {
                frustum.reset(AnimatedObject::modelRegistry[i][j]);
            }
        }
        // render scene from light's point of view
        AnimatedModel::shader.use();
        AnimatedModel::shader.setInt("shadowPass", shadowPass); 
        AnimatedModel::shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        AnimatedObject::drawAllObjects(active_camera, currentFrame, shadowPass);     
    }
    
}

void Application::drawUI() {

    // drawUI
    std::sort( Rect::rects.begin( ), Rect::rects.end( ), [ ]( Rect *lhs, Rect *rhs )
    {
       return lhs->ordering < rhs->ordering;
    });

    for (unsigned int i = 0; i < Rect::rects.size(); i++) {
        Rect *currentRect = Rect::rects.at(i);
        if (currentRect->toDraw == true) {
            currentRect->draw();
        }
    }

    // draw all labels
    for (unsigned int i = 0; i < Label::labels.size(); i++) {
        Label *currentLabel = Label::labels.at(i);
        if (currentLabel->toDraw == true) {
            currentLabel->draw();
        }
    }


}

void Application::update() {



}

int Application::gameLoop() {

    currentFrame = (float)getTime::now().time_since_epoch().count()/1000000000.0f; // convert nanoseconds to seconds

    deltaTime = (float)(currentFrame - lastFrame);
    lastFrame = currentFrame;

    update();

    // // update scene cameras view matrix (ie. GetViewMatrix())
    active_camera.view_matrix = lookAt(active_camera.Position, active_camera.Position + active_camera.Front, vec3(0,1,0));

    frustum = Frustum(active_camera.fov, 0.01f, 100.0f, WIDTH, HEIGHT, active_camera);

    // shadow pass TO DO save bones and culling
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
        glCullFace(GL_FRONT);
        glEnable(GL_DEPTH_CLAMP);
        drawScene(1);
        glFlush();
        glCullFace(GL_BACK);
        glDisable(GL_DEPTH_CLAMP);
        glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // draw scene
    glViewport(0, 0, WIDTH, HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawScene(0);
        drawUI();
        glFlush();


    // if (debug) {
    //     std::cout << "debug!"<< std::endl;
    //     debugDepthQuad->use();
    //     debugDepthQuad->setFloat("near_plane", 0.1f);
    //     debugDepthQuad->setFloat("far_plane", farPlane);
    //     glActiveTexture(GL_TEXTURE0);
    //     glBindTexture(GL_TEXTURE_2D, depthMap);
    //     renderQuad();        
    // }

    return 1;
}

Application::~Application() {

}

void Application::onKeyPressed(int key, int scancode, int action, int mods){

    // for (unsigned int i = 0; i < StaticModel::models.size(); i++) {
    //     StaticModel *statModel = StaticModel::models.at(i);
    //     statModel->onKeyPressed(key, scancode, action, mods);
    // }
};
