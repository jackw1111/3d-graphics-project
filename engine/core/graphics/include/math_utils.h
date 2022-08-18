#ifndef _MATH_UTILS
#define _MATH_UTILS

#include <math.h>
#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_interpolation.hpp>
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


glm::vec3 find_orthogonal(glm::vec3 v);
glm::mat3 formOrthogonalBasis(glm::vec3 const &v);

/*! 
@brief Specifies the bounding box for an AnimatedMesh, used for:
- frustum cull geometry behind the camera
- collision detection and response
*/
class BoundingBox {
public:
    static std::vector<BoundingBox*> allBoundingBoxes;

    unsigned int shaderProgram;
    unsigned int VBO, VAO;

    GLuint queryFront;
    GLuint queryBack;

    glm::vec3 min;
    glm::vec3 max;

    std::vector<glm::vec3> translated_vertices = {};
    std::vector<float> vertices = {};
    glm::vec3 position;
    glm::vec3 velocity;

    glm::mat4 modelMatrix = mat4(1.0f);

    bool toDraw = false;

    BoundingBox();

    BoundingBox( const BoundingBox &obj);


    std::vector<glm::vec3> getTranslatedVertices(glm::mat4 modelMatrix);

    void setup(glm::vec3 min, glm::vec3 max);

    void setMatrix(std::string name, glm::mat4 mat);
    void draw();

    void setBoundary(glm::vec3 min, glm::vec3 max);
};


glm::vec3 reflect(glm::vec3 I, glm::vec3 N);

float boxPlaneIntersect(BoundingBox obb, glm::mat4 modelMatrix, glm::vec3 planeNormal, glm::mat4 planeModelMatrix);

glm::vec3 getPosition(glm::mat4 mat);

float lerp(float a, float b, float f);

vec3 rayCast(double xpos, double ypos, mat4 view, mat4 projection, int WIDTH, int HEIGHT);
bool solveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1);

//bool intersect(const vec3 &ray, const vec3 &sphere, float &sphereT);
bool intersect(const vec3 &pos, const vec3 &ray, const vec3 &sphere, float radius2);

float intersectPlane(vec3 n, vec3 p0, vec3 l0, vec3 l);
float rayIntersectQuad(vec3 n, vec3 p0, vec3 l0, vec3 l, mat4 planeModelMatrix);

float minDistancePointToPlane(glm::vec3 plane_position, glm::vec3 plane_normal, glm::vec3 point);

float rayTriangleIntersect(
    const vec3 &rayOrigin, const vec3 &rayVector,
    const vec3 &vertex0, const vec3 &vertex1, const vec3 &vertex2);

float lineSegmentTriangleIntersect(
    const vec3 &startPos, const vec3 &endPos,
    const vec3 &vertex0, const vec3 &vertex1, const vec3 &vertex2);

bool alongRayTriangleIntersect(
    const vec3 &rayOrigin, const vec3 &rayVector,
    const vec3 &vertex0, const vec3 &vertex1, const vec3 &vertex2,
    float &t);

//float rayObjectIntersect(glm::vec3 rayOrigin, glm::vec3 rayDirection, StaticObject object);

void checkCameraOcclusion(const vec3 &ray_wor, const vec3 &sphere, float &sphereT, float &triangleT, float &distanceFromCamera, const float &setDistance);

bool checkCollision(vec2 ballPosition, vec2 ballSize, vec2 brickPosition, vec2 brickSize);
float distance(vec2 p1, vec2 p2);
int round(float value, int denomination);
/*! @brief data structure to pass around collision specific information
*/
class CollisionInfo {
public:
  glm::vec3 normal;
  glm::vec3 position;
  float depth;
  CollisionInfo();
};

CollisionInfo check_collision(BoundingBox boundingBox1, glm::mat4 modelMatrix1, BoundingBox boundingBox2, glm::mat4 modelMatrix2);
CollisionInfo rayBoxIntersect(glm::vec3 rayOrigin, glm::vec3 rayDirection, BoundingBox &boundingBox);




#endif