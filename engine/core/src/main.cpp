#include "app.h"
#include "engine.h"
#include "main.h"

// #include <boost/python.hpp>
// #include <iostream>
// using namespace std;
// namespace python = boost::python;
// using namespace boost;
// using namespace boost::python;


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
static void onCharPressed(GLFWwindow* window, unsigned int _char)
{
    app->onCharPressed(_char);
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

// std::string runPythonCommand(std::string cmd) {
//   try
//   {
//     Py_Initialize();
//     PyObject *pModule = PyImport_AddModule("__main__"); //create main module
//     std::string stdOutErr = "import sys\nclass CatchOutErr:\n\tdef __init__(self):\n\t\tsys.stdout.write('\\r')\n\t\tself.value = ''\n\tdef write(self, txt):\n\t\tself.value += txt\ncatchOutErr = CatchOutErr()\nsys.stdout = catchOutErr\nsys.stderr = catchOutErr\n";

//     PyRun_SimpleString(stdOutErr.c_str()); //invoke code to redirect
//     std::cout << "start command..." << std::endl;
//     PyRun_SimpleString(cmd.c_str());
//     std::cout << "finish command." << std::endl;
//     PyObject *catcher = PyObject_GetAttrString(pModule, "catchOutErr"); //get our catchOutErr created above
//     PyObject *output = PyObject_GetAttrString(catcher,"value"); //get the stdout and stderr from our catchOutErr object
//     PyObject *encodedData = PyUnicode_AsEncodedString(output, "ascii", NULL); //it's not in our C++ portion
//     char* buf;
//     Py_ssize_t len;
//     PyBytes_AsStringAndSize(encodedData, &buf, &len);
//     std::cout << std::string(buf) << std::endl;
//     Py_DECREF(output);
//     Py_DECREF(encodedData);
//     return std::string(buf);
//   }
//   catch (const python::error_already_set&)
//   {
//     PyObject *ptype, *pvalue, *ptraceback;
//     PyErr_Fetch(&ptype, &pvalue, &ptraceback);
//     std::string strErrorMessage = extract<string>(pvalue);
//     return strErrorMessage;
//     PyErr_Print();
//   }
//   return std::string("");
// }


int main(int argc, char** argv)
{
    #ifdef USE_CPP
        app = getApplication();
        glfwSetJoystickCallback(joystick_callback);
        glfwSetKeyCallback(app->window, onKeyPressed);
        glfwSetCharCallback(app->window, onCharPressed);
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


    // std::ifstream file;
    // std::stringstream fileStream;
    // file.open(argv[1]);
    // fileStream << file.rdbuf();

    // runPythonCommand(fileStream.str());

}
