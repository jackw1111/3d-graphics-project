#ifndef PARTICLE_H
#define PARTICLE_H

#include <stb_image.h>
#include <algorithm>

#include "engine.h"
#include "camera.h"

/*! @brief 3D billboard style particles
see: tests/particles for an example */
class Particle {

public:

	unsigned char *data;
    int _width, _height, nrChannels;
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    unsigned int texture;
    std::string filePath;
    unsigned int cols = 1;
    unsigned int rows = 1;

    int frame = 0;

    float col = 1.0f;
    float row = 1.0f;

    float alpha = 1.0f;

    int order = 1;

    unsigned int blendMode = GL_ONE_MINUS_SRC_ALPHA;

    ~Particle();

    Particle(std::string filePath, glm::mat4 modelMatrix, unsigned int cols, unsigned int rows);

    int remove();

    int getFrame();
    void setFrame(int f);

    static int drawAllParticles(Camera active_camera, int order);
    static int setCamera(glm::mat4 cameraMatrix);

    static StaticShader rectShader;
    static unsigned int VBO, VAO, EBO;
    static std::vector<float> vertices;
    static std::vector<unsigned int> indices; 
    static bool setup;
    static glm::mat4 viewProjection;
    static std::vector<std::vector<Particle*>> particles;
    static std::map<std::string, unsigned int> textureMap;

};


#endif
