#include "_engine.h"

#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/detail/operator_id.hpp>
#include <boost/make_shared.hpp>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

using namespace boost;
using namespace boost::python;

struct CameraWrap : Camera, boost::python::wrapper<Camera>
{
  CameraWrap(glm::vec3 position, glm::vec3 front, glm::vec3 up, float yaw, float pitch);
  int _setup(glm::vec3 position, glm::vec3 front, glm::vec3 up, float yaw, float pitch);
  python::list _GetViewMatrix();
  glm::vec3 getPosition() const;

  void setPosition(glm::vec3 pos);

  glm::vec3 getFront() const;

  void setFront(glm::vec3 front);

  int ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch);

  int ProcessKeyboard(int direction, float deltaTime);

};



void wrap_Camera();