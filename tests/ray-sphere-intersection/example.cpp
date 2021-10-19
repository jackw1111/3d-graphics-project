#include "engine.h"
#include "bindings.h"
unsigned WIDTH   = 800;
unsigned HEIGHT  = 600;

class Example : public Application {


public:
  Example(std::string title, unsigned int WIDTH, unsigned int HEIGHT, bool fullscreen) : Application(title, WIDTH, HEIGHT, fullscreen) {

  }

  void update() override {


    // per-frame time logic
    // --------------------
    float currentFrame = glfwGetTime();
    deltaTime = (float)currentFrame - (float)lastFrame;
    lastFrame = currentFrame;


    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  }

  void onMouseMoved(double xpos, double ypos) override {

  }


  int processInput(GLFWwindow* window) {

  
  }
  void onWindowResized(int width, int height) override {

  }

  void onMouseClicked(int button, int action, int mods) override {

  }

  // void onKeyPressed(GLFWwindow *window, int key, int scancode, int action, int mods) override {

  // }

  void onWindowResized(GLFWwindow* window, int width, int height)
  {

  }

};


Application* getApplication() {
  return new Example("Minecraft",WIDTH, HEIGHT, false);;
}



