#include "camera.h"

extern unsigned int WIDTH;
extern unsigned int HEIGHT;

Camera::Camera() {

}

// Constructor with vectors
Camera::Camera(vec3 position, vec3 front, vec3 up, float yaw, float pitch)
{
    Position = glm::vec3(0.0f, 0.0f, 0.0f);
    Front = glm::vec3(0.0f, 0.0f, -1.0f);
    WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    Yaw = YAW;
    Pitch = PITCH;
    MovementSpeed = SPEED;
    MouseSensitivity = SENSITIVITY;
    Zoom = ZOOM;
    updateCameraVectors();
    fov = 45.0f;
    projection_matrix = perspective(fov, float(WIDTH)/float(HEIGHT), 0.1f, 1000.0f);
    view_matrix = GetViewMatrix();

}
// Constructor with vectors
int Camera::_setup(vec3 position, vec3 front, vec3 up, float yaw, float pitch)
{
    Position = glm::vec3(0.0f, 0.0f, 0.0f);
    Front = glm::vec3(0.0f, 0.0f, -1.0f);
    WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    Yaw = YAW;
    Pitch = PITCH;
    MovementSpeed = SPEED;
    MouseSensitivity = SENSITIVITY;
    Zoom = ZOOM;
    fov = 45.0f;
    updateCameraVectors();
    projection_matrix = perspective(fov, float(WIDTH)/float(HEIGHT), 0.1f, 1000.0f);
    view_matrix = GetViewMatrix();

    return 1;
}
// Constructor with scalar values
Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
 {
    Position = vec3(posX, posY, posZ);
    WorldUp = vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
}

glm::vec3 Camera::getPosition() const {
  return Position;
}
void Camera::setPosition(glm::vec3 pos) {
  Position = pos;
  updateCameraVectors();
  //view_matrix = GetViewMatrix();

}

glm::vec3 Camera::getFront() const {
  return Front;
}

void Camera::setFront(glm::vec3 front) {
  Front = front;
  updateCameraVectors();
  //view_matrix = GetViewMatrix();

}

glm::vec3 Camera::getRight() {
  return Right;
}

int Camera::setRight(glm::vec3 right) {
  Right = right;
  updateCameraVectors();
  //view_matrix = GetViewMatrix();

  return 1;
}

float Camera::getMovementSpeed() { return MovementSpeed; };
void Camera::setMovementSpeed(float speed) { MovementSpeed = speed; };
float Camera::getPitch() { return Pitch; };
void Camera::setPitch(float pitch ) {
  Pitch = pitch;
  updateCameraVectors();
  //view_matrix = GetViewMatrix();
};
float Camera::getYaw() { return Yaw;};

void Camera::setYaw(float yaw) {
  Yaw = yaw;
  updateCameraVectors();
  //view_matrix = GetViewMatrix();
};

float Camera::getFOV() { return Yaw;};

void Camera::setFOV(float f) {
  fov = f;
  projection_matrix = perspective(fov, float(WIDTH)/float(HEIGHT), 0.1f, 1000.0f);
};

void Camera::setFarPlane(float _farPlane) {
    farPlane = _farPlane;
    projection_matrix = perspective(45.0f, float(WIDTH)/float(HEIGHT), nearPlane, farPlane);
}

void Camera::setNearPlane(float _nearPlane) {
    nearPlane = _nearPlane;
    projection_matrix = perspective(45.0f, float(WIDTH)/float(HEIGHT), nearPlane, farPlane);
}

// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
int Camera::ProcessKeyboard(int direction, float deltaTime)
{
  float speed = MovementSpeed * deltaTime;
  if (direction == FORWARD)
    Position += Front * speed;
  if (direction == BACKWARD)
      Position -= Front * speed;
  if (direction == LEFT)
      Position -= Right * speed;
  if (direction == RIGHT)
      Position += Right * speed;
  // TO DO fix gravity
  //entity->velocity[1] = 0.0f;
  return 0;
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
int Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
{
  xoffset *= MouseSensitivity;
  yoffset *= MouseSensitivity;

  Yaw   += xoffset;
  Pitch += yoffset;


  // Make sure that when pitch is out of bounds, screen doesn't get flipped
  if (constrainPitch)
  {
    if (Pitch > 89.0f)
      Pitch = 89.0f;
    if (Pitch < -89.0f)
      Pitch = -89.0f;
  }

  // Update Front, Right and Up Vectors using the updated Euler angles
  updateCameraVectors();
  return 1;
}

// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::ProcessMouseScroll(float yoffset)
{
    if (Zoom >= 1.0f && Zoom <= 45.0f)
        Zoom -= yoffset;
    if (Zoom <= 1.0f)
        Zoom = 1.0f;
    if (Zoom >= 45.0f)
        Zoom = 45.0f;
}

void Camera::updateCameraVectors()
{
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up    = glm::normalize(glm::cross(Right, Front));
}


