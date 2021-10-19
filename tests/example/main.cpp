#include "_engine.h"
#include <math.h>



unsigned int WIDTH = 800;
unsigned int HEIGHT = 600;

class App : public Application {
    StaticModel *axis_3d;
    StaticModel *block1;
    StaticModel *block2;
    AnimatedModel *person;
    Light *l;
public:
    //std::unique_ptr<StaticModel> axis_3d; // TO DO
	App(std::string title, unsigned int WIDTH, unsigned int HEIGHT, bool fullscreen)
        : Application(title, WIDTH, HEIGHT, fullscreen) {
        std::cout << "inside app constructor" << std::endl;

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        //axis_3d = std::unique_ptr<StaticModel>(new StaticModel("../../GUI/3d_axis.obj")); // TO DO

		l = new Light(vec3(0, 1, 4), vec3(1,1,1));
        //axis_3d = new StaticModel("../../GUI/3d_axis.obj");

        block1 = new StaticModel("./data/grass_block.obj");
        block1->model = translate(mat4(1.0), vec3(0.0, 3.0, 0.0)); // TO DO

        block2 = new StaticModel("./data/grass_block.obj");
        block2->model = scale(mat4(1.0), vec3(5.0, 0.1, 5.0));
        person = new AnimatedModel("./data/astroboy.dae");
        person->model = translate(mat4(1.0), vec3(0, 3, 0));
        //sky_box.loadSkybox = false;

	}

	void update() override {
        Application::update();  // TO DO
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
        person->getFrame(currentFrame);

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

	}
};

Application *getApplication() {
	return new App("example", WIDTH, HEIGHT, false);
}

