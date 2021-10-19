#include <boost/python/numpy.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/detail/operator_id.hpp>
#include <boost/make_shared.hpp>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

#include <numeric>

using namespace boost;
using namespace boost::python;

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>

#include "_engine.h"

#include "collision2.h"

glm::vec3 getPosition(glm::mat4 mat){
  glm::vec4 v4 = mat[3];
  return glm::vec3(v4.x, v4.y, v4.z);
}

float spherePlaneCollision(glm::vec3 sphereCentre, float sphereRadius, glm::vec3 planeNormal, glm::vec3 pointOnPlane) {
    glm::vec3 v = sphereCentre - pointOnPlane;
    float d = glm::dot(v, planeNormal);

    // penetration depth along v
    return d;
}

glm::vec3 getWorldSpace(glm::vec3 position, glm::mat4 mat) {
  glm::vec3 v4 = mat * glm::vec4(position, 1.0);
  return glm::vec3(v4.x, v4.y, v4.z);
}


bool rayTriangleCollision(glm::vec3 planeIntersection, glm::vec3 planeNormal, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
  glm::vec3 edge0;
  glm::vec3 edge1;
  glm::vec3 edge2;

  glm::vec3 C0;
  glm::vec3 C1;
  glm::vec3 C2;

    edge0 = v2 - v1; 
    edge1 = v3 - v2;
    edge2 = v1 - v3; 

    C0 = planeIntersection - v1; 
    C1 = planeIntersection - v2;
    C2 = planeIntersection - v3; 


    // check that the vector between each vertex and the plane intersection
    // is left of its corresponding triangle side, ie. the dot product is +ve
    if (glm::dot(planeNormal, glm::cross(edge0, C0)) > 0 && 
        glm::dot(planeNormal, glm::cross(edge1, C1)) > 0 && 
        glm::dot(planeNormal, glm::cross(edge2, C2)) > 0) {
        return true; // plane_intersection is inside the triangle
    }
    return false;
}
glm::vec3 rayPlaneIntersection(glm::vec3 ray_position, glm::vec3 ray_direction, glm::vec3 plane_normal, glm::vec3 plane_position) {
    float d = glm::dot(plane_normal, plane_position - ray_position) / (0.001+glm::dot(ray_direction, plane_normal));
    return ray_position + ray_direction * d;
}

glm::vec3 closestPointOnLineSegment2(glm::vec3 A, glm::vec3 B, glm::vec3 point) {
  glm::vec3 AB = B - A;
  float t = glm::dot(point - A, AB) / (0.00001 + glm::dot(AB, AB));
    return A + AB * std::min(std::max(t, 0.0f), 1.0f);
}
glm::vec3 sphereTriangleCollision(glm::vec3 sphereCentre, float sphereRadius, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2) {
    
    glm::vec3 edge0 = p1 - p0;
    glm::vec3 edge1 = p2 - p1;
    glm::vec3 edge2 = p0 - p2;

    glm::vec3 planeNormal = glm::normalize(glm::cross(edge0, edge1));
    // if sphere intersects plane
    float dist = spherePlaneCollision(sphereCentre, sphereRadius, planeNormal, p0);

    glm::vec3 point0 = sphereCentre - planeNormal * dist; // projected sphere center on triangle plane

    bool inside = rayTriangleCollision(point0, planeNormal, p0, p1, p2);

    float radiussq = sphereRadius * sphereRadius;

    // Edge 1:
    glm::vec3 point1 = closestPointOnLineSegment2(p0, p1, sphereCentre);
    glm::vec3 v1 = sphereCentre - point1;
    float distsq1 = glm::dot(v1,v1);
    bool intersects = distsq1 < radiussq;

    // Edge 2:
    glm::vec3 point2 = closestPointOnLineSegment2(p1, p2, sphereCentre);
    glm::vec3 v2 = sphereCentre - point2;
    float distsq2 = glm::dot(v2,v2);
    intersects |= distsq2 < radiussq;

    // Edge 3:
    glm::vec3 point3 = closestPointOnLineSegment2(p2, p0, sphereCentre);
    glm::vec3 v3 = sphereCentre - point3;
    float distsq3 = glm::dot(v3,v3);
    intersects |= distsq3 < radiussq;
  glm::vec3 best_point;

    if(inside) {
        glm::vec3 intersection_vec = sphereCentre - point0;
        glm::vec3 best_point = point0; // is this right? TO DO
        return best_point;
    }

    glm::vec3 d = sphereCentre - point1;
    float best_distsq = glm::dot(d,d);
    best_point = point1;
    glm::vec3 intersection_vec = d;

    d = sphereCentre - point2;
    float distsq = glm::dot(d,d);
    if (distsq < best_distsq) {
        best_distsq = distsq;
        best_point = point2;
        intersection_vec = d;
    }

    d = sphereCentre - point3;
    distsq = glm::dot(d,d);
    if (distsq < best_distsq) {
        best_distsq = distsq;
        best_point = point3;
        intersection_vec = d;
    }

    return best_point; // intersection success
}


BOOST_PYTHON_MODULE(collisions2)
{
	python::def("get_position", getPosition);
  python::def("sphere_plane_collision", spherePlaneCollision);
  python::def("get_world_space", getWorldSpace);
  python::def("ray_triangle_collision", rayTriangleCollision);
  python::def("ray_plane_intersection", rayPlaneIntersection);
  python::def("closest_point_on_line_segment", closestPointOnLineSegment2);
  python::def("sphere_triangle_collision", sphereTriangleCollision);

}