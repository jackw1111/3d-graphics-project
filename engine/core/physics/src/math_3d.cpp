#include "math_3d.h"

mat3 orthonormalizeOrientation(mat3 m)
{
    m[0] = normalize(m[0]);
    m[2] = normalize(cross(m[0], m[1]));
    m[1] = normalize(cross(m[2], m[0]));
    return m;
}

mat3 setSkewSymmetric(vec3 const &vector)
{
    mat3 tmp = mat3(vec3(0.0, vector.z, -vector.y), vec3(-vector.z,0,vector.x), vec3(vector.y,-vector.x,0));
    return tmp;
}

vector<vec3> getAABBVertices(const vec3 &_min, const vec3 &_max) {
    vec3 point1 = vec3(_min.x, _min.y, _min.z);
    vec3 point2 = vec3(_max.x, _max.y, _max.z);
    vec3 point3 = vec3(point1.x, point2.y, point1.z);
    vec3 point4 = vec3(point2.x, point1.y, point1.z);
    vec3 point5 = vec3(point2.x, point2.y, point1.z);
    vec3 point6 = vec3(point2.x, point1.y, point2.z);
    vec3 point7 = vec3(point1.x, point2.y, point2.z);
    vec3 point8 = vec3(point1.x, point1.y, point2.z);
    vector<vec3> v = {point8, point6, point2, point7, point1, point4, point5, point3};
    return v;
}

float clamp(float value, float _min, float _max) {
    return std::min(std::max(value, _min), _max);
}

vector<float> getAABB(const vector<vec3> &vertices) {
    float minX = FLT_MAX;
    float maxX = -FLT_MAX;
    float minY = FLT_MAX;
    float maxY = -FLT_MAX;
    float minZ = FLT_MAX;
    float maxZ = -FLT_MAX;

    for (unsigned int i = 0; i < vertices.size(); i++) {
        if (vertices.at(i).x < minX) {
            minX = vertices.at(i).x;
        }
        else if (vertices.at(i).x > maxX) {
            maxX = vertices.at(i).x;
        }
        if (vertices.at(i).y < minY) {
            minY = vertices.at(i).y;
        }
        else if (vertices.at(i).y > maxY) {
            maxY = vertices.at(i).y;
        }
        if (vertices.at(i).z < minZ) {
            minZ = vertices.at(i).z;
        }
        else if (vertices.at(i).z > maxZ) {
            maxZ = vertices.at(i).z;
        }
    }
    vector<float> limits = {minX, minY, minZ, maxX, maxY, maxZ};
    return limits;
}

bool AABBOverlap(const vec3 &_min1, const vec3 &_max1, const vec3 &_min2, const vec3 &_max2) {
    return _max2.x > _min1.x && _max1.x > _min2.x &&
           _max2.y > _min1.y && _max1.y > _min2.y &&
           _max2.z > _min1.z && _max1.z > _min2.z;
}

float distPointToTriangle(const vec3 &point, const vec3 &v1, const vec3 &v2, const vec3 &v3) {
    vec3 n = normalize(cross(v2 - v1, v3 - v1));
    // 1. project point onto plane of triangle
    vec3 projected_point = point - n * dot(point, n);
    // 2. inside-outside test for each edge
    vec3 e1 = v2 - v1;
    vec3 e2 = v3 - v2;
    vec3 e3 = v1 - v3;
    float min_dist = -FLT_MAX;
    vec3 c1 = projected_point - v1; 
    vec3 c2 = projected_point - v2;
    vec3 c3 = projected_point - v3;
    float d1 = dot(n, cross(e1, c1));
    if (d1 < 0) {
        // outside of edge 1
        if (d1 > min_dist) {
            min_dist = d1;
        }
    }
    float d2 = dot(n, cross(e2, c2));
    if (d2 < 0) {
        // outside of edge 2
        if (d2 > min_dist) {
            min_dist = d2;
        }
    }
    float d3 = dot(n, cross(e3, c3));
    if (d3 < 0) {
        // outside of edge 3
        if (d3 > min_dist) {
            min_dist = d3;
        }
    }

    return min_dist;
}

vec3 matrixMultiply(const mat4 &m, const vec3 &v) {
    vec4 v4 = vec4(v.x, v.y, v.z, 1.0f);
    vec3 v1 = m * v4;
    return vec3(v1.x, v1.y, v1.z);
}

float pointLineIntersection(const vec3 &p3, const vec3 &p1, const vec3 &p2) {

    float x1 = p1.x;
    float y1 = p1.y;
    float z1 = p1.z;
    float x2 = p2.x;
    float y2 = p2.y;
    float z2 = p2.z;
    float x3 = p3.x;
    float y3 = p3.y;
    float z3 = p3.z;

    float n = (x3-x1)*(x2-x1) + (y3-y1)*(y2-y1) + (z3-z1)*(z2-z1);
    float d = pow(length(p2 - p1),2);
    if (d == 0) {
        return numeric_limits<float>::infinity();
    }
    float u =  n / d;
    return u; // p = p1 + u*(p2-p1)
}

vec2 lineSegmentIntersection(const vec3 &p1, const vec3 &p2, const vec3 &p3, const vec3 &p4) {
    float d1343 = dot(p1 - p3, p4 - p3);
    float d4321 = dot(p4 - p3, p2 - p1);
    float d1321 = dot(p1 - p3, p2 - p1);
    float d4343 = dot(p4 - p3, p4 - p3);
    float d2121 = dot(p2 - p1, p2 - p1);
    float mu_a = (d1343*d4321-d1321*d4343)/(d2121*d4343-d4321*d4321);
    float mu_b = (d1343 + mu_a*d4321) / d4343;
    return vec2(mu_a, mu_b);
}

vec2 closestPointsOnLineSegments(
    const vec3 &P0, const vec3 &P1,
    const vec3 &Q0, const vec3 &Q1)
{
    float const SMALL_NUM = 0.00000001f;
    vec3   u = P1 - P0;
    vec3   v = Q1 - Q0;
    vec3   w = P0 - Q0;
    float    a = dot(u, u);         // always >= 0
    float    b = dot(u, v);
    float    c = dot(v, v);         // always >= 0
    float    d = dot(u, w);
    float    e = dot(v, w);
    float    D = a*c - b*b;        // always >= 0
    float    sc, sN, sD = D;       // sc = sN / sD, default sD = D >= 0
    float    tc, tN, tD = D;       // tc = tN / tD, default tD = D >= 0

    // compute the line parameters of the two closest points
    if (D < SMALL_NUM) { // the lines are almost parallel
        sN = 0.0f;         // force using point P0 on segment S1
        sD = 1.0f;         // to prevent possible division by 0.0 later
        tN = e;
        tD = c;
    }
    else {                 // get the closest points on the infinite lines
        sN = (b*e - c*d);
        tN = (a*e - b*d);
        if (sN < 0.0f) {        // sc < 0 => the s=0 edge is visible
            sN = 0.0f;
            tN = e;
            tD = c;
        }
        else if (sN > sD) {  // sc > 1  => the s=1 edge is visible
            sN = sD;
            tN = e + b;
            tD = c;
        }
    }

    if (tN < 0.0f) {            // tc < 0 => the t=0 edge is visible
        tN = 0.0f;
        // recompute sc for this edge
        if (-d < 0.0f)
            sN = 0.0f;
        else if (-d > a)
            sN = sD;
        else {
            sN = -d;
            sD = a;
        }
    }
    else if (tN > tD) {      // tc > 1  => the t=1 edge is visible
        tN = tD;
        // recompute sc for this edge
        if ((-d + b) < 0.0f)
            sN = 0;
        else if ((-d + b) > a)
            sN = sD;
        else {
            sN = (-d + b);
            sD = a;
        }
    }
    // finally do the division to get sc and tc
    sc = (std::fabs(sN) < SMALL_NUM ? 0.0f : sN / sD);
    tc = (std::fabs(tN) < SMALL_NUM ? 0.0f : tN / tD);

    // get the difference of the two closest points
    float s = sc;
    float t = tc;
    return vec2(s,t);
}


float pointPlaneDistance2(const vec3 &point, const vec3 &planePosition, const vec3 &planeNormal) {
    return dot(point - planePosition, planeNormal);
}

// Sutherland-Hodgeman clipping algorithm
vector<vec3> clipPolygonToPlane(const vector<vec3> &polygon, const vec3 &planePosition, const vec3 &planeNormal) {
    vector<vec3> out = {};

    if (polygon.size() == 0) {
        return out;
    }

    vec3 vertex1 = polygon.at(polygon.size() -1);
    float distance1 = pointPlaneDistance2(vertex1, planePosition, planeNormal);
    for (unsigned int i = 0; i < polygon.size(); i++) {
        vec3 vertex2 = polygon.at(i);
        float fraction = 0.0f;
        float distance2 = pointPlaneDistance2(vertex2, planePosition, planeNormal);
        if (distance1 <= 0.0f && distance2 <= 0.0f) {
            out.push_back(vertex2);
        } else if (distance1 <= 0.0f && distance2 > 0.0f) {
            fraction = distance1 / (distance1 - distance2);
            vec3 intersectionPoint = vertex1 + (vertex2 - vertex1) * fraction;
            out.push_back(intersectionPoint);
        } else if (distance2 <= 0.0f && distance1 > 0.0f) {
            fraction = distance1 / (distance1 - distance2);
            vec3 intersectionPoint = vertex1 + (vertex2 - vertex1) * fraction;
            out.push_back(intersectionPoint);
            out.push_back(vertex2);
        }

        // keep vertex2 as starting vertex for next edge
        vertex1 = vertex2;
        distance1 = distance2;
    }
    return out;
}

bool isMinkowskiFace2(const vec3 &a, const vec3 &b, const vec3 &b_x_a, const vec3 &c, const vec3 &d, const vec3 &d_x_c) {
    float cba = dot(c, b_x_a);
    float dba = dot(d, b_x_a);
    float adc = dot(a, d_x_c);
    float bdc = dot(b, d_x_c);
    return cba * dba < 0.0f && adc * bdc < 0.0f && cba * bdc > 0.0f;
}

vec3 closest_point_on_circle(vec3 p, vec3 plane_position, vec3 plane_normal, float r) {
    float d = pointPlaneDistance2(p, plane_position, plane_normal);
    vec3 closest_point_on_plane = p - plane_normal * d;
    if (glm::distance(closest_point_on_plane, plane_position) < r) {
        return closest_point_on_plane;
    } else {
        return plane_position + normalize(closest_point_on_plane - plane_position) * r;
    }
}

float sphere_plane_collision(glm::vec3 sphereCentre, glm::vec3 planeNormal, glm::vec3 pointOnPlane) {
    glm::vec3 v = sphereCentre - pointOnPlane;
    float d = glm::dot(v, planeNormal);

    // penetration depth along v
    return d;
}

bool ray_triangle_collision(glm::vec3 planeIntersection, glm::vec3 planeNormal, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
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

glm::vec3 closest_point_on_line_segment(glm::vec3 A, glm::vec3 B, glm::vec3 point) {
    glm::vec3 AB = B - A;
    float t = glm::dot(point - A, AB) / (0.00001f + glm::dot(AB, AB));
    return A + AB * std::min(std::max(t, 0.0f), 1.0f);
}

// Given point p, return point q on (or in) OBB b, closest to p
vec3 closest_point_on_obb(vec3 point, float box_dX2, float box_dY2, float box_dZ2, vec3 box_position, mat3 box_orientation) {
    vector<float> box_extents = {box_dX2, box_dY2, box_dZ2};
    vec3 d = point - box_position;
    // start result at center of box; make steps from there
    vec3 q = box_position;
    // For each OBB axis...
    for (unsigned i = 0; i < box_extents.size(); i++) {
        // ...project d onto that axis to get the distance
        //along the axis of d from the box center
        float dist = dot(d, box_orientation[i]);
        // If distance farther than the box extents, clamp to the box
        if (dist > box_extents.at(i)) {
            dist = box_extents.at(i);
        }
        if (dist < -box_extents.at(i)) {
            dist = -box_extents.at(i);
        }
        // Step that distance along the axis to get world coordinate
        q += box_orientation[i] * dist;
    }
    return q;
}

float ray_obb_intersection(vec3 point, vec3 dir, float box_dX2, float box_dY2, float box_dZ2, vec3 box_position, mat3 box_orientation) {
    vec3 q = closest_point_on_obb(point, box_dX2, box_dY2, box_dZ2, box_position, box_orientation);
    return dot(normalize(q - point), glm::normalize(point + dir));
}

float ray_obb_intersection(
    glm::vec3 ray_origin,        // Ray origin, in world space
    glm::vec3 ray_direction,     // Ray direction (NOT target position!), in world space. Must be normalize()'d.
    glm::vec3 aabb_min,          // Minimum X,Y,Z coords of the mesh when not transformed at all.
    glm::vec3 aabb_max,          // Maximum X,Y,Z coords. Often aabb_min*-1 if your mesh is centered, but it's not always the case.
    glm::mat4 ModelMatrix       // Transformation applied to the mesh (which will thus be also applied to its bounding box)
){

    float intersection_distance = 0.0f;
    
    // Intersection method from Real-Time Rendering and Essential Mathematics for Games
    
    float tMin = 0.0f;
    float tMax = 100000.0f;

    glm::vec3 OBBposition_worldspace(ModelMatrix[3].x, ModelMatrix[3].y, ModelMatrix[3].z);

    glm::vec3 delta = OBBposition_worldspace - ray_origin;

    // Test intersection with the 2 planes perpendicular to the OBB's X axis
    {
        glm::vec3 xaxis(ModelMatrix[0].x, ModelMatrix[0].y, ModelMatrix[0].z);
        float e = glm::dot(xaxis, delta);
        float f = glm::dot(ray_direction, xaxis);

        if ( fabs(f) > 0.001f ){ // Standard case

            float t1 = (e+aabb_min.x)/f; // Intersection with the "left" plane
            float t2 = (e+aabb_max.x)/f; // Intersection with the "right" plane
            // t1 and t2 now contain distances betwen ray origin and ray-plane intersections

            // We want t1 to represent the nearest intersection, 
            // so if it's not the case, invert t1 and t2
            if (t1>t2){
                float w=t1;t1=t2;t2=w; // swap t1 and t2
            }

            // tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
            if ( t2 < tMax )
                tMax = t2;
            // tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
            if ( t1 > tMin )
                tMin = t1;

            // And here's the trick :
            // If "far" is closer than "near", then there is NO intersection.
            // See the images in the tutorials for the visual explanation.
            if (tMax < tMin )
                return false;

        }else{ // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
            if(-e+aabb_min.x > 0.0f || -e+aabb_max.x < 0.0f)
                return false;
        }
    }


    // Test intersection with the 2 planes perpendicular to the OBB's Y axis
    // Exactly the same thing than above.
    {
        glm::vec3 yaxis(ModelMatrix[1].x, ModelMatrix[1].y, ModelMatrix[1].z);
        float e = glm::dot(yaxis, delta);
        float f = glm::dot(ray_direction, yaxis);

        if ( fabs(f) > 0.001f ){

            float t1 = (e+aabb_min.y)/f;
            float t2 = (e+aabb_max.y)/f;

            if (t1>t2){float w=t1;t1=t2;t2=w;}

            if ( t2 < tMax )
                tMax = t2;
            if ( t1 > tMin )
                tMin = t1;
            if (tMin > tMax)
                return false;

        }else{
            if(-e+aabb_min.y > 0.0f || -e+aabb_max.y < 0.0f)
                return false;
        }
    }


    // Test intersection with the 2 planes perpendicular to the OBB's Z axis
    // Exactly the same thing than above.
    {
        glm::vec3 zaxis(ModelMatrix[2].x, ModelMatrix[2].y, ModelMatrix[2].z);
        float e = glm::dot(zaxis, delta);
        float f = glm::dot(ray_direction, zaxis);

        if ( fabs(f) > 0.001f ){

            float t1 = (e+aabb_min.z)/f;
            float t2 = (e+aabb_max.z)/f;

            if (t1>t2){float w=t1;t1=t2;t2=w;}

            if ( t2 < tMax )
                tMax = t2;
            if ( t1 > tMin )
                tMin = t1;
            if (tMin > tMax)
                return false;

        }else{
            if(-e+aabb_min.z > 0.0f || -e+aabb_max.z < 0.0f)
                return false;
        }
    }

    intersection_distance = tMin;
    return true;

}
