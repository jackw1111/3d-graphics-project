#ifndef _MATH_UTILS
#define _MATH_UTILS

#include <math.h>
#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <algorithm>
#include "static_model.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#ifndef __GL_HEADERS
#define __GL_HEADERS
#include <glad/glad.h>
#endif

using std::vector;

using glm::vec2;
using glm::vec3;
using glm::dot;
using glm::normalize;
using glm::vec4;
using glm::mat4;

glm::vec3 getPosition(glm::mat4 mat);

float lerp(float a, float b, float f);

vec3 rayCast(double xpos, double ypos, mat4 projection, mat4 view);
bool solveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1);

//bool intersect(const vec3 &ray, const vec3 &sphere, float &sphereT);
bool intersect(const vec3 &pos, const vec3 &ray, const vec3 &sphere, float radius2);

float intersectPlane(vec3 n, vec3 p0, vec3 l0, vec3 l);

float rayTriangleIntersect(
    const vec3 &rayOrigin, const vec3 &rayVector,
    const vec3 &vertex0, const vec3 &vertex1, const vec3 &vertex2);

bool alongRayTriangleIntersect(
    const vec3 &rayOrigin, const vec3 &rayVector,
    const vec3 &vertex0, const vec3 &vertex1, const vec3 &vertex2,
    float &t);

float rayObjectIntersect(glm::vec3 rayOrigin, glm::vec3 rayDirection, StaticObject object);

void checkCameraOcclusion(const vec3 &ray_wor, const vec3 &sphere, float &sphereT, float &triangleT, float &distanceFromCamera, const float &setDistance);

bool checkCollision(vec2 ballPosition, vec2 ballSize, vec2 brickPosition, vec2 brickSize);
float distance(vec2 p1, vec2 p2);
int round(float value, int denomination);

class Poly {
public:
  StaticModel model;
  glm::mat4 modelMatrix;
    std::vector<glm::vec3> translated_vertices = {};
    std::vector<Vertex> vertices;
    Poly(){};
  int setup(glm::mat4 modelMatrix);
  int setup2(std::vector<Vertex> vertices);
  int getTranslatedVertices(std::vector<Vertex> vertices);
};

bool check_collision(BCube boundingCube1, BCube boundingCube2);

#endif