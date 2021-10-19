#ifndef LINE3D_H
#define LINE3D_H


#include "_engine.h"

using std::vector;
using glm::mat4;
using glm::vec3;

class Line3D {
    int shaderProgram;
    unsigned int VBO, VAO;
    vector<float> vertices;



public:
    vec3 startPoint;
    vec3 endPoint;
    mat4 model;
    mat4 viewProjection;
    vec3 color;

    Line3D();
    Line3D(vec3 start, vec3 end);

    int setModel(mat4 modelMatrix);
    int setCamera(mat4 cameraMatrix);

    int setColor(vec3 col);

    int draw();

    int setEndpoints(vec3 startPoint, vec3 endPoint);

    ~Line3D();

    static std::vector<Line3D*> lines;
};



#endif
