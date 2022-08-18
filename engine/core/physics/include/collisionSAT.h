#ifndef COLLISIONSAT_H
#define COLLISIONSAT_H

#include "body.h"

extern float depth_epsilon;   

// find furthest along n
vec3 get_support(const RigidBody2 &hull, vec3 n);

class Query2 {
public:
    float best_distance;
    glm::vec2 best_index;
    glm::vec3 best_axis;
    int id;
    Query2(float _best_distance, glm::vec2 _best_index, glm::vec3 _best_axis) {
        this->best_distance = _best_distance;
        this->best_index = _best_index;
        this->best_axis = _best_axis;
        this->id = -1;
    }
};

// collision
bool sphere_box_collision(const RigidBody2 &hullA, vec3 closest_pt);
bool sphere_sphere_collision(const RigidBody2 &hullA, const RigidBody2 &hullB);
bool sphere_cylinder_collision(const RigidBody2 &body_a, vec3 closest_pt);
vector<Query2> SAT2(RigidBody2 &hullA, RigidBody2 &hullB);
vec3 closest_point_on_cylinder(vec3 p, const RigidBody2 &cylinder);
// Given point p, return point q on (or in) OBB b, closest to p
Query2 query_face_directions(RigidBody2 &hullA, RigidBody2 &hullB);
Query2 query_edge_directions(RigidBody2 &hullA, RigidBody2 &hullB);

#endif
