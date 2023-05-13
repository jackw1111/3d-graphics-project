#include "culling.h"

float pointPlaneDistance(const glm::vec3 &point, const glm::vec3 &planePosition, const glm::vec3 &planeNormal) {
    return glm::dot(point - planePosition, planeNormal);
}

bool isMinkowskiFace(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &b_x_a, const glm::vec3 &c, const glm::vec3 &d, const glm::vec3 &d_x_c) {
    float cba = glm::dot(c, b_x_a);
    float dba = glm::dot(d, b_x_a);
    float adc = glm::dot(a, d_x_c);
    float bdc = glm::dot(b, d_x_c);
    return cba * dba < 0.0f && adc * bdc < 0.0f && cba * bdc > 0.0f;
}

Face::Face(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2) {
    this->vertices = {v0, v1, v2};
    this->position = v0;
    this->normal = glm::normalize(glm::cross(v0 - v2, v1 - v0));
}

bool operator==(Face f1, const Face f2) {
    return f1.vertices.at(0) == f2.vertices.at(0) &&
           f1.vertices.at(1) == f2.vertices.at(1) &&
           f1.vertices.at(2) == f2.vertices.at(2);
}

Edge::Edge(const glm::vec3 &v0, const glm::vec3 &v1) {
    this->v0 = v0;
    this->v1 = v1;
}

bool operator==(Edge e1, const Edge e2) {
    return e1.v0 == e2.v0 &&
           e1.v1 == e2.v1;
}

Body::Body() {
        this->vertices = {};
        this->edges = {};
        this->faces = {};
        this->cm_position = glm::vec3(0,0,0);
}
// in world space
int Body::update(const std::vector<glm::vec3> &v) {

    this->faces = {
        Face(v.at(0), v.at(1), v.at(2)),
        Face(v.at(4), v.at(5), v.at(1)),
        Face(v.at(1), v.at(5), v.at(6)),
        Face(v.at(3), v.at(2), v.at(6)),
        Face(v.at(7), v.at(4), v.at(0)), 
        Face(v.at(7), v.at(6), v.at(5)),                      
    };

    this->edges = {
        Edge(v.at(0), v.at(1)),
        Edge(v.at(1), v.at(2)),
        Edge(v.at(2), v.at(3)),
        Edge(v.at(3), v.at(0)),
        Edge(v.at(4), v.at(5)),
        Edge(v.at(5), v.at(6)),
        Edge(v.at(6), v.at(7)),
        Edge(v.at(7), v.at(4)), 
        Edge(v.at(4), v.at(0)),
        Edge(v.at(5), v.at(1)),
        Edge(v.at(6), v.at(2)),
        Edge(v.at(7), v.at(3)),
    };

    this->edges[0].face_indices = {0, 1};
    this->edges[1].face_indices = {0, 2};
    this->edges[2].face_indices = {0, 3};
    this->edges[3].face_indices = {0, 4};
    this->edges[4].face_indices = {1, 5};
    this->edges[5].face_indices = {2, 5};
    this->edges[6].face_indices = {3, 5};
    this->edges[7].face_indices = {4, 5};
    this->edges[8].face_indices = {1, 4};
    this->edges[9].face_indices = {1, 2};
    this->edges[10].face_indices = {2, 3};
    this->edges[11].face_indices = {3, 4};

    this->vertices = v;

    return 0;

}

// find furthest along n
glm::vec3 get_support(const std::vector<glm::vec3> &vertices, glm::vec3 n) {
    glm::vec3 _v;
    float _d = -FLT_MAX;
    for (unsigned int i = 0; i < vertices.size(); i++) {
        glm::vec3 v = vertices.at(i);
        float d = glm::dot(v, n);
        if ( d > _d) {
            _d = d;
            _v = v;
        }
    }
    return _v;
}

bool operator==(Query q1, const Query q2) {
    return q1.max_seperation == q2.max_seperation && q1.max_index == q2.max_index;
}

Query query_face_directions(const Body &hullA, const Body &hullB) {
    float max_seperation = -FLT_MAX;
    std::vector<int> max_index;
    max_index.push_back(-1);
    max_index.push_back(-1);
    glm::vec3 best_axis;
    for (int i = 0; i < hullA.faces.size(); i++) {
        Face f = hullA.faces.at(i);
        glm::vec3 support_point = get_support(hullB.vertices, f.normal * -1.0f);
        float dist = pointPlaneDistance(support_point, f.position, f.normal);
        if (dist > max_seperation) {
            max_index = {i, -1};
            max_seperation = dist;
            best_axis = f.normal;
        }
    }
    return Query(max_seperation, max_index, best_axis);
}


Query query_edge_directions(const Body &hullA, const Body &hullB) {
    float max_seperation = -FLT_MAX;
    std::vector<int> max_index;
    max_index.push_back(-1);
    max_index.push_back(-1);
    glm::vec3 best_axis;

    for (int i = 0; i < hullA.edges.size(); i++) {
        Edge edge_a = hullA.edges.at(i);
        glm::vec3 edge_a_n1 = hullA.faces[edge_a.face_indices[0]].normal;
        glm::vec3 edge_a_n2 = hullA.faces[edge_a.face_indices[1]].normal;

        for (int j = 0; j < hullB.edges.size(); j++) {
            Edge edge_b = hullB.edges.at(j);
            glm::vec3 edge_b_n1 = hullB.faces[edge_b.face_indices[0]].normal;
            glm::vec3 edge_b_n2 = hullB.faces[edge_b.face_indices[1]].normal;
            // negate last two values for minkowski difference
            bool builds_face = isMinkowskiFace(edge_a_n1, edge_a_n2, glm::cross(edge_a_n1, edge_a_n2), edge_b_n1 * -1.0f, edge_b_n2 * -1.0f, glm::cross(edge_b_n1 * -1.0f, edge_b_n2 * -1.0f));

            if (!builds_face) {
                continue;
            }
            glm::vec3 axis = glm::normalize(glm::cross(edge_a.v1 - edge_a.v0, edge_b.v1 - edge_b.v0));
            // check edges arent parallel
            if (glm::length(axis) < 0.0001f) {
                continue;
            }
            // check normal is pointing away from A
            if (glm::dot(axis, edge_a.v0 - hullA.cm_position) < 0.0f) {
                axis = axis * -1.0f;
            }

            float dist1 = pointPlaneDistance(edge_b.v0, edge_a.v0, axis);
            float dist2 = pointPlaneDistance(edge_b.v1, edge_a.v0, axis);
            float dist;
            if (dist1 > dist2) {
                dist = dist1;
            }
            else {
                dist = dist2;
            }

            // keep largest penetration
            if (max_seperation == -FLT_MAX || dist > max_seperation) {
                max_index = {i,j};
                max_seperation = dist;
                best_axis = axis;
            }

        }
    }
    return Query(max_seperation, max_index, best_axis);
}


std::vector<Query> SAT(const Body &hullA, const Body &hullB) {

    // and the cross product of the edges if they build a face on the minkowski
    Query edge_query = query_edge_directions(hullA, hullB);
    if (edge_query.max_seperation > 0.0f) {
        edge_query.type = 0;
        return {edge_query};
    }
    // test all normals of hull_a as axes
    Query face_query_a = query_face_directions(hullA, hullB);
    if (face_query_a.max_seperation > 0.0f) {
        face_query_a.type = 1;
        return {face_query_a};
    }

    // and all normals of hull_b as axes
    Query face_query_b = query_face_directions(hullA, hullB);
    if (face_query_b.max_seperation > 0.0f) {
        face_query_b.type = 2;
        return {face_query_b};
    }

    // return queries with smallest penetration
    bool face_contact_a = face_query_a.max_seperation > edge_query.max_seperation;
    bool face_contact_b = face_query_b.max_seperation > edge_query.max_seperation;
    if (face_contact_a && face_contact_b) {
        face_query_a.type = 1;
        face_query_b.type = 2;
        return {face_query_a, face_query_b};
    } else {
        edge_query.type = 0;
        return {edge_query};
    }
}

Frustum::Frustum(float _fov, float _nearDist, float _farDist, float _WIDTH, float _HEIGHT, Camera camera) {
    if (_fov != 0.0f) {
        fov = _fov;
        nearDist = _nearDist;
        farDist = _farDist;
        ar = (float)_WIDTH / (float)_HEIGHT;


        // float fov = 45.0f;
        // float nearDist = 0.1f;
        // float farDist = 1000.0f;
        // float ar = (float)WIDTH / (float)HEIGHT;

        float Hnear;
        float Wnear;
        float Hfar;
        float Wfar;

        // perspective
        Hnear = 2* tan(radians(fov/2)) * nearDist;
        Wnear = Hnear * ar;
        Hfar = 2* tan(radians(fov/2)) * farDist;
        Wfar = Hfar * ar; 


        // calculate frustum data per frame
        Cnear = camera.Position + glm::normalize(camera.Front) * nearDist;
        Cfar = camera.Position + glm::normalize(camera.Front) * farDist;

        topRightFar = Cfar + (camera.Up * (Hfar / 2)) + (camera.Right * (Wfar / 2));
        bottomRightFar = Cfar - (camera.Up * (Hfar / 2)) + (camera.Right * (Wfar / 2));

        topLeftFar =  Cfar + (camera.Up * (Hfar / 2)) - (camera.Right * (Wfar / 2));
        bottomLeftFar =  Cfar - (camera.Up * (Hfar / 2)) - (camera.Right * (Wfar / 2));

        topRightNear = Cnear + (camera.Up * (Hnear / 2)) + (camera.Right * (Wnear / 2));
        topLeftNear =  Cnear + (camera.Up * (Hnear / 2)) - (camera.Right * (Wnear / 2));

        bottomLeftNear = Cnear - (camera.Up * (Hnear /2)) - (camera.Right * (Wnear / 2));
        bottomRightNear = Cnear - (camera.Up * (Hnear /2)) + (camera.Right * (Wnear / 2));

        vec3 aux = glm::normalize((Cnear + camera.Right * (float)(Wnear / 2)) - camera.Position);
        rightNormal = glm::normalize(glm::cross(aux, camera.Up));
        aux = glm::normalize((Cnear - camera.Right * (float)(Wnear / 2)) - camera.Position);
        leftNormal = glm::normalize(glm::cross(aux, camera.Up));

        aux = glm::normalize((Cnear + camera.Up * (float)(Hnear / 2)) - camera.Position);
        topNormal =  glm::normalize(glm::cross(aux, camera.Right));

        aux = glm::normalize((Cnear - camera.Up * (float)(Hnear / 2)) - camera.Position);
        bottomNormal =  glm::normalize(glm::cross(aux, camera.Right));

        backNormal = camera.Front;
        frontNormal = -1.0f * camera.Front;
        std::vector<glm::vec2> screenPoints = { glm::vec2(0,0), glm::vec2(_WIDTH, 0),
                                                glm::vec2(_WIDTH, _HEIGHT), glm::vec2(0, _HEIGHT)};

        float fovx = glm::degrees(2.0f * atan(tan(radians(fov/2.0f)) * ar));
        float diagonalNearDist = nearDist / (cos(radians(fovx/2.0f)) * cos(radians(fov/2.0f)));
        float diagonalFarDist = farDist / (cos(radians(fovx/2.0f)) * cos(radians(fov/2.0f)));

        std::vector<glm::vec3> frustumVertices = {};
        for (unsigned int i = 0; i < screenPoints.size(); i++) {
            glm::vec2 p = screenPoints.at(i);
            glm::vec3 ray_wor = rayCast(p.x, _HEIGHT-p.y, camera.view_matrix, camera.projection_matrix, _WIDTH, _HEIGHT);
            glm::vec3 coord1 = camera.Position + ray_wor * diagonalNearDist;
            frustumVertices.push_back(coord1);
        }
        for (unsigned int i = 0; i < screenPoints.size(); i++) {
            glm::vec2 p = screenPoints.at(i);
            glm::vec3 ray_wor = rayCast(p.x, _HEIGHT-p.y, camera.view_matrix, camera.projection_matrix, _WIDTH, _HEIGHT);
            glm::vec3 coord1 = camera.Position + ray_wor * diagonalFarDist;
            frustumVertices.push_back(coord1);
        }

        bottomLeftNear = frustumVertices.at(0);
        bottomRightNear = frustumVertices.at(1);
        topRightNear = frustumVertices.at(2);
        topLeftNear = frustumVertices.at(3);
        bottomLeftFar = frustumVertices.at(4);
        bottomRightFar = frustumVertices.at(5);
        topRightFar = frustumVertices.at(6);
        topLeftFar = frustumVertices.at(7);

        frustum_body.update({topLeftNear, bottomLeftNear, bottomRightNear, topRightNear, topLeftFar, bottomLeftFar, bottomRightFar, topRightFar});
    }
    //  else {
    //     bottomLeftNear = frustumVertices.at(0);
    //     bottomRightNear = frustumVertices.at(1);
    //     topRightNear = frustumVertices.at(2);
    //     topLeftNear = frustumVertices.at(3);
    //     bottomLeftFar = frustumVertices.at(4);
    //     bottomRightFar = frustumVertices.at(5);
    //     topRightFar = frustumVertices.at(6);
    //     topLeftFar = frustumVertices.at(7);

    //     frustum_body.update({topLeftNear, bottomLeftNear, bottomRightNear, topRightNear, topLeftFar, bottomLeftFar, bottomRightFar, topRightFar});
    // }

}

// @todo implement polymorphic behaviour to cull static and animated objects
void Frustum::cullStaticObject(StaticObject &object) {
    StaticModel *statModel = object.model.get();

    mat4 model = object.modelMatrix;

    for (unsigned int j = 0; j < statModel->meshes.size(); j++) {
        StaticMesh *mesh = &statModel->meshes[j];
        mesh->frustumCulled = false;                
        object.meshIsCulled[j] = 0;

        Body box_body;

        vector<float> aabb = mesh->getAABB();
        vec3 _min = vec3(aabb[0], aabb[1], aabb[2]);
        vec3 _max = vec3(aabb[3], aabb[4], aabb[5]);

        glm::vec3 v5 = glm::vec3(model * glm::vec4(_min.x, _min.y, _min.z, 1.0f));
        glm::vec3 v3 = glm::vec3(model * glm::vec4(_max.x, _max.y, _max.z, 1.0f));
        glm::vec3 v4 = glm::vec3(model * glm::vec4(_min.x, _max.y, _min.z, 1.0f));
        glm::vec3 v6 = glm::vec3(model * glm::vec4(_max.x, _min.y, _min.z, 1.0f));
        glm::vec3 v7 = glm::vec3(model * glm::vec4(_max.x, _max.y, _min.z, 1.0f));
        glm::vec3 v2 = glm::vec3(model * glm::vec4(_max.x, _min.y, _max.z, 1.0f));
        glm::vec3 v0 = glm::vec3(model * glm::vec4(_min.x, _max.y, _max.z, 1.0f));
        glm::vec3 v1 = glm::vec3(model * glm::vec4(_min.x, _min.y, _max.z, 1.0f));
        std::vector<glm::vec3> v = {v0, v1, v2, v3, v4, v5, v6, v7};
        box_body.update(v);


        glm::vec3 cm_position(0,0,0);
        for (unsigned int i = 0; i < v.size(); i++) {
            cm_position += v.at(i);
        }
        box_body.cm_position = cm_position * (1.0f / v.size());

        std::vector<Query> queries = SAT(box_body, frustum_body);
        float max_seperation = -FLT_MAX;
        for (unsigned int i = 0; i < queries.size(); i++) {
            Query query = queries[i];
            if (query.max_seperation > max_seperation) {
                max_seperation = query.max_seperation;
            }
        }

        if (max_seperation > 0.0f) {
            object.meshIsCulled[j] = 1;
            mesh->frustumCulled = 1;
        }
    }
}

// @todo implement polymorphic behaviour to cull static and animated objects
void Frustum::cullAnimatedObject(AnimatedObject &object) {
    AnimatedModel *animModel = object.model.get();

    mat4 model = object.modelMatrix;

    for (unsigned int j = 0; j < animModel->meshes.size(); j++) {

        StaticMesh *mesh = &animModel->meshes[j];
        mesh->frustumCulled = false;      
        object.meshIsCulled[j] = 0;
        Body box_body;

        vector<float> aabb = mesh->getAABB();
        vec3 _min = vec3(aabb[0], aabb[1], aabb[2]);
        vec3 _max = vec3(aabb[3], aabb[4], aabb[5]);

        glm::vec3 v5 = glm::vec3(model * glm::vec4(_min.x, _min.y, _min.z, 1.0f));
        glm::vec3 v3 = glm::vec3(model * glm::vec4(_max.x, _max.y, _max.z, 1.0f));
        glm::vec3 v4 = glm::vec3(model * glm::vec4(_min.x, _max.y, _min.z, 1.0f));
        glm::vec3 v6 = glm::vec3(model * glm::vec4(_max.x, _min.y, _min.z, 1.0f));
        glm::vec3 v7 = glm::vec3(model * glm::vec4(_max.x, _max.y, _min.z, 1.0f));
        glm::vec3 v2 = glm::vec3(model * glm::vec4(_max.x, _min.y, _max.z, 1.0f));
        glm::vec3 v0 = glm::vec3(model * glm::vec4(_min.x, _max.y, _max.z, 1.0f));
        glm::vec3 v1 = glm::vec3(model * glm::vec4(_min.x, _min.y, _max.z, 1.0f));
        std::vector<glm::vec3> v = {v0, v1, v2, v3, v4, v5, v6, v7};
        box_body.update(v);


        glm::vec3 cm_position(0,0,0);
        for (unsigned int i = 0; i < v.size(); i++) {
            cm_position += v.at(i);
        }
        box_body.cm_position = cm_position * (1.0f / v.size());

        std::vector<Query> queries = SAT(box_body, frustum_body);
        float max_seperation = -FLT_MAX;
        for (unsigned int i = 0; i < queries.size(); i++) {
            Query query = queries[i];
            if (query.max_seperation > max_seperation) {
                max_seperation = query.max_seperation;
            }
        }

        if (max_seperation > 0.0f) {
            object.meshIsCulled[j] = 1;
            mesh->frustumCulled = 1;      
        }
    }
}

void Frustum::reset(AnimatedObject &object) {
    AnimatedModel *animModel = object.model.get();

    for (unsigned int j = 0; j < animModel->meshes.size(); j++) {

        object.meshIsCulled[j] = 0;
    }
}