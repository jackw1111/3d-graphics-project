#include "_engine.h"

#include <math.h>

unsigned int WIDTH = 800;
unsigned int HEIGHT = 600;

class MyApp : public Application {
    Light *l;
    float offset = 3.0f;
public:
	MyApp(std::string title, unsigned int WIDTH, unsigned int HEIGHT, bool fullscreen)
        : Application(title, WIDTH, HEIGHT, fullscreen) {

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		l = new Light(vec3(0, 1, 4), vec3(1,1,1));

        StaticObject block1("./data/grass_block.obj");
        block1.setModelMatrix(glm::translate(mat4(1.0), vec3(0.0, 3.0, 0.0)));

        StaticObject block2("./data/grass_block.obj");
        block2.setModelMatrix(glm::scale(mat4(1.0), vec3(5.0, 0.1, 5.0)));

        AnimatedObject person("./data/astroboy.dae");
        person.setModelMatrix(glm::translate(mat4(1.0), vec3(0, 3, 0)));
        person.setFrames(0.0, 1.0, 0.55);

	}

	void update() override {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            active_camera.ProcessKeyboard(0, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            active_camera.ProcessKeyboard(1, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            active_camera.ProcessKeyboard(2, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            active_camera.ProcessKeyboard(3, deltaTime);
        }
        l->position = vec3(2*sin(currentFrame), 5, 2*cos(currentFrame));
        //std::cout << "FPS =" << 1.0/(deltaTime) << std::endl;
	}

	void onMouseMoved(double xpos, double ypos) override {
        double xoffset = xpos - lastX;
        double yoffset = lastY - ypos;

        lastX = xpos;
        lastY = ypos;
        active_camera.ProcessMouseMovement(xoffset, yoffset, true);
	}

	void onKeyPressed(int key, int scancode, int action, int mods) override {
        if (key == GLFW_KEY_1) {
            std::cout << "key pressed!" << std::endl;
            offset += 3;
            AnimatedObject tmp_model("./data/astroboy.dae");
            tmp_model.setModelMatrix(glm::translate(mat4(1.0), vec3(0, offset, 0)));
            tmp_model.setFrames(0.0, 1.0, 0.15); 
        }
	}
};

Application *getApplication() {
	return new MyApp("example", WIDTH, HEIGHT, false);
}

