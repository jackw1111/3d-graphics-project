#ifndef MATH_3D_H
#define MATH_3D_H

#include <glm/ext.hpp>

#include <iostream>
#include <vector>
#include <glm/glm.hpp>

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::mat3;
using glm::mat4;

using glm::dot;
using glm::cross;
using glm::translate;
using glm::rotate;
using glm::scale;

using std::vector;
using std::endl;
using std::cout;
using std::string;
using std::numeric_limits;

mat3 orthonormalizeOrientation(mat3 m);
mat3 setSkewSymmetric(vec3 const &vector);
std::vector<glm::vec3> getAABBVertices(const glm::vec3 &_min, const glm::vec3 &_max);
float clamp(float value, float _min, float _max);
glm::vec3 matrixMultiply(const glm::mat4 &m, const glm::vec3 &v);
std::vector<float> getAABB(const std::vector<glm::vec3> &vertices);
bool AABBOverlap(const glm::vec3 &_min1, const glm::vec3 &_max1, const glm::vec3 &_min2, const glm::vec3 &_max2);
float pointLineIntersection(const glm::vec3 &p3, const glm::vec3 &p1, const glm::vec3 &p2);
float distPointToTriangle(const glm::vec3 &point, const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &v3);
glm::vec2 lineSegmentIntersection(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &p4);
glm::vec2 closestPointsOnLineSegments(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &p4);
float pointPlaneDistance2(const glm::vec3 &point, const glm::vec3 &planePosition, const glm::vec3 &planeNormal);
std::vector<glm::vec3> clipPolygonToPlane(const std::vector<glm::vec3> &polygon, const glm::vec3 &plane_pos, const glm::vec3 &plane_normal);
bool isMinkowskiFace2(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &b_x_a, const glm::vec3 &c, const glm::vec3 &d, const glm::vec3 &d_x_c);
vec3 closest_point_on_circle(vec3 p, vec3 plane_position, vec3 plane_normal, float r);
float sphere_plane_collision(glm::vec3 sphereCentre, glm::vec3 planeNormal, glm::vec3 pointOnPlane);
bool ray_triangle_collision(glm::vec3 planeIntersection, glm::vec3 planeNormal, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);
glm::vec3 closest_point_on_line_segment(glm::vec3 A, glm::vec3 B, glm::vec3 point);
vec3 closest_point_on_obb(vec3 point, float box_dX2, float box_dY2, float box_dZ2, vec3 box_position, mat3 box_orientation);
float ray_obb_intersection(glm::vec3 ray_origin, glm::vec3 ray_direction, glm::vec3 aabb_min, glm::vec3 aabb_max, glm::mat4 ModelMatrix);
#endif