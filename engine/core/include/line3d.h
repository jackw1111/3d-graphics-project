#ifndef LINE3D_H
#define LINE3D_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include "glad/glad.h"

using std::vector;
using glm::mat4;
using glm::vec3;
using std::cout;
using std::endl;

/*! @brief Efficient 3D line drawing class, mainly used for debugging */
class Line3D {

public:
    vec3 startPoint;
    vec3 endPoint;
    vec3 color;

    bool project = true;

    Line3D();
    Line3D(vec3 start, vec3 end);


    int setColor(vec3 col);


    int setEndpoints(vec3 startPoint, vec3 endPoint);

    ~Line3D();

    int setProject(int p);
    int getProject();

    static int drawAllLines();
    static int setCamera(mat4 cameraMatrix);

    static int shaderProgram;
    static unsigned int VBO, VAO;
    static vector<float> vertices;
    static bool setup;
    static mat4 viewProjection;
    static bool dirtyPositions;
    static bool dirtyColors;
    static bool dirtyLines;

    static std::vector<Line3D*> lines;
};

#endif
