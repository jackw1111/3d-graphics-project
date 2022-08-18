#ifndef LINE_2D_H
#define LINE_2D_H

#include "engine.h"
#include "math_utils.h"

/*! @brief 2D line drawing class, mainly used for debugging */
class Line2D {
    int shaderProgram;
    unsigned int VBO, VAO;
    vector<float> vertices;
    mat4 MVP = mat4(1.0);
    float w;
    float h;
public:
    vec2 startPoint;
    vec2 endPoint;
    vec3 lineColor;

    Line2D(vec2 start, vec2 end, float WIDTH, float HEIGHT);

    void setEndpoints(vec2 start, vec2 end);
 
    int setMVP(mat4 mvp);
 
    int setColor(vec3 color);
 
    int draw();
 
    ~Line2D();

    static std::vector<Line2D*> lines;
};

#endif
