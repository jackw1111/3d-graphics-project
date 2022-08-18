#include "contact.h"

Contact::Contact(RigidBody2 &hullA, RigidBody2 &hullB) {
    body_a = &hullA;
    body_b = &hullB;
    collision_point = body_a->cm_position;
}
int Contact::apply_impulse() {
    vec3 impulse_direction = this->normal;

    vec3 pa = this->collision_point;
    vec3 ra = pa - body_a->cm_position;
    vec3 va = body_a->cm_velocity + cross(body_a->angular_velocity, ra);
    vec3 pb = this->collision_point;
    vec3 rb = pb - body_b->cm_position;
    vec3 vb = body_b->cm_velocity + cross(body_b->angular_velocity, rb);

    vec3 velocity = vb - va;

    float impulse_numerator = dot(velocity, impulse_direction);
    float dPn = -1.0f / this->impulse_denominator_normal * (impulse_numerator + this->bias);
    float Pn0 = this->Pn;
    this->Pn = std::max(Pn0 + dPn, 0.0f);
    dPn = this->Pn - Pn0;
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

    pa = this->collision_point;
    ra = pa - body_a->cm_position;
    va = body_a->cm_velocity + cross(body_a->angular_velocity, ra);

    pb = this->collision_point;
    rb = pb - body_b->cm_position;
    vb = body_b->cm_velocity + cross(body_b->angular_velocity, rb);
    velocity = vb - va;

    vec3 tangent = formOrthogonalBasis(impulse_direction)[1];
    impulse_direction = tangent;
    impulse_numerator = -(1) * dot(velocity, impulse_direction);

    float friction = body_a->friction * body_b->friction;
    float dPt = 1.0f / this->impulse_denominator_tangent * impulse_numerator;
    // Compute friction impulse
    float maxPt = friction * this->Pn;
    // Clamp friction
    float oldTangentImpulse = this->Pt;
    this->Pt = clamp(oldTangentImpulse + dPt, -maxPt, maxPt);
    dPt = this->Pt - oldTangentImpulse;

    impulse = impulse_direction * dPt;

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

    pa = this->collision_point;
    ra = pa - body_a->cm_position;
    va = body_a->cm_velocity + cross(body_a->angular_velocity, ra);

    pb = this->collision_point;
    rb = pb - body_b->cm_position;
    vb = body_b->cm_velocity + cross(body_b->angular_velocity, rb);
    velocity = vb - va;

    vec3 bitangent = formOrthogonalBasis(this->normal)[2];
    impulse_direction = bitangent;
    impulse_numerator = -(1) * dot(velocity, impulse_direction);

    friction = body_a->friction * body_b->friction;
    float dPbt = 1.0f / this->impulse_denominator_bitangent * impulse_numerator;
    // Compute friction impulse
    float maxPbt = friction * this->Pn;
    // Clamp friction
    float oldBitangentImpulse = this->Pbt;
    this->Pbt = clamp(oldBitangentImpulse + dPbt, -maxPbt, maxPbt);
    dPbt = this->Pbt - oldBitangentImpulse;

    impulse = impulse_direction * dPbt;

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

int Contact::pre_step(float dt) {

    vec3 impulse_direction = this->normal;
    vec3 pa = this->collision_point;
    vec3 ra = pa - body_a->cm_position;

    vec3 pb = this->collision_point;
    vec3 rb = pb - body_b->cm_position;

    this->impulse_denominator_normal = dot(impulse_direction, impulse_direction)*(this->body_a->one_over_mass + this->body_b->one_over_mass) + dot(cross(this->body_a->inverse_world_inertia_tensor * cross(ra, impulse_direction), ra), impulse_direction) + dot(cross(this->body_b->inverse_world_inertia_tensor * cross(rb, impulse_direction), rb), impulse_direction);

    vec3 tangent = formOrthogonalBasis(this->normal)[1];
    impulse_direction = tangent;
    pa = this->collision_point;
    ra = pa - body_a->cm_position;

    pb = this->collision_point;
    rb = pb - body_b->cm_position;

    this->impulse_denominator_tangent = dot(impulse_direction, impulse_direction)*(this->body_a->one_over_mass + this->body_b->one_over_mass) + dot(cross(this->body_a->inverse_world_inertia_tensor * cross(ra, impulse_direction), ra), impulse_direction) + dot(cross(this->body_b->inverse_world_inertia_tensor * cross(rb, impulse_direction), rb), impulse_direction);
    
    vec3 bitangent = formOrthogonalBasis(this->normal)[2];
    impulse_direction = bitangent;

    pa = this->collision_point;
    ra = pa - body_a->cm_position;

    pb = this->collision_point;
    rb = pb - body_b->cm_position;

    this->impulse_denominator_bitangent = dot(impulse_direction, impulse_direction)*(this->body_a->one_over_mass + this->body_b->one_over_mass) + dot(cross(this->body_a->inverse_world_inertia_tensor * cross(ra, impulse_direction), ra), impulse_direction) + dot(cross(this->body_b->inverse_world_inertia_tensor * cross(rb, impulse_direction), rb), impulse_direction);
    

    vec3 va = body_a->cm_velocity + cross(body_a->angular_velocity, ra);
    vec3 vb = body_b->cm_velocity + cross(body_b->angular_velocity, rb);
    vec3 velocity = vb - va;
    float relative_velocity = dot(velocity, this->normal);

    float e = std::min(body_a->coefficient_of_restitution, body_b->coefficient_of_restitution);
    float k_min_penetration = 0.01f;
    float k_biasFactor = 0.2f;
    this->bias = k_biasFactor * 1.0f / dt * std::min(0.0f, this->penetration + k_min_penetration);
    // float slop = 0.5f;
    // float r = e * std::min(0.0f, relative_velocity - slop);
    //std::cout <<  e << ", " << e * dot(velocity, impulse_direction) << std::endl;

    float k_min_velocity = 1.0f;
    if (relative_velocity < -k_min_velocity) {
        if (e * relative_velocity < this->bias) {
            this->bias = e * relative_velocity;
        }
    }

    vec3 impulse = this->normal * this->Pn + tangent * this->Pt + bitangent * this->Pbt;
    // # apply impulse to primary quantities;
    body_a->cm_velocity -= impulse * body_a->one_over_mass;
    body_a->angular_momentum -= cross(ra, impulse);
    // compute affected auxillary quantities;
    body_a->angular_velocity = body_a->inverse_world_inertia_tensor * body_a->angular_momentum;

    // apply impulse to primary quantities;
    body_b->cm_velocity += impulse * body_b->one_over_mass;
    body_b->angular_momentum += cross(rb, impulse);
    // compute affected auxillary quantities;
    body_b->angular_velocity = body_b->inverse_world_inertia_tensor * body_b->angular_momentum;

    return 0;
}

glm::vec3 closest_point_on_capsule(glm::vec3 sphereCentre, RigidBody2 &body_b) {
    glm::vec3 closest_pt = closest_point_on_line_segment(body_b.cm_position - normalize(body_b.orientation[1]) * body_b.dX2, body_b.cm_position + normalize(body_b.orientation[1]) * body_b.dX2, sphereCentre);
    return closest_pt;
}

glm::vec3 closest_point_on_tri(glm::vec3 sphereCentre, vec3 p0, vec3 p1, vec3 p2) {
    vec3 planeNormal = normalize(cross(p1 - p0, p2 - p0));
    // if sphere intersects plane
    float dist = sphere_plane_collision(sphereCentre, planeNormal, p0);

    glm::vec3 possible_point0 = sphereCentre - planeNormal * dist; // projected sphere center on triangle plane

    bool inside = ray_triangle_collision(possible_point0, planeNormal, p0, p1, p2);
    vec3 point0(FLT_MAX, FLT_MAX, FLT_MAX);
    if (inside) {
        point0 = possible_point0;
    }
    // Edge 1:
    glm::vec3 point1 = closest_point_on_line_segment(p0, p1, sphereCentre);
    // Edge 2:
    glm::vec3 point2 = closest_point_on_line_segment(p1, p2, sphereCentre);
    // Edge 3:
    glm::vec3 point3 = closest_point_on_line_segment(p2, p0, sphereCentre);
    vector<vec3> points = {};
    if (point0 == vec3(FLT_MAX, FLT_MAX, FLT_MAX)) {
        vector<vec3> p= {point1, point2, point3};
        points = p;
    } else {
        vector<vec3> p = {point0, point1, point2, point3};
        points = p;
    }
    dist = FLT_MAX;
    vec3 best_point;
    for (unsigned int i = 0; i < points.size(); i++) {
        vec3 current_point = points.at(i);
        float current_dist = glm::distance(current_point, sphereCentre);
        if (current_dist < dist) {
            best_point = current_point;
            dist = current_dist;
        }
    }

    return best_point;
}
vector<Face2> get_most_parallel_face(const RigidBody2 &hullA, vec3 n1) {
    float d = -FLT_MAX;
    vector<Face2> parallel_faces = {};
    // return most anti-parallel face on hull to n
    for (unsigned i = 0; i < hullA.faces.size(); i++) {
        Face2 f = hullA.faces.at(i);
        vec3 n2 = f.normal_ws;
        float alignment = dot(normalize(n1), normalize(n2));
        if (alignment >= d) {
            parallel_faces.push_back(f);
            d = alignment;
        }
    }
    return {parallel_faces.at(parallel_faces.size()-1)};
}

vector<Face2> get_most_antiparallel_face(const RigidBody2 &hullA, vec3 n1) {
    float d = FLT_MAX;
    vector<Face2> antiparallel_faces = {};
    // return most anti-parallel face on hull to n
    for (unsigned i = 0; i < hullA.faces.size(); i++) {
        Face2 f = hullA.faces.at(i);
        vec3 n2 = f.normal_ws;
        float alignment = dot(normalize(n1), normalize(n2));
        if (alignment <= d) {
            antiparallel_faces.push_back(f);
            d = alignment;
        }
    }
    return {antiparallel_faces.at(antiparallel_faces.size()-1)};
}

vector<Face2> get_orthogonal_faces(const RigidBody2 &hullA, vec3 n1) {

    vector<Face2> orthogonal_faces = {};
    // return most anti-parallel face on hull to n
    for (unsigned i = 0; i < hullA.faces.size(); i++) {
        Face2 f = hullA.faces.at(i);
        vec3 n2 = f.normal_ws;
        float alignment = dot(normalize(n1), normalize(n2));
        if (fabs(alignment) < 0.98f){
            orthogonal_faces.push_back(f);
        }
    }
    return orthogonal_faces;
}

vector<Contact> sphere_sphere_contacts(RigidBody2 &body_a,  RigidBody2 &body_b) {
    vector<Contact> contacts = {};

    if (sphere_sphere_collision(body_a, body_b)) {
        vec3 normal = normalize(body_b.cm_position - body_a.cm_position);
        vec3 p_a = body_a.cm_position + normal * body_a.dX2;
        vec3 p_b = body_b.cm_position - normal * body_b.dX2;
        vec3 collision_point = (p_a + p_b ) * 0.5f;
        float penetration = glm::distance(body_a.cm_position, body_b.cm_position) - (body_a.dX2 + body_b.dX2);

        if (penetration < depth_epsilon) {
            Contact contact(body_a, body_b);
            contact.penetration = penetration;
            contact.normal = normalize(normal);
            contact.collision_point = collision_point;
            contacts.push_back(contact);
        }
    }
    return contacts;
}
vector<Contact> sphere_tri_contacts(RigidBody2 &body_a,  RigidBody2 &body_b) {
    vector<Contact> contacts = {};
    vec3 p0 = body_b.faces.at(0).verts_ws.at(0);
    vec3 p1 = body_b.faces.at(0).verts_ws.at(1);
    vec3 p2 = body_b.faces.at(0).verts_ws.at(2);
    vec3 closest_pt = closest_point_on_tri(body_a.cm_position, p0, p1, p2);
    if (sphere_box_collision(body_a, closest_pt)) {
        vec3 normal = normalize(closest_pt - body_a.cm_position);
        vec3 p_a = body_a.cm_position + normal * body_a.dX2;
        vec3 p_b = closest_pt;
        float penetration = glm::distance(body_a.cm_position, closest_pt) - body_a.dX2;
        vec3 collision_point = (p_a + p_b) * 0.5f;
        if (penetration < depth_epsilon) {
            Contact contact(body_a, body_b);
            contact.penetration = penetration;
            contact.normal = normal;
            contact.collision_point = collision_point;
            contacts.push_back(contact);
        }
    }
    return contacts;
}


vector<Contact> sphere_box_contacts(RigidBody2 &body_a,  RigidBody2 &body_b) {
    vector<Contact> contacts = {};

    vec3 closest_pt = closest_point_on_obb(body_a.cm_position, body_b.dX2, body_b.dY2, body_b.dZ2, body_b.cm_position, body_b.orientation);
    if (sphere_box_collision(body_a, closest_pt)) {
        vec3 normal = normalize(closest_pt - body_a.cm_position);
        vec3 p_a = body_a.cm_position + normal * body_a.dX2;
        vec3 p_b = closest_pt;
        float penetration = glm::distance(body_a.cm_position, closest_pt) - body_a.dX2;
        vec3 collision_point = (p_a + p_b) * 0.5f;
        if (penetration < depth_epsilon) {
            Contact contact(body_a, body_b);
            contact.penetration = penetration;
            contact.normal = normal;
            contact.collision_point = collision_point;
            contacts.push_back(contact);
        }
    }
    return contacts;
}


vector<Contact> sphere_cylinder_contacts(RigidBody2 &body_a,  RigidBody2 &body_b) {
    vector<Contact> contacts = {};

    vec3 closest_pt = closest_point_on_cylinder(body_a.cm_position, body_b);
    if (sphere_cylinder_collision(body_a, closest_pt)) {
        vec3 normal = normalize(closest_pt - body_a.cm_position);
        float penetration = glm::distance(body_a.cm_position, closest_pt) - body_a.dX2;
        vec3 collision_point = closest_pt;
        if (penetration < depth_epsilon) {
            Contact contact(body_a, body_b);
            contact.penetration = penetration;
            contact.normal = normal;
            contact.collision_point = collision_point;
            contacts.push_back(contact);
        }
    }

    return contacts;
}

vector<Contact> capsule_capsule_contacts(RigidBody2 &body_a,  RigidBody2 &body_b) {
    vector<Contact> contacts = {};

    vec3 l0 = body_a.cm_position - normalize(body_a.orientation[1]) * body_a.dY2;
    vec3 l1 = body_a.cm_position + normalize(body_a.orientation[1]) * body_a.dY2;

    vec3 l2 = body_b.cm_position - normalize(body_b.orientation[1]) * body_b.dY2;
    vec3 l3 = body_b.cm_position + normalize(body_b.orientation[1]) * body_b.dY2;

    vec2 mu = closestPointsOnLineSegments(l0, l1, l2, l3);
    float mu_a = mu.x;
    float mu_b = mu.y;
    vec3 pt1 = l0 + (l1 - l0) * mu_a;
    vec3 pt2 = l2 + (l3 - l2) * mu_b;

    float penetration = glm::distance(pt1, pt2) - (body_a.dX2 + body_b.dX2);

    if (penetration < depth_epsilon) {
        Contact contact(body_a, body_b);
        contact.penetration = penetration;
        vec3 normal = normalize(pt2 - pt1);
        contact.normal = normalize(normal);
        vec3 p_a = pt1 + normal * body_a.dX2;
        vec3 p_b = pt2 - normal * body_b.dX2;
        vec3 collision_point = (p_a + p_b ) * 0.5f;
        contact.collision_point = collision_point;
        contacts.push_back(contact);
    }
    return contacts;
}
vector<Contact> capsule_sphere_contacts(RigidBody2 &body_a,  RigidBody2 &body_b) {
    vec3 closest_pt = closest_point_on_capsule(body_b.cm_position, body_a);
    vector<Contact> contacts = {};
    vec3 tmp = body_a.cm_position;
    body_a.cm_position = closest_pt;
    if (sphere_sphere_collision(body_a, body_b)) {
        vec3 normal = normalize(body_b.cm_position - body_a.cm_position);
        vec3 p_a = body_a.cm_position + normal * body_a.dX2;
        vec3 p_b = body_b.cm_position - normal * body_b.dX2;
        vec3 collision_point = (p_a + p_b ) * 0.5f;
        float penetration = glm::distance(body_a.cm_position, body_b.cm_position) - (body_a.dX2 + body_b.dX2);

        if (penetration < depth_epsilon) {
            Contact contact(body_a, body_b);
            contact.penetration = penetration;
            contact.normal = normalize(normal);
            contact.collision_point = collision_point;
            contacts.push_back(contact);
        }
    }
    body_a.cm_position = tmp;
    return contacts;
}

bool intersectPlane(const vec3 &n, const vec3 &p0, const vec3 &l0, const vec3 &l, float &t) 
{ 
    // assuming vectors are all normalized
    float denom = dot(n, l); 
    //if (denom > 1e-6) { 
    vec3 p0l0 = p0 - l0; 
    t = dot(p0l0, n) / denom; 
    return true; 
    //} 
 
    return false; 
} 

vector<Contact> capsule_tri_contacts(RigidBody2 &body_a,  RigidBody2 &body_b) {
    vector<Contact> contacts = {};

    float minDist = FLT_MAX;
    vec3 sphere_point;
    vec3 tri_point;
    vec3 closest_pt;
    float current_dist;

    vec3 v0 = body_b.faces.at(0).verts_ws.at(0);
    vec3 v1 = body_b.faces.at(0).verts_ws.at(1);
    vec3 v2 = body_b.faces.at(0).verts_ws.at(2);

    vec3 l0 = body_a.cm_position - normalize(body_a.orientation[1]) * body_a.dX2;
    vec3 l1 = body_a.cm_position + normalize(body_a.orientation[1]) * body_a.dX2;

    closest_pt = closest_point_on_tri(l0, v0, v1, v2);
    current_dist = glm::distance(l0, closest_pt);
    if (current_dist < minDist) {
        minDist = current_dist;
        tri_point = closest_pt;
        sphere_point = l0;
    }

    closest_pt = closest_point_on_tri(l1, v0, v1, v2);
    current_dist = glm::distance(l1, closest_pt);
    if (current_dist < minDist) {
        minDist = current_dist;
        tri_point = closest_pt;
        sphere_point = l1;
    }

    vec2 mu = closestPointsOnLineSegments(v0, v1, l0, l1);
    float mu_a = mu.x;
    float mu_b = mu.y;
    vec3 edgept1 = v0 + (v1 - v0) * mu_a;
    vec3 lspt1 = l0 + (l1 - l0) * mu_b;
    current_dist = glm::distance(edgept1, lspt1);
    if (current_dist < minDist) {
        minDist = current_dist;
        tri_point = edgept1;
        sphere_point = lspt1;
    }

    mu = closestPointsOnLineSegments(v1, v2, l0, l1);
    mu_a = mu.x;
    mu_b = mu.y;
    vec3 edgept2 = v1 + (v2 - v1) * mu_a;
    vec3 lspt2 = l0 + (l1 - l0) * mu_b;
    current_dist = glm::distance(edgept2, lspt2);
    if (current_dist < minDist) {
        minDist = current_dist;
        tri_point = edgept2;
        sphere_point = lspt2;
    }

    mu = closestPointsOnLineSegments(v2, v0, l0, l1);
    mu_a = mu.x;
    mu_b = mu.y;
    vec3 edgept3 = v2 + (v0 - v2) * mu_a;
    vec3 lspt3 = l0 + (l1 - l0) * mu_b;
    current_dist = glm::distance(edgept3, lspt3);
    if (current_dist < minDist) {
        minDist = current_dist;
        tri_point = edgept3;
        sphere_point = lspt3;
    }

    float penetration = glm::distance(sphere_point, tri_point) - body_a.dX2;

    if (penetration < depth_epsilon) {
        vec3 normal = normalize(tri_point - sphere_point);
        vec3 p_a = sphere_point + normal * body_a.dX2;
        vec3 p_b = tri_point;
        vec3 collision_point = (p_a + p_b) * 0.5f;
        if (penetration < depth_epsilon) {
            Contact contact(body_a, body_b);
            contact.penetration = penetration;
            contact.normal = normal;
            contact.collision_point = collision_point;
            contacts.push_back(contact);
        }
    }

    return contacts;  
}
vector<Contact> capsule_box_contacts(RigidBody2 &body_a,  RigidBody2 &body_b) {
    vector<Contact> cylinder_contacts = box_box_contacts(body_a, body_b);
    if (cylinder_contacts.size() == 0) {
        vec3 tmp = body_a.cm_position;
        body_a.cm_position = body_a.cm_position + body_a.orientation[1] * body_a.dX2;
        vector<Contact> sphere1_contacts = sphere_box_contacts(body_a, body_b);
        body_a.cm_position = tmp;
        if (sphere1_contacts.size() == 0) {
            vec3 tmp = body_a.cm_position;
            body_a.cm_position = body_a.cm_position - body_a.orientation[1] * body_a.dX2;
            vector<Contact> sphere2_contacts = sphere_box_contacts(body_a, body_b);
            body_a.cm_position = tmp;
            return sphere2_contacts;
        } else {
            return sphere1_contacts;
        }
    } else {
        return cylinder_contacts;
    }
}
vector<Contact> capsule_cylinder_contacts(RigidBody2 &body_a,  RigidBody2 &body_b) {
    vec3 closest_pt = closest_point_on_capsule(body_b.cm_position, body_a);
    RigidBody2 sphere = body_a;
    sphere.cm_position = closest_pt;
    return {};//sphere_cylinder_contacts(body_a, sphere);
}

vector<Contact> box_box_contacts(RigidBody2 &body_a,  RigidBody2 &body_b) {
    
    vector<Contact> contacts = {};

    vector<Query2> queries = SAT2(body_a, body_b);

    vector<vec3> contact_points = {};
    vector<vec3> contact_normals = {};
    vector<float> contact_penetrations = {};
    for (unsigned i = 0; i < queries.size(); i++) {
        Query2 *query = &queries.at(i);

        float colliding = query->best_distance;
        vec2 best_index =  query->best_index;
        vec3 best_axis = query->best_axis;

        int query_no = query->id;

        RigidBody2 *reference_hull;
        RigidBody2 *incident_hull;

        if (colliding < 0.0f) {
            if (query_no == 1) {
                // reference object A, incident object B
                reference_hull = &body_a;
                incident_hull = &body_b;
            }
            if (query_no == 2) {
                // reference object B, incident object A
                reference_hull = &body_b;
                incident_hull = &body_a;
            } else {
                // edge
            }

            if (query_no >= 1) {
                vector<Face2> reference_faces = get_most_parallel_face(*reference_hull, best_axis);
                for (unsigned i = 0; i < reference_faces.size(); i++) {
                    Face2 *reference_face = &reference_faces.at(i);
                    vector<Face2> incident_faces = get_most_antiparallel_face(*incident_hull, best_axis);
                    vector<Face2> side_faces = get_orthogonal_faces(*reference_hull, reference_face->normal_ws);
                    for (unsigned j = 0; j < incident_faces.size(); j++) {
                        Face2 *incident_face = &incident_faces.at(i);
                        vector<vec3> clipped_polygon = incident_face->verts_ws;
                        // clip incident face against side planes of reference face
                        for (unsigned k = 0; k < side_faces.size(); k++) {
                            Face2 f = side_faces.at(k);
                            clipped_polygon = clipPolygonToPlane(clipped_polygon, f.verts_ws.at(0), f.normal_ws);
                        }
                        // keep all points below reference face plane
                        for (unsigned k = 0; k < clipped_polygon.size(); k++) {
                            vec3 possible_contact_point = clipped_polygon.at(k);
                            //if possible_contact_point not in incident_face.verts_ws:
                            if (pointPlaneDistance2(possible_contact_point, reference_face->position_ws, reference_face->normal_ws) < 0.0f) {
                                contact_points.push_back(possible_contact_point); 
                                if (dot(best_axis, possible_contact_point - body_a.cm_position) < 0.0f) {
                                    best_axis = best_axis * -1.0f;
                                }
                                contact_normals.push_back(best_axis);
                                contact_penetrations.push_back(colliding);      
                            }
                        }
                    }
                }
            } else {
                // edge collision
                int i = best_index.x;
                int j = best_index.y;
                Edge2 edge_a = body_a.edges.at(i);
                Edge2 edge_b = body_b.edges.at(j);
                vec2 mu = lineSegmentIntersection(edge_a.v1_ws, edge_a.v2_ws, edge_b.v1_ws, edge_b.v2_ws);
                float mu_a = mu.x;
                float mu_b = mu.y;
                bool valid_intersection = mu_a >= 0.0f && mu_a <= 1.0f && mu_b >= 0.0f && mu_b <= 1.0f;
                if (valid_intersection) {
                    vec3 pt1 = edge_a.v1_ws + (edge_a.v2_ws - edge_a.v1_ws) * mu_a;
                    vec3 pt2 = edge_b.v1_ws + (edge_b.v2_ws - edge_b.v1_ws) * mu_b;
                    vec3 normal = normalize(cross(edge_a.v2_ws - edge_a.v1_ws, edge_b.v2_ws - edge_b.v1_ws));
                    vec3 contact_point = (pt1 + pt2)*0.5f;

                    if (dot(normal, contact_point - body_a.cm_position) < 0.0f) {
                        normal = normal * -1.0f;
                    }
                    contact_points.push_back(contact_point);
                    contact_normals.push_back(normal);
                    contact_penetrations.push_back(colliding);
                }
                
            }
        }
    }

    for (unsigned int i = 0; i < contact_points.size(); i++)  {
        vec3 p = contact_points.at(i);
        vec3 normal = contact_normals.at(i);
        Contact contact(body_a, body_b);
        contact.penetration = contact_penetrations.at(i);
        contact.normal = normalize(normal);
        contact.collision_point = p;
        contacts.push_back(contact);
    }
    return contacts;
}

vector<Contact> create_contacts(RigidBody2 &body_a,  RigidBody2 &body_b) {

    RigidBody2 *temp;
    RigidBody2 *p_body_a = &body_a;
    RigidBody2 *p_body_b = &body_b;

    if (p_body_a->one_over_mass == 0.0f && p_body_b->one_over_mass == 0.0f) {
        return {};
    }

    if (p_body_a->id == PLANE && p_body_b->id == PLANE) {
        return {};
    }

    if (p_body_a->id == TRIANGLE && p_body_b->id == TRIANGLE) {
        return {};
    }

    //broad phase
    // if (!AABBOverlap(p_body_a->aabb_min_ws, p_body_a->aabb_max_ws, p_body_b->aabb_min_ws, p_body_b->aabb_max_ws)) {
    //     return {};
    // }
    
    if ((p_body_a->id == PLANE && p_body_b->id == SPHERE) || (p_body_a->id == SPHERE && p_body_b->id == PLANE)) {
        if (p_body_a->id == SPHERE && p_body_b->id == PLANE) {
            temp = p_body_a;
            p_body_a = p_body_b;
            p_body_b = temp;
        }
        vec3 normal = p_body_a->faces.at(0).normal_ws;
        vec3 position = p_body_b->cm_position - normal * p_body_b->dX2;
        float penetration = dot(position, normal);
        if (penetration < depth_epsilon) { 
            Contact contact(*p_body_a, *p_body_b);
            contact.penetration = penetration;
            contact.normal = normalize(normal);
            contact.collision_point = position;
            return {contact};
        }
    }

    if ((p_body_a->id == PLANE && p_body_b->id == CAPSULE) || (p_body_a->id == CAPSULE && p_body_b->id == PLANE)) {
        if (p_body_a->id == CAPSULE && p_body_b->id == PLANE) {
            temp = p_body_a;
            p_body_a = p_body_b;
            p_body_b = temp;
        }
        vec3 l0 = body_b.cm_position - normalize(body_b.orientation[1]) * body_b.dY2;
        vec3 l1 = body_b.cm_position + normalize(body_b.orientation[1]) * body_b.dY2;
        vec3 dir = normalize(l1 - l0);
        // get the closest line segment end point to plane
        float d0 = pointPlaneDistance2(l0, body_a.faces.at(0).verts_ws.at(0), body_a.faces.at(0).normal_ws);
        float d1 = pointPlaneDistance2(l1, body_a.faces.at(0).verts_ws.at(0), body_a.faces.at(0).normal_ws);
        vec3 point0 = l0 - body_a.faces.at(0).normal_ws * d0;
        vec3 point1 = l1 - body_a.faces.at(0).normal_ws * d1;
        vec3 point;
        vec3 l;
        // get projected point in plane
        if (d0 < d1) {
            point = point0;
            l = l0;
        } else {
            point = point1;
            l = l1;
        }
        vec3 normal = body_a.faces.at(0).normal_ws;
        float d = dot(l - point, normal);
        vec3 collision_point = l - normal * d;
        float penetration = d - body_b.dX2;
        if (penetration < depth_epsilon) { 
            Contact contact(*p_body_a, *p_body_b);
            contact.penetration = penetration;
            contact.collision_point = collision_point;
            contact.normal = normal;
            return {contact};
        }
    }

    if ((p_body_a->id == PLANE && p_body_b->id == BOX) || (p_body_a->id == BOX && p_body_b->id == PLANE)) {
        if (p_body_a->id == BOX && p_body_b->id == PLANE) {
            temp = p_body_a;
            p_body_a = p_body_b;
            p_body_b = temp;
        }
        std::vector<Contact> contacts = {};
        for (unsigned int i = 0; i < p_body_b->number_of_bounding_vertices; i++) {
            vec3 position = p_body_b->a_bounding_vertices.at(i);
            vec3 normal = p_body_a->faces.at(0).normal_ws;
            float penetration = dot(position, normal);
            if (penetration < depth_epsilon) { 
                Contact contact(*p_body_a, *p_body_b);
                contact.penetration = penetration;
                contact.normal = normalize(normal);
                contact.collision_point = position;
                contacts.push_back(contact);
            }
        }
        return contacts;
    }

    if (p_body_a->id == SPHERE && p_body_b->id == SPHERE) {
        return sphere_sphere_contacts(*p_body_a, *p_body_b);
    }

    if ((p_body_a->id == SPHERE && p_body_b->id == BOX) || (p_body_a->id == BOX && p_body_b->id == SPHERE)) {

        if (p_body_a->id == BOX && p_body_b->id == SPHERE) {
            temp = p_body_a;
            p_body_a = p_body_b;
            p_body_b = temp;
        }
        return sphere_box_contacts(*p_body_a, *p_body_b);
    }

    if ((p_body_a->id == SPHERE && p_body_b->id == TRIANGLE) || (p_body_a->id == TRIANGLE && p_body_b->id == SPHERE)) {

        if (p_body_a->id == TRIANGLE && p_body_b->id == SPHERE) {
            temp = p_body_a;
            p_body_a = p_body_b;
            p_body_b = temp;
        }
        return sphere_tri_contacts(*p_body_a, *p_body_b);
    }

    if ((p_body_a->id == SPHERE && p_body_b->id == CYLINDER) || (p_body_a->id == CYLINDER && p_body_b->id == SPHERE)) {

        if (p_body_a->id == CYLINDER && p_body_b->id == SPHERE) {
            temp = p_body_a;
            p_body_a = p_body_b;
            p_body_b = temp;
        }
        return sphere_cylinder_contacts(*p_body_a, *p_body_b);
    }

    if (body_a.id == BOX && body_b.id == BOX) {
        return box_box_contacts(*p_body_a, *p_body_b);
    }

    if ((body_a.id == BOX && body_b.id == TRIANGLE) || (body_a.id == TRIANGLE &&  body_b.id == BOX)) {
        if (p_body_a->id == TRIANGLE && p_body_b->id == BOX) {
            temp = p_body_a;
            p_body_a = p_body_b;
            p_body_b = temp;
        }

        return box_box_contacts(*p_body_a, *p_body_b);
    }

    if (body_a.id == CYLINDER && body_b.id == CYLINDER) {
        return box_box_contacts(*p_body_a, *p_body_b);
    }

    if ((body_a.id == CYLINDER && body_b.id == BOX) || (body_a.id == BOX &&  body_b.id == CYLINDER)) {
        if (p_body_a->id == BOX && p_body_b->id == CYLINDER) {
            temp = p_body_a;
            p_body_a = p_body_b;
            p_body_b = temp;
        }
        return box_box_contacts(*p_body_a, *p_body_b);
    }


    if ((body_a.id == CYLINDER && body_b.id == TRIANGLE) || (body_a.id == TRIANGLE &&  body_b.id == CYLINDER)) {
        if (p_body_a->id == TRIANGLE && p_body_b->id == CYLINDER) {
            temp = p_body_a;
            p_body_a = p_body_b;
            p_body_b = temp;
        }
        return box_box_contacts(*p_body_a, *p_body_b);
    }

    // capsule
    // if ((p_body_a->id == PLANE && p_body_b->id == CAPSULE) || (p_body_a->id == CAPSULE && p_body_b->id == PLANE)) {
    //     if (p_body_a->id == CAPSULE && p_body_b->id == PLANE) {
    //         temp = p_body_a;
    //         p_body_a = p_body_b;
    //         p_body_b = temp;
    //     }
    //     vec3 normal = p_body_a->faces.at(0).normal_ws;
    //     vec3 position = p_body_b->cm_position - normal * p_body_b->dX2;
    //     float penetration = dot(position, normal);
    //     if (penetration < depth_epsilon) { 
    //         Contact contact(*p_body_a, *p_body_b);
    //         contact.penetration = penetration;
    //         contact.normal = normalize(normal);
    //         contact.collision_point = position;
    //         return {contact};
    //     }
    // }

    if (p_body_a->id == CAPSULE && p_body_b->id == CAPSULE) {
        return capsule_capsule_contacts(*p_body_a, *p_body_b);
    }

    if ((p_body_a->id == CAPSULE && p_body_b->id == SPHERE) || (p_body_a->id == SPHERE && p_body_b->id == CAPSULE)) {

        if (p_body_a->id == SPHERE && p_body_b->id == CAPSULE) {
            temp = p_body_a;
            p_body_a = p_body_b;
            p_body_b = temp;
        }
        return capsule_sphere_contacts(*p_body_a, *p_body_b);
    }
    if ((p_body_a->id == CAPSULE && p_body_b->id == BOX) || (p_body_a->id == BOX && p_body_b->id == CAPSULE)) {

        if (p_body_a->id == BOX && p_body_b->id == CAPSULE) {
            temp = p_body_a;
            p_body_a = p_body_b;
            p_body_b = temp;
        }
        return capsule_box_contacts(*p_body_a, *p_body_b);
    }

    if ((p_body_a->id == CAPSULE && p_body_b->id == TRIANGLE) || (p_body_a->id == TRIANGLE && p_body_b->id == CAPSULE)) {

        if (p_body_a->id == TRIANGLE && p_body_b->id == CAPSULE) {
            temp = p_body_a;
            p_body_a = p_body_b;
            p_body_b = temp;
        }
        return capsule_tri_contacts(*p_body_a, *p_body_b);
    }

    if ((p_body_a->id == CAPSULE && p_body_b->id == CYLINDER) || (p_body_a->id == CYLINDER && p_body_b->id == CAPSULE)) {

        if (p_body_a->id == CYLINDER && p_body_b->id == CAPSULE) {
            temp = p_body_a;
            p_body_a = p_body_b;
            p_body_b = temp;
        }
        return capsule_box_contacts(*p_body_a, *p_body_b);
    }

    return {};
}

vector<Contact> create_minimal_contacts(vector<Contact> contacts) {

    vector<Contact> minimal_contacts = {};

    // first contact
    Contact *deepest_contact;
    float deepest_penetration = FLT_MAX;
    bool foundcontact1 = false;
    for (unsigned int i = 0; i < contacts.size(); i++)  {
        Contact *contact = &contacts.at(i);

        if (contact->penetration < deepest_penetration) {
            deepest_contact = contact;
            deepest_penetration = contact->penetration;
            foundcontact1 = true;
        }
    }   

    // second contact
    float max_dist = -FLT_MAX;
    Contact *furthest_contact1;
    bool foundcontact2 = false;
    for (unsigned int i = 0; i < contacts.size(); i++)  {
        Contact *contact = &contacts.at(i);
        float current_dist = glm::distance(contact->collision_point, deepest_contact->collision_point);
        if (current_dist > max_dist) {
            furthest_contact1 = contact;
            max_dist = current_dist;
            foundcontact2 = true;
        }
    }

    // third contact
    max_dist = -FLT_MAX;
    Contact *furthest_contact2;
    bool foundcontact3 = false;
    for (unsigned int i = 0; i < contacts.size(); i++)  {
        Contact *contact = &contacts.at(i);
        float u = pointLineIntersection(contact->collision_point, furthest_contact1->collision_point, deepest_contact->collision_point);
        vec3 p = furthest_contact1->collision_point + (deepest_contact->collision_point-furthest_contact1->collision_point) * u;
        float current_dist = glm::distance(p, contact->collision_point);
        if (current_dist > max_dist) {
            furthest_contact2 = contact;
            max_dist = current_dist;
            foundcontact3 = true;
        }
    }        

    // fourth contact
    max_dist = -FLT_MAX;
    Contact *furthest_contact3;
    bool foundcontact4 = false;
    for (unsigned int i = 0; i < contacts.size(); i++)  {
        Contact *contact = &contacts.at(i);
        float d = distPointToTriangle(contact->collision_point, deepest_contact->collision_point, furthest_contact1->collision_point, furthest_contact2->collision_point);
        if (d > max_dist) {
            furthest_contact3 = contact;
            max_dist = d;
            foundcontact4 = true;
        }
    }        

    if (foundcontact1) {
        minimal_contacts.push_back(*deepest_contact);
    }
    if (foundcontact2) {
        minimal_contacts.push_back(*furthest_contact1);
    }
    if (foundcontact3) {
        minimal_contacts.push_back(*furthest_contact2);
    }
    if (foundcontact4) {
        minimal_contacts.push_back(*furthest_contact3);
    }

    return minimal_contacts;    
}

float t = 0.0f;
float dt = 0.02f;

float accumulator = 0.0f;

mat3 emptymat3(vec3(0,0,0),vec3(0,0,0),vec3(0,0,0));

extern TriOctree octree;

int simulate(vector<RigidBody2> &aBodies, vector<Contact> &contacts, vector<SphericalJoint> &joints, float deltaTime) {
    vector<RigidBody2*> dynamicBodies = {};
    vector<int> staticBodies = {};

    for (unsigned int i = 0; i < aBodies.size(); i++) {
        if (aBodies.at(i).id != TRIANGLE) {
            if (aBodies.at(i).one_over_mass == 0.0f && aBodies.at(i).inverse_body_inertia_tensor == emptymat3 && aBodies.at(i).cm_velocity == vec3(0,0,0)) {
                staticBodies.push_back(aBodies.at(i).octreeID);
            } else {
                dynamicBodies.push_back(&aBodies.at(i));
            }
        }
    }

    for (unsigned int i = 0; i < aBodies.size(); i++) {
        RigidBody2 *body = &aBodies.at(i);
        if (body->id != TRIANGLE) {
            body->update();
        }
    }

    // float frameTime = deltaTime;
    // if ( frameTime > 0.25f ) {
    //     frameTime = 0.25f;
    // }
    // accumulator += frameTime;

    // while ( accumulator >= dt )
    // {

	    for (unsigned int i = 0; i < dynamicBodies.size(); i++) {
	        RigidBody2 *body = dynamicBodies.at(i);

	        if (body->id == TRIANGLE) {
	            continue;
	        }
            // to do get the unique_set of triangle ids
            vector<int> tri_ids = {};
            tri_ids = get_triangle_set(octree, body->a_bounding_vertices, tri_ids);
            body->triIDs = tri_ids;
	        body->integrate_forces(body->cm_force, deltaTime);
	    }

	    vector<Contact> current_contacts = {};

	    for (unsigned int i = 0; i < dynamicBodies.size(); i++) {
	        RigidBody2 *body_a = dynamicBodies.at(i);
	        for (unsigned int j = i + 1; j < dynamicBodies.size(); j++) {
	            RigidBody2 *body_b = dynamicBodies.at(j);
	            if (body_a->id == TRIANGLE && body_b->id == TRIANGLE) {
	                continue;
	            }
	            vector<Contact> all_contacts = create_contacts(*body_a, *body_b);
	            vector<Contact> minimal_contacts = {};
	            if (all_contacts.size() > 4) {
	                vector<Contact> minimal_contacts = create_minimal_contacts(all_contacts);
	                current_contacts.insert( current_contacts.end(), minimal_contacts.begin(), minimal_contacts.end() );
	            } else {
	                current_contacts.insert( current_contacts.end(), all_contacts.begin(), all_contacts.end() );
	            }
	        }
	    }

        for (unsigned int i = 0; i < dynamicBodies.size(); i++) {
            RigidBody2 *body_a = dynamicBodies.at(i);
            vector<int> objectIDs = {};
            objectIDs.insert(objectIDs.end(), staticBodies.begin(), staticBodies.end());
            objectIDs.insert(objectIDs.end(), body_a->triIDs.begin(), body_a->triIDs.end());

            for (unsigned int j = 0; j < objectIDs.size(); j++) {
                RigidBody2 *body_b = &aBodies.at(objectIDs.at(j));

                vector<Contact> all_contacts = create_contacts(*body_a, *body_b);
                vector<Contact> minimal_contacts = {};
                if (all_contacts.size() > 4) {
                    vector<Contact> minimal_contacts = create_minimal_contacts(all_contacts);
                    current_contacts.insert( current_contacts.end(), minimal_contacts.begin(), minimal_contacts.end() );
                } else {
                    current_contacts.insert( current_contacts.end(), all_contacts.begin(), all_contacts.end() );
                }
            }
        }

	    for (unsigned int i = 0; i < current_contacts.size(); i++) {
	        Contact *contact = &current_contacts.at(i);
	        for (unsigned int j = 0; j < contacts.size(); j++) {
	            Contact *old_contact = &contacts.at(j);
	            if (glm::distance(contact->collision_point, old_contact->collision_point) < 0.001) {
	                contact->Pn = old_contact->Pn;
	                contact->Pt = old_contact->Pt;
	                contact->Pbt = old_contact->Pbt;
	                contact->penetration = old_contact->penetration;
	                contact->normal = old_contact->normal;
	                break;
	            }
	        }
	    }
	    contacts = current_contacts;

	    for (unsigned int i = 0; i < contacts.size(); i++) {
	        Contact *contact = &contacts.at(i);
	        contact->pre_step(deltaTime);
	    }

	    unsigned int iterations = 5;
	    for (unsigned int i = 0; i < iterations; i++) {
	        for (unsigned int j = 0; j < contacts.size(); j++) {
	            Contact *contact = &contacts.at(j);
	            contact->apply_impulse();
	        }
	    }
	    for (unsigned int i = 0; i < joints.size(); i++) {
	        SphericalJoint *joint = &joints.at(i);
	        joint->pre_step(deltaTime, aBodies);
	    }

	    for (unsigned int i = 0; i < iterations; i++) {
	        for (unsigned int j = 0; j < joints.size(); j++) {
	            SphericalJoint *joint = &joints.at(j);
	            joint->apply_impulse();
	        }
	    }

	    for (unsigned int i = 0; i < dynamicBodies.size(); i++) {
	        RigidBody2 *body = dynamicBodies.at(i);
	        body->integrate_velocities(deltaTime);
	    }
  //       accumulator -= dt;
  //   }
  //   float alpha = accumulator / dt;

  //   // interpolate between last and current frame 
  //   for (unsigned int i = 0; i < aBodies.size(); i++) {
  //       RigidBody2 *body = &aBodies.at(i);
		// body->cm_position = body->cm_position * alpha + body->prev_cm_position * ( 1.0f - alpha );
		// body->orientation = (mat3)(interpolate((mat4)body->prev_orientation, (mat4)body->orientation, 1.0f - alpha));
		// body->cm_velocity = body->cm_velocity * alpha + body->prev_cm_velocity * ( 1.0f - alpha );
		// body->angular_momentum = body->angular_momentum * alpha + body->prev_angular_momentum * ( 1.0f - alpha );
		// body->cm_force = body->cm_force * alpha + body->prev_cm_force * ( 1.0f - alpha );
		// body->torque = body->torque * alpha + body->prev_torque * ( 1.0f - alpha );
		// body->angular_velocity = body->angular_velocity * alpha + body->prev_angular_velocity * ( 1.0f - alpha );
  //   }
    return 0;
}