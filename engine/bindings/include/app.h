
#include "engine.h"

#include <GLFW/glfw3.h>

#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/detail/operator_id.hpp>
#include <boost/make_shared.hpp>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

using namespace boost;
using namespace boost::python;

struct ApplicationWrap : Application, boost::python::wrapper<Application>
{
  ApplicationWrap();
  ApplicationWrap(std::string title, unsigned int WIDTH, unsigned int HEIGHT, bool fullscreen);
  int setup(std::string title, unsigned int WIDTH, unsigned int HEIGHT, bool fullscreen);

  void update();
  void onKeyPressed(int key, int scancode, int action, int mods);
  void onCharPressed(unsigned int _char);
  void onMouseClicked(int button, int action, int mods);
  void onWindowResized(int width, int height);
  void onMouseMoved(double xpos, double ypos);
  void onJoystickMoved(int jid, int event);
  void onMouseScrolled(double xpos, double ypos);
  void gameLoop();
  void draw();

  float getFPS();
};
void wrap_Application();
void run(python::object obj);
