#ifndef CONTACT_H
#define CONTACT_H

#include "../../../core/graphics/include/math_utils.h"

#include "body.h"
#include "joint.h"
#include "collisionSAT.h"

class Contact {
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

    Contact(RigidBody2 &hullA, RigidBody2 &hullB);
    int apply_impulse();
    int pre_step(float dt);
};

vector<Face2> get_most_parallel_face(const RigidBody2 &hullA, vec3 n1);
vector<Face2> get_most_antiparallel_face(const RigidBody2 &hullA, vec3 n1);
vector<Face2> get_orthogonal_faces(const RigidBody2 &hullA, vec3 n1);
vector<Contact> sphere_sphere_contacts(RigidBody2 &body_a,  RigidBody2 &body_b);
vector<Contact> sphere_tri_contacts(RigidBody2 &body_a,  RigidBody2 &body_b);
vector<Contact> sphere_box_contacts(RigidBody2 &body_a,  RigidBody2 &body_b);
vector<Contact> sphere_cylinder_contacts(RigidBody2 &body_a,  RigidBody2 &body_b);
vector<Contact> capsule_capsule_contacts(RigidBody2 &body_a,  RigidBody2 &body_b);
vector<Contact> capsule_sphere_contacts(RigidBody2 &body_a,  RigidBody2 &body_b);
vector<Contact> capsule_tri_contacts(RigidBody2 &body_a,  RigidBody2 &body_b);
vector<Contact> capsule_box_contacts(RigidBody2 &body_a,  RigidBody2 &body_b);
vector<Contact> capsule_cylinder_contacts(RigidBody2 &body_a,  RigidBody2 &body_b);
vector<Contact> box_box_contacts(RigidBody2 &body_a,  RigidBody2 &body_b);
vector<Contact> create_contacts(RigidBody2 &body_a,  RigidBody2 &body_b);
vector<Contact> create_minimal_contacts(vector<Contact> contacts);
glm::vec3 closest_point_on_capsule(glm::vec3 sphereCentre, RigidBody2 &body_b);
glm::vec3 closest_point_on_tri(glm::vec3 sphereCentre, vec3 p0, vec3 p1, vec3 p2);

int simulate(vector<RigidBody2> &aBodies, vector<Contact> &contacts,
            vector<SphericalJoint> &joints, float deltaTime);

#endif