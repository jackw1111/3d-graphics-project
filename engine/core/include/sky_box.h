#ifndef __SKY_BOX_H
#define __SKY_BOX_H

#include "static_shader.h"
#include "stb_image.h"

/*! @brief environment cubemap class, defaults to a generic skybox if none specified.
- must be explicitly turned off with Application.setSkybox = false
- if you want to use a custom one, you must include a folder ./data/skybox with all jpg face images. */
class Skybox {
public:
    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    unsigned int cubemapTexture;
    bool loadSkybox = true;
    bool loadDefaultCube = false;
	Skybox();

    void load(std::vector<std::string> faces);
    void Draw(StaticShader skyboxShader);
    void bindTexture();
    // loads a cubemap texture from 6 individual texture faces
    // order:
    // +X (right)
    // -X (left)
    // +Y (top)
    // -Y (bottom)
    // +Z (front) 
    // -Z (back)
    // -------------------------------------------------------
    unsigned int loadCubemap(std::vector<std::string> faces);
};


#endif
