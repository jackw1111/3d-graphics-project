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
#include <array>

#ifdef GAME_TOOL
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/bind.h>
using namespace emscripten;
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GLES2/gl2.h>
#endif
#else
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>
#include "audio.h"
#include "fbo.h"
#include "camera.h"
#include "math_utils.h"
#include "animated_model.h"
#include "line3d.h"
#include "sky_box.h"
#include "culling.h"


/*! @brief TODO */
class Scene {
public:
  Scene();
  std::vector<StaticModel*> models;
};


class Data {
public:
    glm::vec3 normal;
    glm::vec3 point;
    float penetration;
    int colliding;
};


/*! @brief Main entry point into the graphics program and all game logic is written somewhere inside this.
- 1 created per program (see Scene for WIP on more than one graphics application per context)
- Must be overriden (can be overriden from Python)
- Responsible for updating, drawing, input handling and context handling
*/
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

  unsigned int shadowMapResolution = 2048;

  unsigned int SHADOW_WIDTH = shadowMapResolution;
  unsigned int SHADOW_HEIGHT = shadowMapResolution;
  unsigned int depthMapFBO;
  unsigned int depthMap;

  glm::vec3 shadowMapCenter;
  glm::mat4 lightProjection, lightView;
  glm::mat4 lightSpaceMatrix;
  glm::mat4 farLightSpaceMatrix;

  bool debug = false;

  unsigned int ditherMap;

  bool useCustomViewMatrix = false;

  bool MSAA = false;

  bool frustumCullingEnabled = false;

public:

  Application(std::string title, unsigned int WIDTH, unsigned int HEIGHT, bool fullscreen, bool MSAA);
  virtual int setup(std::string title, unsigned int WIDTH, unsigned int HEIGHT, bool fullscreen, bool MSAA);

  Application();
  virtual ~Application();
  int gameLoop();
  int draw();

  void drawScene(int shadowPass);
  void drawUI();

  void createShadowFBO();

  unsigned int getShadowMapResolution();
  void setShadowMapResolution(unsigned int r);

  // abstract callbacks (can be overridden, no error if not though)
  virtual void update();
  virtual void onMouseClicked(int button, int action, int mods){};
  virtual void onKeyPressed(int key, int scancode, int action, int mods);
  virtual void onCharPressed(unsigned int _char);
  virtual void onWindowResized(int width, int height){};
  virtual void onMouseMoved(double xpos, double ypos){};
  virtual void onJoystickMoved(int jid, int event){};
  virtual void onMouseScrolled(double xpos, double ypos){};

  void onWindowClose(GLFWwindow* window) {
    glfwSetWindowShouldClose(window, true);
  }
  #ifdef GAME_TOOL
  //
  #else
  GLFWwindow* getWindow() const { return window; }
  void setWindow(GLFWwindow *win) { window = win; }
  #endif

  float getFPS();

  void setBackgroundColor(vec3 color);

};



#endif
