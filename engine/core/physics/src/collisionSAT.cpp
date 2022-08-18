#include "collisionSAT.h"

float depth_epsilon = 0.001f;   

// find furthest along n
vec3 get_support(const RigidBody2 &hull, vec3 n) {
    vec3 _v;
    float _d = -FLT_MAX;
    for (unsigned int i = 0; i < hull.a_bounding_vertices.size(); i++) {
        vec3 v = hull.a_bounding_vertices.at(i);
        float d = dot(v, n);
        if (_d == -FLT_MAX || d > _d) {
            _d = d;
            _v = v;
        }
    }
    return _v;
}

bool sphere_sphere_collision(const RigidBody2 &hullA, const RigidBody2 &hullB) {
    return glm::distance(hullA.cm_position, hullB.cm_position) < (hullA.dX2 + hullB.dX2);
}


bool sphere_box_collision(const RigidBody2 &hullA, vec3 closest_pt) {
    return glm::distance(hullA.cm_position, closest_pt) - hullA.dX2 < 0.0f;
}
vec3 closest_point_on_cylinder(vec3 p, const RigidBody2 &cylinder) {
    vec3 p1 = cylinder.cm_position - cylinder.orientation[1];
    vec3 p2 = cylinder.cm_position + cylinder.orientation[1];
    float dist = dot(p - p1, p2 - p1);
    float cylinder_r = cylinder.dX2;
    vec3 point;
    if (dist <= 0.0f) {
        point = closest_point_on_circle(p, p1, normalize(cylinder.orientation[1]) * -1.0f, cylinder_r);
    } else if (dist >= length(p2 - p1)) {
        point = closest_point_on_circle(p, p2, normalize(cylinder.orientation[1]), cylinder_r);
    } else {
        float d = pointLineIntersection(p, p1, p2);
        vec3 point_on_line_segment = p1 + normalize(p2 - p1) * d;
        point = point_on_line_segment + normalize(p - point_on_line_segment) * cylinder_r;
    }
    return point;
}
bool sphere_cylinder_collision(const RigidBody2 &body_a, vec3 closest_pt) {
    return glm::distance(body_a.cm_position, closest_pt) - body_a.dX2 < 0.0f;
}


Query2 query_face_directions(RigidBody2 &hullA, RigidBody2 &hullB) {
    float best_distance = -FLT_MAX;
    vec2 best_index(-1,-1);
    vec3 best_axis(-FLT_MAX,-FLT_MAX,-FLT_MAX);
    // *try* not to test against bottom face of triangle
    unsigned int range = hullA.faces.size();
    if (hullB.id == TRIANGLE) {
        range = 1;
    }
    for (unsigned i = 0; i < range; i++) {
        Face2 *f = &hullA.faces.at(i);
        vec3 vertexB = get_support(hullB, f->normal_ws * -1.0f);
        float dist = pointPlaneDistance2(vertexB, f->position_ws, f->normal_ws);
        if (dist > best_distance) {
            best_index = vec2(i, -1);
            best_distance = dist;
            best_axis = f->normal_ws;
        }
    }
    return Query2(best_distance, best_index, best_axis);
}

Query2 query_edge_directions(RigidBody2 &hullA, RigidBody2 &hullB) {
    float best_distance = -FLT_MAX;
    vec2 best_index(-1,-1);
    vec3 best_axis(-FLT_MAX,-FLT_MAX,-FLT_MAX);

    for (unsigned i = 0; i < hullA.edges.size(); i++) {

        Edge2 *edge_a = &hullA.edges.at(i);
        for (unsigned j = 0; j < hullB.edges.size(); j++) {

            Edge2 *edge_b = &hullB.edges.at(j);

            // negate last two values for minkowski difference
            vec3 edge_a_n1 = edge_a->faces.at(0).normal_ws;
            vec3 edge_a_n2 = edge_a->faces.at(1).normal_ws;
            vec3 edge_b_n1 = edge_b->faces.at(0).normal_ws;
            vec3 edge_b_n2 = edge_b->faces.at(1).normal_ws;

            bool builds_face = isMinkowskiFace2(edge_a_n1, edge_a_n2, cross(edge_a_n2, edge_a_n1) * -1.0f, edge_b_n1 * -1.0f, edge_b_n2 * -1.0f, cross(edge_b_n2,edge_b_n1) * -1.0f);
            if (!builds_face) {
                continue;
            }
            vec3 axis = normalize(cross(edge_a->v2_ws - edge_a->v1_ws, edge_b->v2_ws - edge_b->v1_ws));
            // check edges arent parallel
            if (length(axis) < 0.005f) {
                continue;
            }
            // check normal is pointing away from A
            if (dot(axis, edge_a->v1_ws - hullA.cm_position) < 0.0f) {
                axis = axis * -1.0f;
            }

            float dist1 = pointPlaneDistance2(edge_b->v1_ws, edge_a->v1_ws, axis);
            float dist2 = pointPlaneDistance2(edge_b->v2_ws, edge_a->v1_ws, axis);
            float dist;
            if (dist1 > dist2) {
                dist = dist1;
            } else {
                dist = dist2;
            }

            vec2 mu = lineSegmentIntersection(edge_a->v1_ws, edge_a->v2_ws, edge_b->v1_ws, edge_b->v2_ws);
            float mu_a = mu.x;
            float mu_b = mu.y;
            bool valid_intersection = mu_a >= 0.0f && mu_a <= 1.0f && mu_b >= 0.0f && mu_b <= 1.0f;

            // keep largest penetration
            if (valid_intersection && dist > best_distance) {
                best_index = vec2(i,j);
                best_distance = dist;
                best_axis = axis; 
            }
        }
    }
    return Query2(best_distance, best_index, best_axis);
}

vector<Query2> SAT2(RigidBody2 &hullA, RigidBody2 &hullB) {
    // test all normals of hull_a as axes
    Query2 face_query_a = query_face_directions(hullA, hullB);
    face_query_a.id = 1;
    if (face_query_a.best_distance > 0.0f) {
        return {face_query_a};
    }

    // and all normals of hull_b as axes
    Query2 face_query_b = query_face_directions(hullB, hullA);
    face_query_b.id = 2;
    if (face_query_b.best_distance > 0.0f) {
        return {face_query_b};
    }

    // and the cross product of the edges if they build a face on the minkowski
    Query2 edge_query = query_edge_directions(hullA, hullB);
    edge_query.id = 0;
    if (edge_query.best_distance > 0.0f) {
        return {edge_query};
    }

    bool face_contact_a = face_query_a.best_distance > edge_query.best_distance;
    bool face_contact_b = face_query_b.best_distance > edge_query.best_distance;

    if (face_contact_a && face_contact_b) {
        if (face_query_a.best_distance > face_query_b.best_distance) {
            return {face_query_a};
        } else {
            return {face_query_b};
        }
    } else {
        return {face_query_a, face_query_b, edge_query};
    }
}