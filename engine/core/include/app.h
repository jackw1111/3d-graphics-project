#ifndef _APP_H
#define _APP_H

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <vector>
#include <random>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "audio.h"
#include "fbo.h"
#include "camera.h"
#include "math_utils.h"
#include "animated_model.h"
#include "line3d.h"
#include "sky_box.h"
#include "culling.h"

class Scene {
public:
  Scene();
  std::vector<StaticModel*> models;
};


class Application {

public:
    // timing
  float deltaTime = 0.0;
  float lastFrame = 0.0;
  float currentFrame = 0.0;

  float lastX;
  float lastY;
  
  std::string name = "Application";
  GLFWwindow *window;

  Camera active_camera;
  Scene active_scene;
  Skybox sky_box;
  StaticShader sky_box_shader;

  Frustum frustum;

  vec3 backgroundColor;

  AnimatedShader *simpleDepthShader;
  StaticShader *debugDepthQuad;

  const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
  unsigned int depthMapFBO;
  unsigned int depthMap;

  glm::vec3 shadowMapCenter;
  glm::mat4 lightProjection, lightView;
  glm::mat4 lightSpaceMatrix;
  glm::mat4 farLightSpaceMatrix;

  float farPlane = 100.0f;

  bool debug = false;

public:

  Application(std::string title, unsigned int WIDTH, unsigned int HEIGHT, bool fullscreen);
  virtual int setup(std::string title, unsigned int WIDTH, unsigned int HEIGHT, bool fullscreen);

  Application();
  virtual ~Application();
  int gameLoop();
  void drawScene(int shadowPass);
  void drawUI();

  // abstract callbacks (can be overridden, no error if not though)
  virtual void update();
  virtual void onMouseClicked(int button, int action, int mods){};
  virtual void onKeyPressed(int key, int scancode, int action, int mods);
  virtual void onWindowResized(int width, int height){};
  virtual void onMouseMoved(double xpos, double ypos){};
  virtual void onJoystickMoved(int jid, int event){};
  virtual void onMouseScrolled(double xpos, double ypos){};
  void onWindowClose(GLFWwindow* window) {
    glfwSetWindowShouldClose(window, true);
  }
  GLFWwindow* getWindow() const { return window; }
  void setWindow(GLFWwindow *win) { window = win; }

  float getFPS();

  void setBackgroundColor(vec3 color);

  void setFarPlane(float farPlane);

};



#endif
