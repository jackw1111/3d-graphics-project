#include "../../bindings/include/app.h"



ApplicationWrap::ApplicationWrap() : Application() {};
ApplicationWrap::ApplicationWrap(std::string title, unsigned int WIDTH, unsigned int HEIGHT, bool fullscreen) : Application(title, WIDTH, HEIGHT, fullscreen) {}
int ApplicationWrap::setup(std::string title, unsigned int WIDTH, unsigned int HEIGHT, bool fullscreen) {
  return Application::setup(title, WIDTH, HEIGHT, fullscreen);
}


void ApplicationWrap::update()
{
    if (boost::python::override _update = this->get_override("update")) {

        // call default update method
        Application::update();

        // call python overriden update method first
        _update();


    } else {
        // no python method; just call default update method
        Application::update();
    }
}

void ApplicationWrap::onKeyPressed(int key, int scancode, int action, int mods) 
{
    if (boost::python::override f = this->get_override("on_key_pressed")) {
        f(key, scancode, action, mods);
        Application::onKeyPressed(key, scancode, action, mods);
    } else {
        Application::onKeyPressed(key, scancode, action, mods);
    }

}

void ApplicationWrap::onMouseClicked(int button, int action, int mods)
{
    if (boost::python::override f = this->get_override("on_mouse_clicked")) {
        f(button, action, mods);
        Application::onMouseClicked(button, action, mods);
    } else {
        Application::onMouseClicked(button, action, mods);
    }
}

void ApplicationWrap::onMouseScrolled(double xpos, double ypos)
{
    if (boost::python::override f = this->get_override("on_mouse_scrolled")) {
        f(xpos, ypos);
        Application::onMouseScrolled(xpos, ypos);
    } else {
        Application::onMouseScrolled(xpos, ypos);
    }
}

void ApplicationWrap::onWindowResized(int width, int height)
{
    if (boost::python::override f = this->get_override("on_window_resized")) {
        glViewport(0,0, width, height);
        f(width, height);
        Application::onWindowResized(width, height);
    } else {
        Application::onWindowResized(width, height);
    }
}

void ApplicationWrap::onMouseMoved(double xpos, double ypos)
{
    if (boost::python::override f = this->get_override("on_mouse_moved")) {
        f(xpos, ypos);
        Application::onMouseMoved(xpos, ypos);
    } else {
      
        Application::onMouseMoved(xpos, ypos);
    }
}

void ApplicationWrap::onJoystickMoved(int jid, int event)
{
    if (boost::python::override f = this->get_override("on_joystick_moved")) {
        f(jid, event);
        Application::onJoystickMoved(jid, event);
    } else {
        Application::onJoystickMoved(jid, event);
    }
}

float ApplicationWrap::getFPS() {
  return Application::getFPS();
}


void ApplicationWrap::gameLoop()
{
    Application::gameLoop();
}

void ApplicationWrap::draw()
{
    Application::draw();
}
BOOST_PYTHON_OPAQUE_SPECIALIZED_TYPE_ID(GLFWwindow);

void wrap_Application() {
    python::class_<ApplicationWrap, boost::noncopyable>("Application",python::init<std::string, unsigned int, unsigned int, bool>())
      .def(python::init<>())
      .def("setup", &ApplicationWrap::setup)
      .def_readwrite("lastX", &Application::lastX)
      .def_readwrite("lastY", &Application::lastY)
      .def_readwrite("deltaTime", &Application::deltaTime)
      .def_readwrite("currentFrame", &Application::currentFrame)
      .def_readwrite("lastFrame", &Application::lastFrame)
      .def_readwrite("active_camera", &Application::active_camera)
      .def_readwrite("sky_box", &Application::sky_box)
      .def_readwrite("shadow_map_center", &Application::shadowMapCenter)
      .def_readwrite("debug", &Application::debug)
      .def_readwrite("light_projection_matrix", &Application::lightProjection)
      .def_readwrite("light_view_matrix", &Application::lightView)
      .def_readwrite("use_custom_view_matrix", &Application::useCustomViewMatrix)

      .add_property("window", boost::python::make_function(&Application::getWindow, boost::python::return_value_policy<boost::python::return_opaque_pointer>()), &Application::setWindow)
      .def("update", &ApplicationWrap::update)
      .def("on_key_pressed", &ApplicationWrap::onKeyPressed)
      .def("on_mouse_clicked", &ApplicationWrap::onMouseClicked)
      .def("on_window_resized", &ApplicationWrap::onWindowResized)
      .def("on_mouse_scrolled", &ApplicationWrap::onMouseScrolled)
      .def("on_mouse_moved", &ApplicationWrap::onMouseMoved)
      .def("on_joystick_moved", &ApplicationWrap::onJoystickMoved)
      .def("gameLoop", &ApplicationWrap::gameLoop)
      .def("get_fps", &ApplicationWrap::getFPS)
      .def("set_background_color", &Application::setBackgroundColor)
      ;

      python::def("run", run);

}

ApplicationWrap *app;

static void onMouseClicked(GLFWwindow* window, int button, int action, int mods)
{
    app->onMouseClicked(button, action, mods);
}
static void onMouseScrolled(GLFWwindow* window, double xpos, double ypos)
{
    app->onMouseScrolled(xpos, ypos);
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

static void onJoystickMoved(int jid, int event)
{
    app->onJoystickMoved(jid, event);
}

void run(python::object obj) {
  app = python::extract<ApplicationWrap*>(obj);

  glfwSetCursorPosCallback(app->window, onMouseMoved);
  glfwSetJoystickCallback(onJoystickMoved);
  glfwSetMouseButtonCallback(app->window, onMouseClicked);
  glfwSetFramebufferSizeCallback(app->window, onWindowResized);
  glfwSetScrollCallback(app->window, onMouseScrolled);
  glfwSetKeyCallback(app->window, onKeyPressed);
  while(!glfwWindowShouldClose(app->window))
  {
      app->gameLoop();
      glfwSwapBuffers(app->window);
      glFinish();
      glfwPollEvents();    
  }
  glfwDestroyWindow(app->window);
  glfwTerminate();
}