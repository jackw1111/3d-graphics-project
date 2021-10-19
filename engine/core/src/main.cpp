#include "app.h"
#include "_engine.h"

extern unsigned int WIDTH;
extern unsigned int HEIGHT;

// check if building for python or cpp application
#ifndef USE_CPP
extern Application *app; // exported from python file
#endif
#ifdef USE_CPP
Application *app;
extern Application* getApplication();
#endif

static void onMouseClicked(GLFWwindow* window, int button, int action, int mods)
{
    app->onMouseClicked(button, action, mods);
}
static void onKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    app->onKeyPressed(key, scancode, action, mods);
}
static void onWindowResized(GLFWwindow* window, int width, int height)
{
    glViewport(0,0, width, height);
    app->onWindowResized(width, height);
}
static void onMouseMoved(GLFWwindow* window, double xpos, double ypos)
{
    app->onMouseMoved(xpos, ypos);
}

static void onWindowClose(GLFWwindow* window) {
    app->onWindowClose(window);
}

void joystick_callback(int jid, int event)
{
    if (event == GLFW_CONNECTED)
    {
        std::cout <<" joystick connected" << std::endl;
        // The joystick was connected
    }
    else if (event == GLFW_DISCONNECTED)
    {
        // The joystick was disconnected
        std::cout <<" joystick disconnected" << std::endl;
    }
}


int main(int argc, char** argv)
{
    #ifdef USE_CPP
        app = getApplication();
        glfwSetJoystickCallback(joystick_callback);
        glfwSetKeyCallback(app->window, onKeyPressed);
        glfwSetCursorPosCallback(app->window, onMouseMoved);
        glfwSetMouseButtonCallback(app->window, onMouseClicked);
        glfwSetFramebufferSizeCallback(app->window, onWindowResized);
        glfwSetWindowCloseCallback(app->window, onWindowClose);
        while(!glfwWindowShouldClose(app->window))
        {
            app->gameLoop();
            glfwSwapBuffers(app->window);
            glFinish();
            glfwPollEvents();    
        }
        glfwDestroyWindow(app->window);
        glfwTerminate();
    #endif   

}
