#ifndef LABEL_3D_H
#define LABEL_3D_H

#include <map>

#include "static_shader.h"
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

#include "label2d.h"


extern unsigned int WIDTH;
extern unsigned int HEIGHT;


/*! @brief 3D text utility */
class Label3D {

    int _isInit = false;


    std::map<GLchar, Character> Characters;
    GLuint VAO, VBO;


public:
    Label3D(std::string text, glm::vec2 position, std::string font, GLfloat scale);
    ~Label3D();
    int draw();

    static std::vector<Label3D*> labels;
    std::string text;
    glm::vec2 position;
    glm::vec3 color;
    std::string font;
    GLfloat scale;
    StaticShader shader;
    bool toDraw = true;
};

#endif
