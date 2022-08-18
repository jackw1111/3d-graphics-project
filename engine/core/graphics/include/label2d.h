#ifndef LABEL_2D_H
#define LABEL_2D_H

#include <map>

#include "static_shader.h"
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H


extern unsigned int WIDTH;
extern unsigned int HEIGHT;

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;    // Horizontal offset to advance to next glyph
};

/*! @brief 2D text utility */
class Label2D {

    int _isInit = false;


    std::map<GLchar, Character> Characters;
    GLuint VAO, VBO;


public:
    Label2D(std::string text, glm::vec2 position, std::string font, GLfloat scale);
    ~Label2D();
    int draw();

    static std::vector<Label2D*> labels;
    std::string text;
    glm::vec2 position;
    glm::vec3 color;
    std::string font;
    GLfloat scale;
    StaticShader shader;
    bool toDraw = true;
};

#endif
