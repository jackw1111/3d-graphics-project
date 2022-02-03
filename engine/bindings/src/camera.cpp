#include "../../bindings/include/camera.h"


CameraWrap::CameraWrap(glm::vec3 position, glm::vec3 front, glm::vec3 up, float yaw, float pitch) : Camera(position, front, up, yaw, pitch) {};

int CameraWrap::_setup(glm::vec3 position, glm::vec3 front, glm::vec3 up, float yaw, float pitch)
{
  return Camera::_setup(position, front, up, yaw, pitch);
}

glm::vec3 CameraWrap::getPosition() const
{
  return Camera::getPosition();
}

void CameraWrap::setPosition(glm::vec3 pos)
{
  return Camera::setPosition(pos);
}

glm::vec3 CameraWrap::getFront() const
{
  return Camera::getFront();
}

void CameraWrap::setFront(glm::vec3 front)
{
  return Camera::setFront(front);
}

int CameraWrap::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
  return Camera::ProcessMouseMovement(xoffset, yoffset, constrainPitch);
}

int CameraWrap::ProcessKeyboard(int direction, float deltaTime)
{
  return Camera::ProcessKeyboard(direction, deltaTime);
}

void wrap_Camera(){

  python::class_<CameraWrap, boost::noncopyable>("Camera", python::init<glm::vec3, glm::vec3, glm::vec3, float, float>())
    .def("_setup", &Camera::_setup)
    .def("ProcessMouseMovement", &Camera::ProcessMouseMovement)
    .def("GetViewMatrix", &Camera::GetViewMatrix)
    .def("ProcessKeyboard", &Camera::ProcessKeyboard)
    .def("updateCameraVectors", &Camera::updateCameraVectors)
    .def_readwrite("projection_matrix", &Camera::projection_matrix)
    .def_readwrite("view_matrix", &Camera::view_matrix)
    .add_property("front", &Camera::getFront, &Camera::setFront)
    .add_property("right", &Camera::getRight, &Camera::setFront)
    .def_readwrite("up", &Camera::Up)
    .add_property("position", &Camera::getPosition, &Camera::setPosition)
    .add_property("yaw", &Camera::getYaw, &Camera::setYaw)
    .add_property("pitch", &Camera::getPitch, &Camera::setPitch)
    .add_property("fov", &Camera::getFOV, &Camera::setFOV)
    .def_readwrite("MovementSpeed", &Camera::MovementSpeed)
    .def("set_far_plane", &Camera::setFarPlane)
    .def("set_near_plane", &Camera::setNearPlane)
    ;
}