#ifndef BASE_CAMERA_H
#define BASE_CAMERA_H

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>


using glm::vec3;
using glm::mat4;
using glm::radians;
using glm::perspective;

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.0f;
const float SENSITIVITY =  0.03f;
const float ZOOM        =  90.0f;
//  *Camera: An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL

/**
 *@Position - a 3D vector
 *@Up -
 *@Front - 
 *@Yaw -
 *@Pitch -
 **/
class Camera
{
public:
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Euler Angles
    float Yaw;
    float Pitch;
    float fov;

    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    mat4 projection_matrix;
    mat4 view_matrix;
    vec3 dPosition;

    Camera();
    Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up, float yaw, float pitch);
    int _setup(glm::vec3 position, glm::vec3 front, glm::vec3 up, float yaw, float pitch);
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
    glm::mat4 GetViewMatrix();
    virtual int ProcessKeyboard(int direction, float deltaTime);
    virtual int ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch);
    virtual void ProcessMouseScroll(float yoffset);
    glm::vec3 getPosition() const;
    void setPosition(glm::vec3 pos);
    glm::vec3 getFront() const;
    void setFront(glm::vec3 front);
    glm::vec3 getRight();
    int setRight(glm::vec3 right);
    float getMovementSpeed();
    void setMovementSpeed(float speed);
    float getPitch();
    void setPitch(float pitch );
    float getYaw();
    void setYaw(float yaw);
    float getFOV();
    void setFOV(float f);

    // Calculates the front vector from the Camera's (updated) Euler Angles
    virtual void updateCameraVectors();
};


#endif
