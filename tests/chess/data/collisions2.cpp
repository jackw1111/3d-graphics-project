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

CharacterEntity2::CharacterEntity2(StaticModel scene_object, glm::mat4 object_model_mat, glm::vec3 top, glm::vec3 bottom, float sphere_radius) {
    this->scene_object = scene_object;
    this->object_model_mat = object_model_mat;
    this->top = top;
    this->bottom = bottom;
    this->position = (top + bottom) * 0.5f;
    this->sphere_radius = sphere_radius;
    this->velocity = glm::vec3(0,0,0);
    this->movementSpeed = 10.0f;
    this->gravitySpeed = 50.0f;
    this->displacement_vec = glm::vec3(0,0,0);
    for (int i = 0; i < scene_object.meshes.size(); i++) {
        std::vector<std::vector<vec3>> t = *(scene_object.meshes[i].getTriangles(mat4(1.0)));
        for (int k = 0; k < t.size(); k++) {
            glm::vec4 a(t[k][0], 1.0f);
            glm::vec4 b(t[k][1], 1.0f);
            glm::vec4 c(t[k][2], 1.0f);
            a = object_model_mat * a;
            b = object_model_mat * b;
            c = object_model_mat * c;
            triangles.push_back({glm::vec3(a), glm::vec3(b), glm::vec3(c)});
        }
    }

    this->closestRegion = NULL;
    this->cube = new Octree(0, this->position, 100.0, triangles);
}

bool CharacterEntity2::capsuleTriangleCollision(glm::vec3 tip, glm::vec3 base, float sphere_radius,glm::vec3 p0, glm::vec3 p1, glm::vec3 p2) {
    // find trace point
    glm::vec3 CapsuleNormal = glm::normalize(tip - base);
    glm::vec3 edge0 = p1 - p0; 
    glm::vec3 edge1 = p2 - p1; 
    glm::vec3 edge2 = p0 - p2; 
    glm::vec3 plane_normal = glm::normalize(glm::cross(edge0, edge1));
    glm::vec3 line_plane_intersection;

    if (glm::dot(CapsuleNormal, plane_normal) == 0){
        line_plane_intersection = p0;
    }
    else {
        line_plane_intersection = rayPlaneIntersection(base, CapsuleNormal, plane_normal, p0);
    }

    glm::vec3 LineEndOffset = CapsuleNormal * sphere_radius; 
    glm::vec3 A = base + LineEndOffset;
    glm::vec3 B = tip - LineEndOffset;

    // find closest point on triangle to trace point
    bool inside = rayTriangleCollision(CapsuleNormal, plane_normal, p0, p1, p2);
    if (inside){
        // The center of the best sphere candidate:
        glm::vec3 sphere_centre = closestPointOnLineSegment2(A, B, line_plane_intersection);
        glm::vec3 intersection_vec = sphereTriangleCollision(sphere_centre, sphere_radius, p0, p1, p2);

        if (glm::length(sphere_centre - intersection_vec) <= sphere_radius) { 
            float depth = (sphere_radius - glm::length(sphere_centre - intersection_vec));
            this->displacement_vec = glm::normalize(sphere_centre - intersection_vec) * depth;
            // Modify player velocity to slide on contact surface:
            float velocity_length = glm::length(this->velocity);
            glm::vec3 velocity_normalized = this->velocity / velocity_length;
            glm::vec3 undesired_motion = plane_normal * glm::dot(velocity_normalized, plane_normal);
            glm::vec3 desired_motion = velocity_normalized - undesired_motion;
            this->velocity = desired_motion * velocity_length;
            this->displacement_vec = plane_normal * (depth + 0.0001f);
            this->position += this->displacement_vec;

            // Remove penetration (penetration epsilon added to handle infinitely small penetration):
            //capsule.Translate();
            //this->displacement_vec = plane_normal * (depth + 0.0001f);
            // Remove penetration (penetration epsilon added to handle infinitely small penetration):
            //capsule.Translate();

            return true;
        }
        else {
            this->displacement_vec = glm::vec3(0,0,0);
            return false;
        }    
    }

    float radiussq = sphere_radius * sphere_radius;

    // Edge 1:
    glm::vec3 point1 = closestPointOnLineSegment2(p0, p1, line_plane_intersection);
    glm::vec3 v1 = line_plane_intersection - point1;
    float distsq1 = glm::dot(v1,v1);
    bool intersects = distsq1 < radiussq;

    // Edge 2:
    glm::vec3 point2 = closestPointOnLineSegment2(p1, p2, line_plane_intersection);
    glm::vec3 v2 = line_plane_intersection - point2;
    float distsq2 = glm::dot(v2,v2);
    intersects |= distsq2 < radiussq;

    // Edge 3:
    glm::vec3 point3 = closestPointOnLineSegment2(p2, p0, line_plane_intersection);
    glm::vec3 v3 = line_plane_intersection - point3;
    float distsq3 = glm::dot(v3,v3);
    intersects |= distsq3 < radiussq;

    glm::vec3 d = line_plane_intersection - point1;
    float best_distsq = glm::dot(d,d);
    glm::vec3 best_point = point1;
    glm::vec3 intersection_vec = d;

    d = line_plane_intersection - point2;
    float distsq = glm::dot(d,d);
    if (distsq < best_distsq) {
        best_distsq = distsq;
        best_point = point2;
        intersection_vec = d;
    }

    d = line_plane_intersection - point3;
    distsq = glm::dot(d,d);
    if (distsq < best_distsq) {
        best_distsq = distsq;
        best_point = point3;
        intersection_vec = d;
    }

    // The centre of the best sphere candidate:
    glm::vec3 sphere_centre = closestPointOnLineSegment2(A, B, best_point);
    intersection_vec = sphereTriangleCollision(sphere_centre, sphere_radius, p0, p1, p2);

    if (glm::length(sphere_centre - intersection_vec) <= sphere_radius) { 
        float depth = (sphere_radius - glm::length(sphere_centre - intersection_vec));
        this->displacement_vec = glm::normalize(sphere_centre - intersection_vec) * depth;
        // Modify player velocity to slide on contact surface:
        float velocity_length = glm::length(this->velocity);
        glm::vec3 velocity_normalized = this->velocity / velocity_length;
        glm::vec3 undesired_motion = glm::normalize(plane_normal) * glm::dot(velocity_normalized, plane_normal);
        glm::vec3 desired_motion = velocity_normalized - undesired_motion;
        this->velocity = desired_motion * velocity_length;
        this->displacement_vec = glm::normalize(plane_normal) * (depth + 0.0001f);
        this->position += this->displacement_vec;
        // Remove penetration (penetration epsilon added to handle infinitely small penetration):
        //capsule.Translate();
        //std::cout << "displacement " << to_string(this->displacement_vec) << std::endl;

        return true;
    }
    else {
        this->displacement_vec = glm::vec3(0,0,0);
        return false;
    }
}


bool CharacterEntity2::checkCollision2()
{
    std::vector<std::vector<glm::vec3>> triangles = this->cube->getTriangleSet(this->position);
    if (triangles.size() == 0) {
        return 0;
    }
    //std::vector<Vertex> vertices = this->scene_object.meshes[0].vertices;
    bool colliding = false;

    for (int i = 0; i < triangles.size(); i++) {
        glm::vec3 v1 = triangles.at(i).at(0);
        glm::vec3 v2 = triangles.at(i).at(1);
        glm::vec3 v3 = triangles.at(i).at(2);
        // Plane trianglePlane(v1, v2, v3);
        // float t;
        // rayIntersectPlane(trianglePlane.normal,
        //              trianglePlane.origin, 
        //              A, capsuleNormal, t);
        // glm::vec3 tracePoint = A + capsuleNormal * t;
        // glm::vec3 referencePoint = closestPointOnTriangle(tracePoint, v1, v2, v3);
        // glm::vec3 sphereCenter = closestPointOnLineSegment(A, B, referencePoint);
        // this->collisionPackage.R3Position = sphereCenter;    
        // vec3 a = v1;
        // vec3 b = v2;
        // vec3 c = v3;
        //std::cout << to_string(this->top) << ", " << to_string(this->bottom) << std::endl;
        bool res = capsuleTriangleCollision(this->top, this->bottom, this->sphere_radius, v1, v2, v3);
        if (res == true){
            return true;
        }
    }

    // for (int i = 0; i < vertices.size();i+=3){
    //     glm::vec3 v1 = getWorldSpace(vertices[i].Position, this->object_model_mat);
    //     glm::vec3 v2 = getWorldSpace(vertices[i+1].Position, this->object_model_mat);
    //     glm::vec3 v3 = getWorldSpace(vertices[i+2].Position, this->object_model_mat);
    //     bool res = capsuleTriangleCollision(this->top, this->bottom, this->sphere_radius, v1, v2, v3);
    //     if (res == true){
    //         return true;
    //     }

    // }
    return false;
}

int CharacterEntity2::update(float dt)
{
    velocity = velocity * 0.7f;
    position += velocity * dt * movementSpeed;
    checkCollision2();

    //velocity += displacement_vec;



    // gravity step
    //velocity += vec3(0,-10.0f, 0) * dt * gravitySpeed;

    //checkCollision2();

    //velocity += displacement_vec;
    //position += velocity * dt * movementSpeed;


    float g = velocity.y;
    velocity = velocity * 0.7f;
    // keep gravity but slow velocity due to friction
    velocity.y = g;
    return 0;
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