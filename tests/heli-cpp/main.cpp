#include "engine.h"

#include <math.h>

unsigned int WIDTH = 1280;
unsigned int HEIGHT = 720;

class ParticleWrap : public Particle {
public:
    int frameNumber = 0;
    ParticleWrap(std::string s, glm::mat4 m, int row, int col) : Particle(s, m , row, col) {

    }
};

class Helicopter {
    
public:
    AnimatedObject model;
    AudioWindow audioWindow;
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 velocity;

    float yaw = 0.0f;
    float pitch = 0.0f;
    float roll = 0.0f;

    float speed = 0.0f;
    float maxSpeed = 10.0f;

    bool thirdPerson = true;

    Helicopter() {
        model = AnimatedObject("./data/heli.dae");
        model.setDrawBoundingBox(true);
        //audioWindow.playAudio("./data/heli.wav");
        //audioWindow.setVolume("./data/heli.wav", 0.05);
    }
};

std::vector<float> getJoystickAxes() {
  if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
        int axesCount;
        const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);

        std::vector<float> axesVec;
        for (int i = 0; i < 6; i++) {
          axesVec.push_back(axes[i]);
        }
        return axesVec;
  } else {
    std::vector<float> axesVec(6);
    return axesVec;
  }
}
// TO DO fix bug of view matrix not updating for frustum cull
class HeliDemo : public Application {
    Light *l;
    Helicopter heli;
    glm::vec3 gravity = glm::vec3(0.0f, -10.0f, 0.0f);
    unsigned tick = 0;
    std::vector<StaticModel> v;
    std::vector<glm::mat4> mapMatrix;
    CharacterEntity entity;
    bool firstMouse = false;
    bool shiftPressed = false;
    float cameraOffset = 0.0f;
    float _yaw = active_camera.Yaw;
    float i = 0.0f;
    std::vector<ParticleWrap*> smokeTrail = {};
    int noOfSmokePuffs = 100;
    int timer = 0;

    Line3D frontLine;
    Line3D rightLine;
    Line3D upLine;

    float rlastX = 0.0f;
    float rlastY = 0.0f;

public:
	HeliDemo(std::string title, unsigned int WIDTH, unsigned int HEIGHT, bool fullscreen)
        : Application(title, WIDTH, HEIGHT, fullscreen) {

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        setBackgroundColor(glm::vec3(173.0f/255.0f, 216.0f/255.0f, 230.0f/255.0f));

		l = new Light(vec3(0, 1, 4), vec3(1,1,1));
        active_camera.setFarPlane(100.0f);
        StaticObject map("./data/mountains.obj");
        map.setModelMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(400,200,400)));
        
        active_camera.Yaw = -90.0f;

        heli.front = active_camera.Front;
        heli.yaw = -90.0f;
        heli.pitch = 0.0f;
        heli.roll = 0.0f;
        heli.right = active_camera.Right;
        heli.position = glm::vec3(0.0f, 30.0f, 0.0f);
        heli.speed = 0.0f;
        heli.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
        heli.maxSpeed = 10.0f;

        v = {map.getModel()};
        mapMatrix = {map.getModelMatrix()};
        entity = CharacterEntity(v, mapMatrix, glm::vec3(5.0f, 5.0f, 5.0f));
        entity.addModel(v, mapMatrix);
        entity.position = glm::vec3(-10.0f, 11.5f, 0.0f);
	    useCustomViewMatrix = true;

        frontLine.color = glm::vec3(0.0f,0.0f,1.0f);
        rightLine.color = glm::vec3(1.0f,0.0f,0.0f);
        upLine.color = glm::vec3(0.0f,1.0f,0.0f);

    }

    void processInput(GLFWwindow *window) {

        if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
            std::cout << "joysticks" << std::endl;   
            float speed = active_camera.MovementSpeed;
            vector<float> axes = getJoystickAxes();
            float xpos = (axes[3] + 1) * WIDTH / 2;
            float ypos = (axes[4] + 1) * HEIGHT / 2;
            float xoffset = xpos - lastX;
            float yoffset = lastY - ypos;
            lastX = xpos;
            lastY = ypos;

            active_camera.ProcessMouseMovement(axes[3]*speed, axes[4]*speed, true);

            float rxpos = (axes[1] + 1) * WIDTH / 2;
            float rypos = (axes[0] + 1) * HEIGHT / 2;

            float rxoffset = rxpos - rlastX;
            float ryoffset = rypos - rlastY;

            rlastX = rxpos;
            rlastY = rypos;
            float tolerance = 25.0f;

           if (!heli.thirdPerson) {
                active_camera.Pitch += axes[4]*speed;
           }

            heli.pitch += axes[4]*speed;
            heli.roll += axes[3]*speed;

            heli.yaw += axes[0]*speed;
            if (!heli.thirdPerson) {
                active_camera.Yaw += axes[0]*speed;
            }

            if (xoffset){
                heli.speed = 30.0f * -axes[1];
            } else {
                heli.speed = -10.0f;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            firstMouse = true;
            heli.speed = 30.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            heli.speed = -30.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            heli.yaw -= 1;
            if (!heli.thirdPerson) {
                active_camera.Yaw += -1;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            heli.yaw += 1;
            if (!heli.thirdPerson) {
                active_camera.Yaw -= 1;
            }
        }

        // if (glfwGetKey(window, GLFW_KEY_W) != GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) != GLFW_PRESS) {
        //     heli.speed = 0.0f;
        // }

        active_camera.Yaw = heli.yaw;
        if (shiftPressed) {
            active_camera.Yaw += cameraOffset;
        }
    }

	void update() override {
        //std::cout << getFPS() << std::endl;
        processInput(window);

        glm::vec3 front(0.0f, 0.0f, 0.0f);
        front.x = cos(glm::radians(heli.yaw)) * cos(glm::radians(heli.pitch));
        front.y = sin(glm::radians(heli.pitch));
        front.z = sin(glm::radians(heli.yaw)) * cos(glm::radians(heli.pitch));
	
        heli.front = glm::normalize(front);
        glm::mat4 rollMat = glm::rotate(glm::mat4(1.0f), glm::radians(heli.roll), heli.front);
        heli.right = glm::normalize(glm::cross(heli.front, glm::vec3(0.0f,1.0f,0.0f))); 

        glm::vec4 right = glm::vec4(heli.right.x, heli.right.y, heli.right.z, 1.0f);
        right = rollMat * right;
        heli.right = glm::vec3(right.x, right.y, right.z);
        heli.up = glm::normalize(glm::cross(heli.right, heli.front));

        frontLine.setEndpoints(entity.position + glm::vec3(0.0f,1.0f,0.0f), entity.position + glm::vec3(0.0f,1.0f,0.0f) + front);
        rightLine.setEndpoints(entity.position + glm::vec3(0.0f,1.0f,0.0f), entity.position + glm::vec3(0.0f,1.0f,0.0f) + heli.right);
        upLine.setEndpoints(entity.position + glm::vec3(0.0f,1.0f,0.0f), entity.position + glm::vec3(0.0f,1.0f,0.0f) + heli.up);
        glm::vec3 dv = heli.up * deltaTime * heli.speed;
        heli.velocity += dv;
        if (firstMouse) {
            heli.velocity += gravity * deltaTime;
        }
        if (glm::length(heli.velocity) > heli.maxSpeed) {
            heli.velocity = glm::normalize(heli.velocity) * heli.maxSpeed;
        }
        heli.position += heli.velocity * deltaTime;

        heli.velocity.x *= 0.9999f;
        heli.velocity.z *= 0.9999f;

        heli.model.setModelMatrix(glm::translate(glm::mat4(1.0f), heli.position));
        if (!heli.thirdPerson) {
            heli.model.setModelMatrix(glm::translate(heli.model.getModelMatrix(), heli.up * 0.5f));
        }

        heli.model.setModelMatrix(glm::rotate(heli.model.getModelMatrix(), glm::radians(-heli.yaw), heli.up));
        heli.model.setModelMatrix(glm::rotate(heli.model.getModelMatrix(), glm::radians(heli.pitch), heli.right));
        heli.model.setModelMatrix(glm::rotate(heli.model.getModelMatrix(), glm::radians(heli.roll), heli.front));

        heli.model.setModelMatrix(glm::rotate(heli.model.getModelMatrix(), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    
        float t = currentFrame - (int)currentFrame;
        heli.model.setFrames(0.0f, 5.0f, currentFrame);

        if (heli.thirdPerson) {
            float modelHeight = 20.0f;
            float yTheta = -active_camera.Pitch;
            float horizDist = modelHeight * cos(radians(yTheta));
            float vertDist = modelHeight * sin(radians(yTheta));
            float xTheta = active_camera.Yaw - 90.0f;
            float offsetX = horizDist * sin(glm::radians(-xTheta));
            float offsetZ = horizDist * cos(glm::radians(xTheta));

            active_camera.Position = heli.position + glm::vec3(-offsetX, vertDist, -offsetZ) + glm::vec3(0.0f, 4.0f, 0.0f);
        } else {
            active_camera.Position = heli.position - heli.up * 0.5f + heli.front * 0.7f;
        }

        entity.velocity = heli.velocity;
        entity.velocity.x *= 0.05f;
        entity.velocity.y *= 0.05f;
        entity.velocity.z *= 0.05f;

        entity.gravity = vec3(0,0,0);
        entity.update();
        heli.position = entity.position;
        if (!heli.thirdPerson) {
            active_camera.view_matrix = glm::lookAt(active_camera.Position, active_camera.Position + active_camera.Front, heli.up);
        } else {
            active_camera.view_matrix = glm::lookAt(active_camera.Position, entity.position, glm::vec3(0.0f,1.0f,0.0f));
        }
        std::cout << "view matrix " << glm::to_string(active_camera.view_matrix) << std::endl;

        if (smokeTrail.size() < noOfSmokePuffs) {
            ParticleWrap *newParticle = new ParticleWrap("./data/smoke_atlas.png", glm::mat4(1.0f), 8, 8);
            newParticle->frameNumber =  rand() % 64 + 1;
            float _scale =  rand() % 4 + 1;
            newParticle->modelMatrix = glm::scale(glm::translate(glm::mat4(1.0f), heli.position-heli.up*2.0f-heli.front*4.0f), glm::vec3(_scale, _scale, _scale));
            float alpha =  rand() % 10 + 5;
            alpha *= 0.01f;
            newParticle->alpha = alpha;
            smokeTrail.push_back(newParticle);
        } else {
            ParticleWrap *newParticle = smokeTrail.at(0);
            newParticle->frameNumber = rand() % 64 + 1;
            float _scale =  rand() % 4 + 1;
            newParticle->modelMatrix = glm::scale(glm::translate(glm::mat4(1.0f), heli.position-heli.up*2.0f-heli.front*4.0f), glm::vec3(_scale, _scale, _scale));
            float alpha =  rand() % 10 + 5;
            alpha *= 0.01f;
            newParticle->alpha = alpha;
            smokeTrail.push_back(newParticle);
            smokeTrail.erase(smokeTrail.begin());
        }

        for (unsigned int i = 0; i < smokeTrail.size(); i++) {
            ParticleWrap *currentParticle = smokeTrail.at(i);
            currentParticle->row = (int)currentParticle->frameNumber / 8;
            currentParticle->col = currentParticle->frameNumber - 8 * currentParticle->row;
            currentParticle->modelMatrix = glm::scale(glm::translate(currentParticle->modelMatrix, glm::vec3(0,deltaTime*0.5f,0)), vec3(1.0f+deltaTime, 1.0f+deltaTime, 1.0f+deltaTime));
            currentParticle->frameNumber += 1;
            currentParticle->alpha *= 0.99f;
        }
    }

	void onMouseMoved(double xpos, double ypos) override {
        double xoffset = xpos - lastX;
        double yoffset = lastY - ypos;

        if (shiftPressed) {
            cameraOffset += xoffset * 0.1f;
        }

        lastX = xpos;
        lastY = ypos;

        if (!heli.thirdPerson) {
            active_camera.Pitch += -yoffset * deltaTime;
        }

        heli.pitch += -yoffset * deltaTime;
        heli.roll += xoffset * deltaTime;


        //active_camera.ProcessMouseMovement(xoffset, yoffset, true);
	}

	void onKeyPressed(int key, int scancode, int action, int mods) override {
        if (action == 1) {
            if (key == 70) { // KEY_F = 70
                if (heli.thirdPerson) {
                    heli.thirdPerson = false;
                    heli.audioWindow.setVolume("./data/heli.wav", 0.02f);
                } else {
                    heli.thirdPerson = true;
                    heli.audioWindow.setVolume("./data/heli.wav", 0.05f);
                }
            }
        }
        // KEY_LEFT_SHIFT = 340
        if (key == 340 && action == 1) {
            shiftPressed = true;
        }
        if (key == 340 && action == 0) {
            shiftPressed = false;
            cameraOffset = 0;
        }
	}
};

Application *getApplication() {
	return new HeliDemo("heli-cpp", WIDTH, HEIGHT, true, false);
}

