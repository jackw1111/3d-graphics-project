#include "body.h"
#include "collisionSAT.h"

int NONE = -1;
int TRIANGLE = 0;
int BOX = 1;
int CYLINDER = 2;
int SPHERE = 3;
int PLANE = 4;
int CAPSULE = 5;

int RigidBody2::Counter = -1;

RigidBody2::RigidBody2(){
    RigidBody2::Counter += 1;
    octreeID = RigidBody2::Counter;
    offset = mat4(1.0f);
    coefficient_of_restitution = 0.7f;
    density = 0.4f;
    friction = 1.0f;
    cm_position = vec3(0,0,0);
    orientation = mat3(vec3(1,0,0), vec3(0,1,0), vec3(0,0,1));
    cm_velocity = vec3(0,0,0);
    angular_momentum = vec3(0,0,0);
    cm_force = vec3(0,0,0);
    torque = vec3(0,0,0);
    inverse_world_inertia_tensor = mat3(vec3(1,0,0), vec3(0,1,0), vec3(0,0,1));
    inverse_body_inertia_tensor = mat3(vec3(1,0,0), vec3(0,1,0), vec3(0,0,1));
    angular_velocity = vec3(0,0,0);
    prev_cm_position = vec3(0,0,0);
    prev_orientation = mat3(vec3(1,0,0), vec3(0,1,0), vec3(0,0,1));
    prev_cm_velocity = vec3(0,0,0);
    prev_angular_momentum = vec3(0,0,0);
    prev_cm_force = vec3(0,0,0);
    prev_torque = vec3(0,0,0);
    prev_inverse_world_inertia_tensor = mat3(vec3(1,0,0), vec3(0,1,0), vec3(0,0,1));
    prev_inverse_body_inertia_tensor = mat3(vec3(1,0,0), vec3(0,1,0), vec3(0,0,1));
    prev_angular_velocity = vec3(0,0,0);

    mass = 10.0f;
    one_over_mass = 1.0f / mass;
};

int RigidBody2::integrate_forces(vec3 gravity, float deltaTime) {

    if (one_over_mass != 0.0f){
        cm_force += gravity * (1.0f / one_over_mass);
    }

    // integrate primary quantities
    cm_velocity = cm_velocity + cm_force * one_over_mass * deltaTime;
    angular_momentum = angular_momentum + torque * deltaTime;
    // compute auxillary quantities
    inverse_world_inertia_tensor = orientation * inverse_body_inertia_tensor * transpose(orientation);
    angular_velocity = inverse_world_inertia_tensor * angular_momentum;

    prev_cm_velocity = cm_velocity;
    prev_angular_momentum = angular_momentum;
    prev_inverse_world_inertia_tensor = inverse_world_inertia_tensor;
    prev_angular_velocity = angular_velocity;
    return 0;
}

int RigidBody2::integrate_velocities(float deltaTime) {
    // integrate primary quantities
    cm_position = cm_position + cm_velocity * deltaTime;
    orientation = orientation + setSkewSymmetric(angular_velocity) * orientation * deltaTime;
    orientation = orthonormalizeOrientation(orientation);
    torque = vec3(0,0,0);
    cm_force = vec3(0,0,0);

    prev_cm_position = cm_position;
    prev_orientation = orientation;
    return 0;
}

mat4 RigidBody2::get_model_matrix() {

    mat4 am = translate(mat4(1.0f), cm_position) * mat4(orientation) * offset;
    am = scale(am, vec3(dX2, dY2, dZ2));
    return am;
}

int RigidBody2::update() {

    calculate_vertices();
    for (unsigned int i = 0; i < faces.size(); i++) {

        Face2 *f = &faces.at(i);
        f->set_data(a_bounding_vertices);
    }
    for (unsigned int i = 0; i < edges.size(); i++) {
        Edge2 *e = &edges.at(i);
        e->set_data(a_bounding_vertices, faces);
    }
    return 0;
}



int RigidBody2::calculate_vertices() {
    a_bounding_vertices = {};
    for (unsigned int i = 0; i < number_of_bounding_vertices; i++) {
        a_bounding_vertices.push_back(cm_position + orientation * a_body_bounding_vertices.at(i));
    }
    return 0;
}

Face2::Face2(const vector<int> &_verts_indices) {
    this->verts_indices = _verts_indices;
    this->position_ws = vec3(0,0,0);
    this->normal_ws =  vec3(0,0,0);
    this->verts_ws = {};
}

int Face2::set_data(const vector<vec3> &verts) {
  verts_ws = {};
  position_ws = vec3(0,0,0);
  for (unsigned int i = 0; i < verts_indices.size(); i++) {
    verts_ws.push_back(verts.at(verts_indices.at(i)));
  }
  for (unsigned int i = 0; i < verts_ws.size(); i++) {
      position_ws += verts_ws.at(i);
  }
  position_ws = position_ws * (1.0f / (float)verts_ws.size());
  if (verts_ws.size() >= 3) {
    normal_ws = normalize(cross(verts_ws.at(0) - verts_ws.at(2), verts_ws.at(1) - verts_ws.at(0)));
  } else {
    normal_ws = vec3(0,0,0);
  }  
  return 0;
}

Edge2::Edge2(const vector<int> &verts_indices, const vector<int> &_faces_indices) {
        v1_index = verts_indices.at(0);
        v2_index = verts_indices.at(1);
        faces_indices = _faces_indices;
}

int Edge2::set_data(const vector<vec3> &verts, const vector<Face2> &_faces) {
    faces = {_faces.at(faces_indices.at(0)), _faces.at(faces_indices.at(1))};
    v1_ws = verts.at(v1_index);
    v2_ws = verts.at(v2_index);
    return 0;
}

int init_plane(RigidBody2 &body) {
    body.id = PLANE;
    body.inverse_body_inertia_tensor = mat3(vec3(0,0,0),
                                       vec3(0,0,0),
                                       vec3(0,0,0));
    body.inverse_world_inertia_tensor = mat3(vec3(0,0,0),
                                           vec3(0,0,0),
                                           vec3(0,0,0));
   
    vector<vec3> verts = {vec3(-1,0,-1), vec3(-1,0,1), vec3(1,0,1),vec3(1,0,-1)};
    body.a_body_bounding_vertices = verts;
    body.a_bounding_vertices = verts;
    body.number_of_bounding_vertices = 4;
    Face2 f0({0,1,2,3});
    body.faces = {f0};
    body.update();

    return 0;
}   

int init_triangle(RigidBody2 &body, vector<vec3> verts) {
    body.id = TRIANGLE;
    body.inverse_body_inertia_tensor = mat3(vec3(0,0,0), vec3(0,0,0), vec3(0,0,0));
    body.inverse_world_inertia_tensor = mat3(vec3(0,0,0), vec3(0,0,0), vec3(0,0,0));
    body.dX2 = 1.0f;
    body.dY2 = 1.0f;
    body.dZ2 = 1.0f;
    body.faces = {};
    body.edges = {};
    body.coefficient_of_restitution = 0.0f;
    body.a_body_bounding_vertices = verts;
    body.a_bounding_vertices = verts;
    body.number_of_bounding_vertices = 3;
    body.mass = FLT_MAX;
    body.one_over_mass = 0.0f;
    Face2 f0({0,1,2});
    Face2 f1({0,2,1});
    Face2 f2({0,1});
    Face2 f3({1,2});
    Face2 f4({2,0});
    body.faces = {f0, f1, f2, f3, f4};

    Edge2 e0({0,1},{0, 1});
    Edge2 e1({1,2},{0, 1});
    Edge2 e2({2,0},{0, 1});
    body.edges = {e0, e1, e2};

    body.update();
    body.faces.at(2).normal_ws = normalize(cross(body.faces.at(0).normal_ws, body.faces.at(2).verts_ws.at(1) - body.faces.at(2).verts_ws.at(0)));
    body.faces.at(3).normal_ws = normalize(cross(body.faces.at(0).normal_ws, body.faces.at(3).verts_ws.at(1) - body.faces.at(3).verts_ws.at(0)));
    body.faces.at(4).normal_ws = normalize(cross(body.faces.at(0).normal_ws, body.faces.at(4).verts_ws.at(1) - body.faces.at(4).verts_ws.at(0)));

    if (dot(body.faces.at(2).verts_ws.at(0) - body.faces.at(0).position_ws, body.faces.at(2).normal_ws) < 0.0f) {
        body.faces.at(2).normal_ws = body.faces.at(2).normal_ws * -1.0f;
    }
    if (dot(body.faces.at(3).verts_ws.at(0) - body.faces.at(0).position_ws, body.faces.at(3).normal_ws) < 0.0f) {
        body.faces.at(3).normal_ws = body.faces.at(3).normal_ws * -1.0f;
    }
    if (dot(body.faces.at(4).verts_ws.at(0) - body.faces.at(0).position_ws, body.faces.at(4).normal_ws) < 0.0f) {
        body.faces.at(4).normal_ws = body.faces.at(4).normal_ws * -1.0f;
    }
    // calculate cm_position from vertices;
    body.cm_position = vec3(0,0,0);

    return 0;
}

Tri::Tri(int _id, std::vector<vec3> _data) {
    id = _id;
    data = _data;
}  

int TriOctree::counter = 0;
TriOctree::TriOctree() {
} 


void create_children(TriOctree &octree) {

    vec3 position = octree.position;
    float hw = octree.hw/2.0f;
    vector<vec3> allPositions = {vec3(-hw,hw,hw) + position, 
                                vec3(hw,hw,hw) + position, 
                                vec3(hw,hw,-hw) + position, 
                                vec3(-hw,hw,-hw) + position, 

                                vec3(-hw,-hw,hw) + position, 
                                vec3(hw,-hw,hw) + position, 
                                vec3(hw,-hw,-hw) + position, 
                                vec3(-hw,-hw,-hw) + position};

    // split triangles into nodes
    RigidBody2 body1;
    init_box(body1, octree.hw/2.0f,octree.hw/2.0f,octree.hw/2.0f);

    for (unsigned int i = 0; i < allPositions.size(); i++) {
        vec3 position = allPositions.at(i);
        body1.cm_position = position;
        body1.update();
        vector<Tri> node_triangles = {};
        for (unsigned int j = 0; j < octree.triangles.size(); j++) {

            vector<vec3> tri = octree.triangles.at(j).data;
            RigidBody2 body2;
            init_triangle(body2, tri);
            vector<Query2> queries = SAT2(body1, body2);

            for (unsigned k = 0; k < queries.size(); k++) {

                Query2 *query = &queries.at(k);
                if (query->best_distance < 0.0f) {
                    node_triangles.push_back(octree.triangles.at(j));
                    break;
                }
            }

            // if (is_triangle_in_cube(position, octree.hw/2.0f, tri)) {
            //     node_triangles.push_back(octree.triangles.at(j));
            // }
        }
        if (node_triangles.size()) {
            TriOctree ot;
            octree.children.push_back(ot);
            init_octree(octree.children.at(octree.children.size()-1),0, position, octree.hw/2.0f, node_triangles);
        }
    }
}


void init_octree(TriOctree &octree, int ind, vec3 pos, float hwidth, std::vector<Tri> tris) {
    TriOctree::counter++;
    octree.MAX_TRIANGLES = 50;
    octree.index = ind;
    octree.position = pos;
    octree.hw = hwidth;
    octree.triangles = tris;
    octree.xMin = octree.yMin = octree.zMin = FLT_MAX;
    octree.xMax = octree.yMax = octree.zMax = -FLT_MAX;
    octree.children = {};

    float hw = octree.hw;
    vec3 position = octree.position;
    vector<vec3> points = {vec3(-hw,hw,hw) + position, 
                            vec3(hw,hw,hw) + position, 
                            vec3(hw,hw,-hw) + position, 
                            vec3(-hw,hw,-hw) + position, 

                            vec3(-hw,-hw,hw) + position, 
                            vec3(hw,-hw,hw) + position, 
                            vec3(hw,-hw,-hw) + position, 
                            vec3(-hw,-hw,-hw) + position};

    if (octree.triangles.size() > octree.MAX_TRIANGLES) {
        create_children(octree);
        octree.triangles = {};
    }

    octree.xMin = octree.yMin = octree.zMin = FLT_MAX;
    octree.xMax = octree.yMax = octree.zMax = -FLT_MAX;

    for (unsigned int i = 0; i < points.size(); i++) {

        vec3 point = points.at(i);

        if  (point.x < octree.xMin){
            octree.xMin = point.x;
        }
        if  (point.y < octree.yMin){
            octree.yMin = point.y;
        }
        if  (point.z < octree.zMin){
            octree.zMin = point.z;
        }
        if  (point.x > octree.xMax){
            octree.xMax = point.x;
        }
        if  (point.y > octree.yMax){
            octree.yMax = point.y;
        }
        if  (point.z > octree.zMax){
            octree.zMax = point.z;
        }
    }
}


// get the triangles to collide with for a particular point, pos.
vector<int> get_triangle_set(TriOctree &octree, const vector<vec3> &positions, vector<int> &tri_ids_) {
    vector<int> tri_ids = {};
    TriOctree *currentCube;
    for (unsigned int i = 0; i < positions.size(); i++) {
        vec3 pos = positions.at(i);
        currentCube = &octree;
        if (octree.children.size()) {
            for (unsigned int j = 0; j < octree.children.size(); j++) {
                currentCube = &octree.children.at(j);
                vec3 _min(currentCube->xMin, currentCube->yMin, currentCube->zMin);
                vec3 _max(currentCube->xMax, currentCube->yMax, currentCube->zMax);
                if (is_point_in_cube(_min, _max, pos)) {
                    vector<int> _tri_ids = get_triangle_set(*currentCube, {pos}, tri_ids);
                    tri_ids.insert(tri_ids.end(), _tri_ids.begin(), _tri_ids.end());
                }
            }
        } else {
            for (unsigned int j = 0; j < currentCube->triangles.size(); j++) {
                Tri *tri = &currentCube->triangles.at(j);
                tri_ids.push_back(tri->id);
            }
        }
    }
    return tri_ids;
    // vector<int> tri_ids = {};
    // std::stack<TriOctree*> stack;
    // stack.push(&octree);

    // while (!stack.empty()) {
    //     TriOctree *top = stack.top();
    //     stack.pop();
    //     vec3 _min(top->xMin, top->yMin, top->zMin);
    //     vec3 _max(top->xMax, top->yMax, top->zMax);
    //     for (unsigned int i = 0; i < positions.size(); i++) {
    //         if (is_point_in_cube(_min, _max, positions.at(i))) {
    //             if (top->children.size()) {
    //                 for (unsigned int j = 0; j < top->children.size(); j++) {
    //                     TriOctree *child = &top->children.at(j);
    //                     stack.push(&top->children.at(j));
    //                 }
    //             } else {
    //                 for (unsigned int j = 0; j < top->triangles.size(); j++) {
    //                     Tri *tri = &top->triangles.at(j);
    //                     tri_ids.push_back(tri->id);
    //                 }
    //             }
    //             break;
    //         }
    //     }
    // }
    // return tri_ids;
}


bool is_point_in_cube(vec3 min, vec3 max, vec3 position) {

    // check if at least one vertex is in bounds of cube
    if (position.x >= min.x && position.x <= max.x &&
        position.y >= min.y && position.y <= max.y &&
        position.z >= min.z && position.z <= max.z) {
        return true;
    }
    return false;
}

bool is_triangle_in_cube(vec3 position, float hw, const vector<vec3> &triangles) {

    vector<vec3> points = {vec3(-hw,hw,hw) + position, 
            vec3(hw,hw,hw) + position, 
            vec3(hw,hw,-hw) + position, 
            vec3(-hw,hw,-hw) + position, 

            vec3(-hw,-hw,hw) + position, 
            vec3(hw,-hw,hw) + position, 
            vec3(hw,-hw,-hw) + position, 
            vec3(-hw,-hw,-hw) + position};

    float xMin;
    float yMin;
    float zMin;
    float xMax;
    float yMax;
    float zMax;  

    xMin = yMin = zMin = FLT_MAX;
    xMax = yMax = zMax = -FLT_MAX;

    for (unsigned int i = 0; i < points.size(); i++) {

        vec3 point = points.at(i);


        if  (point.x < xMin){
            xMin = point.x;
        }
        if  (point.y < yMin){
            yMin = point.y;
        }
        if  (point.z < zMin){
            zMin = point.z;
        }
        if  (point.x > xMax){
            xMax = point.x;
        }
        if  (point.y > yMax){
            yMax = point.y;
        }
        if  (point.z > zMax){
            zMax = point.z;
        }
    }

    // check if at least one vertex is in bounds of cube
    for (unsigned int i = 0; i < triangles.size(); i++) {

        vec3 vertex = triangles.at(i);
        if (vertex.x >= xMin && vertex.x <= xMax &&
            vertex.y >= yMin && vertex.y <= yMax &&
            vertex.z >= zMin && vertex.z <= zMax) {
            return true;
        }
    }
        
    return false;

}

TriOctree octree;

vector<vec4> getAllNodes() {

    std::vector<vec4> data = {};
    std::stack<TriOctree*> stack;
    stack.push(&octree);

    while (!stack.empty()) {
        TriOctree *top = stack.top();
        stack.pop();
        for (unsigned int i = 0; i < top->children.size(); i++) {
            if (top->children.at(i).children.size()) {
                stack.push(&top->children.at(i));
            } else {
                data.push_back(vec4(top->children.at(i).position.x, top->children.at(i).position.y, top->children.at(i).position.z, top->children.at(i).hw));
            }
        }
    };

    return data;
}

std::vector<RigidBody2> init_triangle_mesh(vector<vec3> verts) {
    float min_x = FLT_MAX;
    float min_y = FLT_MAX;
    float min_z = FLT_MAX;

    float max_x = -FLT_MAX;
    float max_y = -FLT_MAX;
    float max_z = -FLT_MAX;

    std::vector<RigidBody2> bodies = {};
    for (unsigned int i = 0; i < verts.size(); i+=3) {
            RigidBody2 body;
            vec3 v0 = verts.at(i+0);
            vec3 v1 = verts.at(i+1);
            vec3 v2 = verts.at(i+2);
            if (v0.x < min_x) {
                min_x = v0.x;
            }
            if (v0.y < min_y) {
                min_y = v0.y;
            }
            if (v0.z < min_z) {
                min_z = v0.z;
            }

            if (v1.x < min_x) {
                min_x = v1.x;
            }
            if (v1.y < min_y) {
                min_y = v1.y;
            }
            if (v1.z < min_z) {
                min_z = v1.z;
            }

            if (v2.x < min_x) {
                min_x = v2.x;
            }
            if (v2.y < min_y) {
                min_y = v2.y;
            }
            if (v2.z < min_z) {
                min_z = v2.z;
            }

            if (v0.x > max_x) {
                max_x = v0.x;
            }
            if (v0.y > max_y) {
                max_y = v0.y;
            }
            if (v0.z > max_z) {
                max_z = v0.z;
            }

            if (v1.x > max_x) {
                max_x = v1.x;
            }
            if (v1.y > max_y) {
                max_y = v1.y;
            }
            if (v1.z > max_z) {
                max_z = v1.z;
            }

            if (v2.x > max_x) {
                max_x = v2.x;
            }
            if (v2.y > max_y) {
                max_y = v2.y;
            }
            if (v2.z > max_z) {
                max_z = v2.z;
            }
            init_triangle(body, {v0, v1, v2});
            bodies.push_back(body);
    }
    vector<Tri> octree_tris = {};
    for (unsigned int i = 0; i < bodies.size(); i++) {
        octree_tris.push_back(Tri(bodies.at(i).octreeID, bodies.at(i).faces.at(0).verts_ws));
    }

    vec3 _min1(min_x,0,0);
    vec3 _max1(max_x,0,0);
    vec3 _min2(0,min_y,0);
    vec3 _max2(0,max_y,0);
    vec3 _min3(0,0,min_z);
    vec3 _max3(0,0,max_z);
    float l1 = glm::distance(_min1, _max1);
    float l2 = glm::distance(_min2, _max2);
    float l3 = glm::distance(_min3, _max3);
    float d = std::max(std::max(l1, l2), l3);

    init_octree(octree, 0, vec3(0,0,0), d, octree_tris);

    return bodies;
}

int init_box(RigidBody2 &body, float dX2, float dY2, float dZ2) {

    body.id = BOX;

    body.dX2 = dX2;
    body.dY2 = dY2;
    body.dZ2 = dZ2;

    body.inverse_body_inertia_tensor = mat3(vec3(3.0f/(body.mass*(body.dY2*body.dY2+body.dZ2*body.dZ2)),0,0),
                                    vec3(0,3.0f/(body.mass*(body.dX2*body.dX2+body.dZ2*body.dZ2)),0),
                                    vec3(0,0,3.0f/(body.mass*(body.dX2*body.dX2+body.dY2*body.dY2))));

    body.inverse_world_inertia_tensor = mat3(vec3(0,0,0), vec3(0,0,0), vec3(0,0,0));

    body.number_of_bounding_vertices = 8;

    body.a_body_bounding_vertices ={vec3(-body.dX2, body.dY2, body.dZ2),
                                vec3(-body.dX2,-body.dY2, body.dZ2),
                                vec3( body.dX2,-body.dY2, body.dZ2),
                                vec3( body.dX2, body.dY2, body.dZ2),
                                vec3(-body.dX2, body.dY2,-body.dZ2),
                                vec3(-body.dX2,-body.dY2,-body.dZ2),
                                vec3( body.dX2,-body.dY2,-body.dZ2),
                                vec3( body.dX2, body.dY2,-body.dZ2)};

    body.calculate_vertices();

    Face2 f0({0,1,2,3});
    Face2 f1({4,5,1,0});
    Face2 f2({1,5,6,2});
    Face2 f3({3,2,6,7});
    Face2 f4({4,0,3,7});
    Face2 f5({7,6,5,4});
    vector<Face2> f = {f0, f1, f2, f3, f4, f5};
    body.faces = f;

    Edge2 e0({0,1},{0, 1});
    Edge2 e1({1,2},{0, 2});
    Edge2 e2({2,3},{0, 3});
    Edge2 e3({3,0},{0, 4});

    Edge2 e4({5,4},{1, 5});
    Edge2 e5({6,5},{2, 5});
    Edge2 e6({7,6},{3, 5});
    Edge2 e7({4,7},{4, 5});

    Edge2 e8({4,0},{1, 4});
    Edge2 e9({1,5},{1, 2});
    Edge2 e10({6,2},{2, 3});
    Edge2 e11({3,7},{3, 4});

    vector<Edge2> e = {e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11};
    body.edges = e;
    // calculate cm_position from vertices
    body.cm_position = vec3(0,0,0);

    return 0;
}


int init_cylinder(RigidBody2 &body) {
    float r = 1;
    float h = 1;
    body.id = CYLINDER;
    body.inverse_body_inertia_tensor = mat3(vec3(12.0f/(body.mass*(3*r*r + h*h)),0,0),
                                            vec3(0,2.0f/(body.mass*(r*r)),0),
                                            vec3(0,0,12.0f/(body.mass*(3*r*r + h*h))));


    body.faces = {};
    body.edges = {};
    body.number_of_bounding_vertices = 32;

    vector<vec3> b = {};
    for (unsigned int i = 0; i < 16; i++) {
        b.push_back(vec3(-cos(glm::radians(i/16.0f*360.0f)), h,sin(glm::radians(i/16.0f*360.0f)))*r);
    }
    for (unsigned int i = 0; i < 16; i++) {
        b.push_back(vec3(-cos(glm::radians(i/16.0f*360.0f)),-h,sin(glm::radians(i/16.0f*360.0f)))*r);
    }
    body.a_body_bounding_vertices = b;
    body.calculate_vertices();

    Edge2 e0({0,1},{0,1});
    Edge2 e1({1,2},{0,2});
    Edge2 e2({2,3},{0,3});
    Edge2 e3({3,4},{0,4});
    Edge2 e4({4,5},{0,5});
    Edge2 e5({5,6},{0,6});
    Edge2 e6({6,7},{0,7});
    Edge2 e7({7,8},{0,8});
    Edge2 e8({8,9},{0,9});
    Edge2 e9({9,10},{0,10});
    Edge2 e10({10,11},{0,11});
    Edge2 e11({11,12},{0,12});
    Edge2 e12({12,13},{0,13});
    Edge2 e13({13,14},{0,14});
    Edge2 e14({14,15},{0,15});
    Edge2 e15({15,0},{0,16});
    Edge2 e16({17,16},{1,17});
    Edge2 e17({18,17},{2,17});
    Edge2 e18({19,18},{3,17});
    Edge2 e19({20,19},{4,17});
    Edge2 e20({21,20},{5,17});
    Edge2 e21({22,21},{6,17});
    Edge2 e22({23,22},{7,17});
    Edge2 e23({24,23},{8,17});
    Edge2 e24({25,24},{9,17});
    Edge2 e25({26,25},{10,17});
    Edge2 e26({27,26},{11,17});
    Edge2 e27({28,27},{12,17});
    Edge2 e28({29,28},{13,17});
    Edge2 e29({30,29},{14,17});
    Edge2 e30({31,30},{15,17});
    Edge2 e31({16,31},{16,17});
    vector<Edge2> e = {e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, e12, e13, e14, e15, e16, e17, e18, e19, e20, e21, e22, e23, e24, e25, e26, e27, e28, e29, e30, e31};
    body.edges = e;
    Face2 f0({0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15});
    Face2 f1({0, 16, 17, 1});
    Face2 f2({1, 17, 18, 2});
    Face2 f3({2, 18, 19, 3});
    Face2 f4({3, 19, 20, 4});
    Face2 f5({4, 20, 21, 5});
    Face2 f6({5, 21, 22, 6});
    Face2 f7({6, 22, 23, 7});
    Face2 f8({7, 23, 24, 8});
    Face2 f9({8, 24, 25, 9});
    Face2 f10({9, 25, 26, 10});
    Face2 f11({10, 26, 27, 11});
    Face2 f12({11, 27, 28, 12});
    Face2 f13({12, 28, 29, 13});
    Face2 f14({13, 29, 30, 14});
    Face2 f15({14, 30, 31, 15});
    Face2 f16({15, 31, 16, 0});
    Face2 f17({31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16});
    vector<Face2> f = {f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17};
    body.faces = f;
    // calculate cm_position from vertices
    body.cm_position = vec3(0,0,0);

    //body.offset = rotate(mat4(1.0f), radians(90.0f), vec3(1,0,0));

    return 0;
}

int init_sphere(RigidBody2 &body) {
    body.id = SPHERE;
    
    body.number_of_bounding_vertices = 8;

    body.a_body_bounding_vertices ={vec3(-body.dX2, body.dY2, body.dZ2),
                                vec3(-body.dX2,-body.dY2, body.dZ2),
                                vec3( body.dX2,-body.dY2, body.dZ2),
                                vec3( body.dX2, body.dY2, body.dZ2),
                                vec3(-body.dX2, body.dY2,-body.dZ2),
                                vec3(-body.dX2,-body.dY2,-body.dZ2),
                                vec3( body.dX2,-body.dY2,-body.dZ2),
                                vec3( body.dX2, body.dY2,-body.dZ2)};
    body.faces = {};
    body.edges = {};
    float pi = 3.141592654f;
    body.mass = body.density*4.0f/3.0f*pi*body.dX2*body.dX2*body.dX2;
    body.one_over_mass = 1.0f / body.mass;
    body.inverse_body_inertia_tensor = mat3(vec3(2.5f*body.mass*body.dX2*body.dX2,0,0),
                                       vec3(0,2.5f*body.mass*body.dX2*body.dX2,0),
                                       vec3(0,0,2.5f*body.mass*body.dX2*body.dX2));
    body.coefficient_of_restitution = 1.0f;
    return 0;
}

#define PI 3.141592654f
#define PI_TIMES2 6.283185307f
const float oneDiv3 = (float)(1.0 / 3.0);
const float oneDiv8 = (float)(1.0 / 8.0);
const float oneDiv12 = (float)(1.0 / 12.0);

void ComputeRigidBodyProperties_Capsule(float capsuleHeight, float capsuleRadius, float density, float &mass, mat3 &inertia) {
    float cM; // cylinder mass
    float hsM;
    // mass of hemispheres
    float rSq = capsuleRadius*capsuleRadius;
    cM = PI*capsuleHeight*rSq*density;
    hsM = PI_TIMES2*oneDiv3*rSq*capsuleRadius*density;
    // from cylinder 
    inertia[1][1] = rSq*cM*0.5f; 
    inertia[0][0] = inertia[2][2]= inertia[1][1]*0.5f + cM*capsuleHeight*capsuleHeight*oneDiv12;
    // from hemispheres 
    float temp0 = hsM*2.0f*rSq / 5.0f;
    inertia[1][1] += temp0 * 2.0f;
    float temp1 = capsuleHeight*0.5f; float temp2 = temp0 + hsM*(temp1*temp1 + 3.0f*oneDiv8*capsuleHeight*capsuleRadius);
    inertia[0][0] += temp2 * 2.0f;
    inertia[2][2] += temp2 * 2.0f;
    inertia[0][1] = inertia[0][2] = inertia[1][0] = inertia[1][2] = inertia[2][0] = inertia[2][1] = 0.0f;
    mass = cM + hsM * 2.0f;
} 

int init_capsule(RigidBody2 &body) {
    body.id = CAPSULE;

    body.number_of_bounding_vertices = 0;
    float pi = 3.141592654f;
    body.inverse_body_inertia_tensor = mat3(vec3(0,0,0), vec3(0,0,0), vec3(0,0,0));
    body.inverse_world_inertia_tensor = mat3(vec3(0,0,0), vec3(0,0,0), vec3(0,0,0));    
    body.density = 1.0f;
    body.dX2 = 1.0f;
    body.dY2 = 1.0f;
    body.mass = body.density*4.0f/3.0f*pi*body.dX2*body.dX2*body.dX2;

    // body.inverse_body_inertia_tensor = mat3(vec3(2.5f*body.mass*body.dX2*body.dX2,0,0),
    //                                    vec3(0,2.5f*body.mass*body.dX2*body.dX2,0),
    //                                    vec3(0,0,2.5f*body.mass*body.dX2*body.dX2));
    mat3 inertia_tensor;
    ComputeRigidBodyProperties_Capsule(body.dY2, body.dX2, body.density, body.mass, body.inverse_body_inertia_tensor);

    body.one_over_mass = 1.0f / body.mass;
    body.coefficient_of_restitution = 1.0f;

    float r = 1;
    float h = 1;

    body.faces = {};
    body.edges = {};
    body.number_of_bounding_vertices = 32;

    vector<vec3> b = {};
    for (unsigned int i = 0; i < 16; i++) {
        b.push_back(vec3(-cos(glm::radians(i/16.0f*360.0f)), h,sin(glm::radians(i/16.0f*360.0f)))*r);
    }
    for (unsigned int i = 0; i < 16; i++) {
        b.push_back(vec3(-cos(glm::radians(i/16.0f*360.0f)),-h,sin(glm::radians(i/16.0f*360.0f)))*r);
    }
    body.a_body_bounding_vertices = b;
    body.calculate_vertices();

    Edge2 e0({0,1},{0,1});
    Edge2 e1({1,2},{0,2});
    Edge2 e2({2,3},{0,3});
    Edge2 e3({3,4},{0,4});
    Edge2 e4({4,5},{0,5});
    Edge2 e5({5,6},{0,6});
    Edge2 e6({6,7},{0,7});
    Edge2 e7({7,8},{0,8});
    Edge2 e8({8,9},{0,9});
    Edge2 e9({9,10},{0,10});
    Edge2 e10({10,11},{0,11});
    Edge2 e11({11,12},{0,12});
    Edge2 e12({12,13},{0,13});
    Edge2 e13({13,14},{0,14});
    Edge2 e14({14,15},{0,15});
    Edge2 e15({15,0},{0,16});
    Edge2 e16({17,16},{1,17});
    Edge2 e17({18,17},{2,17});
    Edge2 e18({19,18},{3,17});
    Edge2 e19({20,19},{4,17});
    Edge2 e20({21,20},{5,17});
    Edge2 e21({22,21},{6,17});
    Edge2 e22({23,22},{7,17});
    Edge2 e23({24,23},{8,17});
    Edge2 e24({25,24},{9,17});
    Edge2 e25({26,25},{10,17});
    Edge2 e26({27,26},{11,17});
    Edge2 e27({28,27},{12,17});
    Edge2 e28({29,28},{13,17});
    Edge2 e29({30,29},{14,17});
    Edge2 e30({31,30},{15,17});
    Edge2 e31({16,31},{16,17});
    vector<Edge2> e = {e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, e12, e13, e14, e15, e16, e17, e18, e19, e20, e21, e22, e23, e24, e25, e26, e27, e28, e29, e30, e31};
    body.edges = e;
    Face2 f0({0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15});
    Face2 f1({0, 16, 17, 1});
    Face2 f2({1, 17, 18, 2});
    Face2 f3({2, 18, 19, 3});
    Face2 f4({3, 19, 20, 4});
    Face2 f5({4, 20, 21, 5});
    Face2 f6({5, 21, 22, 6});
    Face2 f7({6, 22, 23, 7});
    Face2 f8({7, 23, 24, 8});
    Face2 f9({8, 24, 25, 9});
    Face2 f10({9, 25, 26, 10});
    Face2 f11({10, 26, 27, 11});
    Face2 f12({11, 27, 28, 12});
    Face2 f13({12, 28, 29, 13});
    Face2 f14({13, 29, 30, 14});
    Face2 f15({14, 30, 31, 15});
    Face2 f16({15, 31, 16, 0});
    Face2 f17({31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16});
    vector<Face2> f = {f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17};
    body.faces = f;

    return 0;
}