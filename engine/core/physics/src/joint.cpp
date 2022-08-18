#include "joint.h"

SphericalJoint::SphericalJoint(unsigned int hullAIndex, vec3 anchorA,  unsigned int hullBIndex,  vec3 anchorB) {
    body_a_index = hullAIndex;
    body_b_index = hullBIndex;
    anchor_a = anchorA;
    anchor_b = anchorB;
    anchor_a_ws = anchorA;
    anchor_b_ws = anchorB;
    unset = true;
}

int SphericalJoint::setBodies(std::vector<RigidBody2> &aBodies) {

    unset = false;
    return 0;
}

int SphericalJoint::apply_impulse() {

    vec3 impulse_direction = this->normal;

    anchor_a_ws = body_a->orientation * this->anchor_a;
    anchor_b_ws = body_b->orientation * this->anchor_b;

    vec3 pa = body_a->cm_position + body_a->orientation * this->anchor_a;
    vec3 ra = pa - body_a->cm_position;
    vec3 va = body_a->cm_velocity + cross(body_a->angular_velocity, ra);
    vec3 pb = body_b->cm_position + body_b->orientation * this->anchor_b;
    vec3 rb = pb - body_b->cm_position;
    vec3 vb = body_b->cm_velocity + cross(body_b->angular_velocity, rb);

    vec3 rel_vel = vb - va;

    rel_vel = vb - va;

    this->normal = normalize(pb - pa);
    this->penetration = dot(pb - pa, this->normal);
    // float k_min_penetration = 0.05f;
    // float k_biasFactor = 0.2f;
    // if (this->penetration - k_min_penetration > 0) {
    //     this->bias = -k_biasFactor * 1.0 / this->dt * std::max(0.0f, this->penetration - k_min_penetration);
    // }

    float impulse_numerator = -(1.0f) * dot(rel_vel, this->normal);
    float dPn = 1.0 / this->impulse_denominator_normal * (impulse_numerator + this->bias);
    // float Pn0 = this->Pn;
    // this->Pn = std::max(Pn0 + dPn, 0.0f);
    // dPn = this->Pn - Pn0;
    vec3 impulse = impulse_direction *  dPn;

    // apply impulse to primary quantities
    body_a->cm_velocity -= impulse * body_a->one_over_mass;
    body_a->angular_momentum -= cross(ra, impulse);
    // compute affected auxillary quantities
    body_a->angular_velocity = body_a->inverse_world_inertia_tensor * body_a->angular_momentum;

    // apply impulse to primary quantities
    body_b->cm_velocity += impulse * body_b->one_over_mass;
    body_b->angular_momentum += cross(rb, impulse);
    // compute affected auxillary quantities
    body_b->angular_velocity = body_b->inverse_world_inertia_tensor * body_b->angular_momentum;
    return 0;
}

int SphericalJoint::pre_step(float dt, std::vector<RigidBody2> &aBodies) {

    body_a = &(aBodies.at(body_a_index));
    body_b = &(aBodies.at(body_b_index));

    this->dt = dt;
    vec3 impulse_direction = this->normal;

    vec3 pa = body_a->cm_position + body_a->orientation * this->anchor_a;
    vec3 ra = pa - body_a->cm_position;
    vec3 pb = body_b->cm_position + body_b->orientation * this->anchor_b;
    vec3 rb = pb - body_b->cm_position;

    //float k_min_penetration = 0.01f;
    float k_biasFactor = 0.2f;
    this->normal = normalize(pb - pa);
    this->penetration = dot(pb - pa, this->normal);

    this->bias = -k_biasFactor * 1.0 / dt * this->penetration;
    //float gamma = 0.1f;
    this->impulse_denominator_normal = dot(impulse_direction, impulse_direction)*(body_a->one_over_mass + body_b->one_over_mass) + dot(cross(body_a->inverse_world_inertia_tensor * cross(ra, impulse_direction), ra), impulse_direction) + dot(cross(body_b->inverse_world_inertia_tensor * cross(rb, impulse_direction), rb), impulse_direction);

    vec3 impulse = impulse_direction *  this->Pn;

    // apply impulse to primary quantities
    body_a->cm_velocity -= impulse * body_a->one_over_mass;
    body_a->angular_momentum -= cross(ra, impulse);
    // compute affected auxillary quantities
    body_a->angular_velocity = body_a->inverse_world_inertia_tensor * body_a->angular_momentum;

    // apply impulse to primary quantities
    body_b->cm_velocity += impulse * body_b->one_over_mass;
    body_b->angular_momentum += cross(rb, impulse);
    // compute affected auxillary quantities
    body_b->angular_velocity = body_b->inverse_world_inertia_tensor * body_b->angular_momentum;

    return 0;
}