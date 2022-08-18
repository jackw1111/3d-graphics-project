#ifndef BODY_H
#define BODY_H

#include "math_3d.h"

#include <stack>

extern int NONE;
extern int TRIANGLE;
extern int BOX;
extern int CYLINDER;
extern int SPHERE;
extern int PLANE;
extern int CAPSULE;

class Tri {
public:
    int id;
    std::vector<vec3> data;
    Tri(int _id, std::vector<vec3> _data);    
};

class TriOctree {
public:
    static int counter;
    int index;
    vec3 position;
    float hw;
    vector<Tri> triangles;
    float xMin;
    float yMin;
    float zMin;
    float xMax;
    float yMax;
    float zMax;
    vector<TriOctree> children;
    int MAX_TRIANGLES = 50;
    TriOctree();    
};

vector<int> get_triangle_set(TriOctree &octree, const vector<vec3> &positions, vector<int> &tri_ids);
bool is_point_in_cube(vec3 min, vec3 max, vec3 position);
bool is_triangle_in_cube(vec3 position, float hw, const vector<vec3> &triangles);
void create_children(TriOctree &octree);
void init_octree(TriOctree &octree, int ind, vec3 pos, float hwidth, std::vector<Tri> tris);
vector<vec4> getAllNodes();

class Face2 {
public:
    std::vector<glm::vec3> verts_ws = {};
    std::vector<int> verts_indices = {};
    glm::vec3 position_ws;
    glm::vec3 normal_ws;
    Face2(const std::vector<int> &verts_indices);

    int set_data(const std::vector<glm::vec3> &verts);
};

class Edge2 {
public:
    glm::vec3 v1_ws;
    glm::vec3 v2_ws;

    int v1_index;
    int v2_index;

    std::vector<int> faces_indices = {};
    std::vector<Face2> faces = {};

    Edge2(const std::vector<int> &verts_indices, const std::vector<int> &faces_indices);
    int set_data(const std::vector<glm::vec3> &verts, const std::vector<Face2> &faces);
};

class RigidBody2 {
public:
    static int Counter;
    float dX2 = 1.0f;
    float dY2 = 1.0f;
    float dZ2 = 1.0f;

    // glm::vec3 aabb_min_ws;
    // glm::vec3 aabb_max_ws;

    //StaticObject object;
    int id = -1;
    int octreeID = -1;
    std::vector<int> triIDs = {};
    std::vector<int> objectIDs = {};

    float coefficient_of_restitution;
    float mass;
    float one_over_mass;
    float density;
    float friction;
    glm::vec3 cm_position;
    glm::mat3 orientation;
    glm::vec3 cm_velocity;
    glm::vec3 angular_momentum;
    glm::vec3 cm_force;
    glm::vec3 torque;
    glm::mat3 inverse_world_inertia_tensor;
    glm::mat3 inverse_body_inertia_tensor;
    glm::vec3 angular_velocity;
    glm::vec3 prev_cm_position;
    glm::mat3 prev_orientation;
    glm::vec3 prev_cm_velocity;
    glm::vec3 prev_angular_momentum;
    glm::vec3 prev_cm_force;
    glm::vec3 prev_torque;
    glm::mat3 prev_inverse_world_inertia_tensor;
    glm::mat3 prev_inverse_body_inertia_tensor;
    glm::vec3 prev_angular_velocity;

    std::vector<glm::vec3> a_bounding_vertices = {vec3(0,0,0),vec3(0,0,0),vec3(0,0,0),vec3(0,0,0),vec3(0,0,0),vec3(0,0,0),vec3(0,0,0),vec3(0,0,0)};
    unsigned int number_of_bounding_vertices;
    std::vector<glm::vec3> a_body_bounding_vertices = {};
    std::vector<Face2> faces = {};
    std::vector<Edge2> edges = {};

    //std::vector<glm::vec3> aabb_verts = {};

    glm::mat4 offset;

    RigidBody2();

    int integrate_forces(glm::vec3 gravity, float deltaTime);

    int integrate_velocities(float deltaTime);

    glm::mat4 get_model_matrix();

    int update();

    glm::vec3 get_support(glm::vec3 n);

    int calculate_vertices();

};

int init_plane(RigidBody2 &body);
int init_triangle(RigidBody2 &body, vector<vec3> verts);
std::vector<RigidBody2> init_triangle_mesh(vector<vec3> verts);
int init_box(RigidBody2 &body, float dX2, float dY2, float dZ2);
int init_cylinder(RigidBody2 &body);
int init_sphere(RigidBody2 &body);
int init_capsule(RigidBody2 &body);

#endif