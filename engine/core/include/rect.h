#ifndef _RECT_H
#define _RECT_H

#include <stb_image.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <map>

#include "static_shader.h"

class Rect {

public:

	unsigned int VBO, VAO, EBO;
    unsigned int texture;
	StaticShader rectShader;
	unsigned char *data;
    int _width, _height, nrChannels;
	bool withImage = false;
	glm::vec3 colour;
    glm::vec2 position;
    glm::vec2 size;
    bool toDraw = true;
    int ordering = 1;
    int shaded = 0;

    static std::vector<Rect*> rects;

    ~Rect();

    Rect(glm::vec2 pos, glm::vec2 size, std::string filePath);

    int setupWithImage(std::string imageLocation);
    void setupWithColour(glm::vec3 colour);
    int draw();

    int remove();

};

#endif