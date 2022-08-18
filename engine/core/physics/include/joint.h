#ifndef JOINT_H
#define JOINT_H

#include "body.h"

class SphericalJoint {
public:

    vec3 normal = vec3(0,1,0);
    vec3 collision_point;
    float penetration = 0;
    float bias = 0;
    float impulse_denominator_bitangent = 0.0f;
    float impulse_denominator_tangent = 0.0f;
    float impulse_denominator_normal = 0.0f;
    float Pn = 0;
    float Pt = 0;
    float Pbt = 0;
    RigidBody2 *body_a;
    RigidBody2 *body_b;

    vec3 anchor_a;
    vec3 anchor_b;

    vec3 anchor_a_ws;
    vec3 anchor_b_ws;

    float dt = 0.02f;

    unsigned int body_a_index;
    unsigned int body_b_index;

    bool unset = true;

    SphericalJoint(unsigned int indexA, vec3 anchorA,  unsigned int indexB,  vec3 anchorB);

    int setBodies(std::vector<RigidBody2> &aBodies);

    int apply_impulse();

    int pre_step(float dt, std::vector<RigidBody2> &aBodies);
};

#endif