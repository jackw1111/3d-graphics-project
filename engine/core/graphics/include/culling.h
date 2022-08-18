#ifndef _CULLING_H
#define _CULLING_H

#include "math_utils.h"
#include "animated_model.h"

float pointPlaneDistance(const glm::vec3 &point, const glm::vec3 &planePosition, const glm::vec3 &planeNormal);

bool isMinkowskiFace(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &b_x_a, const glm::vec3 &c, const glm::vec3 &d, const glm::vec3 &d_x_c);

class Edge {
public:
    glm::vec3 v0;
    glm::vec3 v1;
    std::vector<int> face_indices;
    Edge(const glm::vec3 &v0, const glm::vec3 &v1);
};

class Face {
public:
    std::vector<glm::vec3> vertices;
    glm::vec3 position;
    glm::vec3 normal;
    Face(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2);
};

class Body {
public:
    glm::vec3 cm_position;
    std::vector<glm::vec3> vertices;
    std::vector<Edge> edges;
    std::vector<Face> faces;

    Body();

    int update(const std::vector<glm::vec3> &vertices);
};

class Query {
public:
    float max_seperation;
    std::vector<int> max_index;
    glm::vec3 best_axis;
    int type;
    Query(float max_seperation, std::vector<int> best_index, glm::vec3 best_axis) {
        this->max_seperation = max_seperation;
        this->max_index = max_index;
        this->best_axis = best_axis;
    }
};

bool operator==(Query q1, const Query q2);
bool operator==(Face q1, const Face q2);
bool operator==(Edge q1, const Edge q2);

std::vector<Query> SAT(const Body &hullA, const Body &hullB);

/*! @brief helper class for culling objects behind the Camera */
class Frustum {
public:
    float fov;
    float nearDist;
    float farDist;
    float ar;
    vec3 Cnear;
    vec3 Cfar;
    vec3 topRightFar;
    vec3 topLeftFar;
    vec3 topRightNear;
    vec3 topLeftNear;
    vec3 bottomLeftNear;
    vec3 bottomLeftFar;
    vec3 bottomRightNear;
    vec3 bottomRightFar;
    vec3 rightNormal;
    vec3 leftNormal;
    vec3 topNormal;
    vec3 bottomNormal;
    vec3 backNormal;
    vec3 frontNormal;
    Body frustum_body;
    Frustum(){};
    Frustum(float _fov, float _nearDist, float _farDist, float _WIDTH, float _HEIGHT, Camera active_camera);
    void cullStaticObject(StaticObject &object);
    void cullAnimatedObject(AnimatedObject &object);
    void reset(AnimatedObject &object);

};

#endif