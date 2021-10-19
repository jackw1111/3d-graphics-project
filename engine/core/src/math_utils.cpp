#include "math_utils.h"
#include "camera.h"
#include "static_model.h"


//extern Camera camera;
extern std::vector<StaticModel> models;
extern unsigned int WIDTH;
extern unsigned int HEIGHT;

glm::vec3 getPosition(glm::mat4 mat){
  glm::vec4 v4 = mat[3];
  return glm::vec3(v4.x, v4.y, v4.z);
}

float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}  

vec3 rayCast(double xpos, double ypos, mat4 projection, mat4 view) {
    // converts a position from the 2d xpos, ypos to a normalized 3d direction
    float x = (2.0f * xpos) / WIDTH - 1.0f;
    float y = 1.0f - (2.0f * ypos) / HEIGHT;
    float z = 1.0f;
    vec3 ray_nds = vec3(x, y, z);
    vec4 ray_clip = vec4(ray_nds.x, ray_nds.y, -1.0f, 1.0f);
    // eye space to clip we would multiply by projection so
    // clip space to eye space is the inverse projection
    vec4 ray_eye = inverse(projection) * ray_clip;
    // convert point to forwards
    ray_eye = vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);
    // world space to eye space is usually multiply by view so
    // eye space to world space is inverse view
    vec4 inv_ray_wor = (inverse(view) * ray_eye);
    vec3 ray_wor = vec3(inv_ray_wor.x, inv_ray_wor.y, inv_ray_wor.z);
    ray_wor = normalize(ray_wor);
    return ray_wor;
}

bool solveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1) {
  // scratch a pixel
  float discr = b * b - 4 * a * c;
  if (discr < 0) {
    return false;
  }
  else if (discr == 0)
  {
    x0 = x1 = -0.5 * b / a;
  } else {
    float q = ( b > 0 ) ? - 0.5 * (b + sqrt(discr)) :
                        - 0.5 * (b - sqrt(discr));
    x0 = q / a;
    x1 = c / q;
  }
  if (x0 > x1) {
    std::swap(x0, x1);
  }
  return true;
}

bool intersect(const vec3 &pos, const vec3 &ray, const vec3 &sphere, float radius2) {
  // scratch a pixel
  float t0, t1; // soluitions for t if the ray intersects
#if 0
  // geometric solution
  vec3 L = sphere - camera.Position;
  float tca = dot(L, ray_wor);
  // if (tca < 0) return false;
  float d2 = dot(L, L) - tca * tca;
  if (d2 > radius2)
    return false;
  float thc = sqrt(radius2 - d2);
  t0 = tca - thc;
  t1 = tca + thc;
#else
  // analytic solution
  vec3 L = pos - sphere;
  float a = dot(ray, ray);
  float b = 2 * dot(ray, L);
  float c = dot(L, L) - radius2;
  if (!solveQuadratic(a, b, c, t0, t1)) return false;
#endif // 0
  if (t0 > t1) std::swap(t0, t1);
  if (t0 < 0) {
    t0 = t1; // if t0 is negative lets use t1 instead
    if (t0 < 0)
      return false; // both t0 and t1 are negative
  }
  //sphereT = t0;
  //new_pos = camera.Position + t * ray;
  return true;
}

/*bool intersect(const vec3 &ray, const vec3 &sphere, float &sphereT) {
  float radius2 = 10.0f;
  // scratch a pixel
  float t0, t1; // soluitions for t if the ray intersects
#if 0
  // geometric solution
  vec3 L = sphere - camera.Position;
  float tca = dot(L, ray_wor);
  // if (tca < 0) return false;
  float d2 = dot(L, L) - tca * tca;
  if (d2 > radius2)
    return false;
  float thc = sqrt(radius2 - d2);
  t0 = tca - thc;
  t1 = tca + thc;
#else
  // analytic solution
  vec3 L = camera.Position - sphere;
  float a = dot(ray, ray);
  float b = 2 * dot(ray, L);
  float c = dot(L, L) - radius2;
  if (!solveQuadratic(a, b, c, t0, t1)) return false;
#endif // 0
  if (t0 > t1) std::swap(t0, t1);
  if (t0 < 0) {
    t0 = t1; // if t0 is negative lets use t1 instead
    if (t0 < 0)
      return false; // both t0 and t1 are negative
  }
  sphereT = t0;
  //new_pos = camera.Position + t * ray;
  return true;
}*/

float intersectPlane(vec3 n, vec3 p0, vec3 l0, vec3 l)
{
  // scratch a pixel
    // assuming vectors are all normalized
    float denom = dot(l,n);
    vec3 p0l0 = p0 - l0;
    float t = dot(p0l0, n) / denom;


    return t;
}

float rayTriangleIntersect(
    const vec3 &rayOrigin, const vec3 &rayVector,
    const vec3 &vertex0, const vec3 &vertex1, const vec3 &vertex2)
{
  float t;
  // Moller-trumbore intersection algorithm
  const float EPSILON = 0.0000001f;
  vec3 edge1, edge2, h, s, q;
  float a, f, u, v;
  edge1 = vertex1 - vertex0;
  edge2 = vertex2 - vertex0;
  h = cross(rayVector, edge2);
  a = dot(edge1, h);
  if (a > -EPSILON && a < EPSILON)
    return false; // this ray is parallel to this triangle plane
  f = 1.0f/a;
  s = rayOrigin - vertex0;
  u = f * dot(s, h);
  if (u < 0.0f || u > 1.0f)
    return false;
  q = cross(s, edge1);
  v = f * dot(rayVector, q);
  if (v < 0.0f || u + v > 1.0f)
    return false;
  // at this stage we can compute t to find out where the intersection point is on the line
  t = f * dot(edge2, q);
  if (t > EPSILON && t < 1/EPSILON) // ray intersection
  {
    return t;
  }
  else // line intersection but no ray intersection
    return 0.0f;
}
// same as above but returns true if triangle intersects anywhere on line
bool alongRayTriangleIntersect(
    const vec3 &rayOrigin, const vec3 &rayVector,
    const vec3 &vertex0, const vec3 &vertex1, const vec3 &vertex2,
    float &t)
{
  // Moller-trumbore intersection algorithm
  const float EPSILON = 0.0000001f;
  vec3 edge1, edge2, h, s, q;
  float a, f, u, v;
  edge1 = vertex1 - vertex0;
  edge2 = vertex2 - vertex0;
  h = cross(rayVector, edge2);
  a = dot(edge1, h);
  if (a > -EPSILON && a < EPSILON)
    return false; // this ray is parallel to this triangle plane
  f = 1.0f/a;
  s = rayOrigin - vertex0;
  u = f * dot(s, h);
  if (u < 0.0f || u > 1.0f)
    return false;
  q = cross(s, edge1);
  v = f * dot(rayVector, q);
  if (v < 0.0f || u + v > 1.0f)
    return false;
  // at this stage we can compute t to find out where the intersection point is on the line
  t = f * dot(edge2, q);
  return true;
}
float rayObjectIntersect(glm::vec3 rayOrigin, glm::vec3 rayDirection, StaticObject object)
{   
  std::vector<float> intersect_values = {};
  for (unsigned int j = 0; j < object.model.get()->meshes.size(); j++) {
    std::vector<Vertex> vertices = object.model.get()->meshes[j].vertices;
    for (unsigned int i = 0; i < vertices.size(); i+=3) {
      vec3 v1 = glm::vec3(object.getModelMatrix() * glm::vec4(vertices.at(i+0).Position, 1.0));
      vec3 v2 = glm::vec3(object.getModelMatrix() * glm::vec4(vertices.at(i+1).Position, 1.0));
      vec3 v3 = glm::vec3(object.getModelMatrix() * glm::vec4(vertices.at(i+2).Position, 1.0));
      float t = rayTriangleIntersect(rayOrigin, rayDirection, v1, v2, v3);
      if (t) {
        intersect_values.push_back(t);
      }
    }
  }
  if (intersect_values.size()) {
    float shortest_distance = FLT_MAX;
    for (unsigned int i = 0; i < intersect_values.size(); i++) {
        if (intersect_values.at(i) < shortest_distance) {
          shortest_distance = intersect_values.at(i);
        }
    }
    return shortest_distance;
  } else {
    return 0.0f;
  }
}

/*void checkCameraOcclusion(const vec3 &ray_wor, const vec3 &sphere, float &sphereT, float &triangleT, float &distanceFromCamera, const float &setDistance) {
 for (unsigned int i = 0; i < models.size(); i++){
      BaseModel model = models.at(i);

      for (unsigned int j = 0; j < model.meshes.size(); j++) {
        BaseMesh mesh = model.meshes.at(j);

        for (unsigned int k=0; k<mesh.vertices.size(); k) {
          //std::cout << mesh.vertices.size() << std::endl;
          glm::vec3 a, b, c;
          a = mesh.vertices.at(k++).Position;
          b = mesh.vertices.at(k++).Position;
          c = mesh.vertices.at(k++).Position;
          bool lineTriangleIntersection = alongRayTriangleIntersect(camera.Position, ray_wor, a, b, c, triangleT);
          if (lineTriangleIntersection){
            bool triangleIntersection = alongRayTriangleIntersect(camera.Position, ray_wor, a, b, c, triangleT);
            bool sphereIntersection = intersect(ray_wor, sphere, sphereT);
            if (triangleT < sphereT)
                distanceFromCamera = 5.0f;
              return;
          } else {
              distanceFromCamera = setDistance;
          }
        }
      }
    }
}*/

bool checkCollision(vec2 ballPosition, vec2 ballSize, vec2 brickPosition, vec2 brickSize) {
    if ((ballPosition.x + ballSize.x/2) > (brickPosition.x - brickSize.x/2))
    {
      if ((ballPosition.x - ballSize.x/2) < (brickPosition.x + brickSize.x/2))
      {
        if ((ballPosition.y + ballSize.y/2) > (brickPosition.y - brickSize.y/2)){
          if ((ballPosition.y - ballSize.y/2) < (brickPosition.y + brickSize.y/2)){
            return true;
          }
        }
      }
    }
    return false;
}

float distance(vec2 p1, vec2 p2) {
  return abs(sqrt((p2.x - p1.x)*(p2.x - p1.x) + (p2.y - p1.y)*(p2.y - p1.y)));
}

int round(float value, int denomination) {
  return ((int)value % denomination) * denomination;
}

int Poly::setup(glm::mat4 modelMatrix) {
  //this->model = model;
  this->modelMatrix = modelMatrix;
  //this->getTranslatedVertices(modelMatrix);
  return 0;
}
int Poly::setup2(std::vector<Vertex> vertices) {
  this->vertices = vertices;
  this->getTranslatedVertices(vertices);
  return 0;
}

int Poly::getTranslatedVertices(std::vector<Vertex> vertices) {
  this->translated_vertices = {};
  for(unsigned int i = 0; i < vertices.size(); i++) {
    glm::vec3 v = vertices.at(i).Position;
    glm::vec4 _v = glm::vec4(v.x, v.y, v.z, 1.0);
    glm::vec4 tv = modelMatrix * _v;
    this->translated_vertices.push_back(glm::vec3(tv.x, tv.y, tv.z));
  }
  return 0;
}

bool check_collision(BCube boundingCube1, BCube boundingCube2) {
  BCube *poly1 = &boundingCube1;
  BCube *poly2 = &boundingCube2;
  for (unsigned int shape = 0; shape < 2; shape++) {
    if (shape == 1) {
      poly1 = &boundingCube2;
      poly2 = &boundingCube1;
    }
    for (unsigned int a = 0; a < poly1->translated_vertices.size(); a++) {
      float b = (a + 1) % poly1->translated_vertices.size();
      float axisProjx = -(poly1->translated_vertices[b].y - poly1->translated_vertices[a].y);
      float axisProjy = poly1->translated_vertices[b].x - poly1->translated_vertices[a].x;
 
      float minR1_xy = FLT_MAX;
      float maxR1_xy = -FLT_MAX;
      for (unsigned int p = 0; p < poly1->translated_vertices.size(); p++) {
        glm::vec2 translated_vertex = glm::vec2(poly1->translated_vertices[p].x, poly1->translated_vertices[p].y);
        float q = glm::dot(translated_vertex, glm::vec2(axisProjx, axisProjy));
        minR1_xy = std::min(minR1_xy, q);
        maxR1_xy = std::max(maxR1_xy, q);
      }

      float minR2_xy = FLT_MAX;
      float maxR2_xy = -FLT_MAX;
      for (unsigned int p = 0; p < poly2->translated_vertices.size(); p++) {
        glm::vec2 translated_vertex = glm::vec2(poly2->translated_vertices[p].x, poly2->translated_vertices[p].y);
        float q = glm::dot(translated_vertex, glm::vec2(axisProjx, axisProjy));
        minR2_xy = std::min(minR2_xy, q);
        maxR2_xy = std::max(maxR2_xy, q);
      }
 
      axisProjx = -(poly1->translated_vertices[b].z - poly1->translated_vertices[a].z);
      float axisProjz = poly1->translated_vertices[b].x - poly1->translated_vertices[a].x;
      float minR1_xz = FLT_MAX;
      float maxR1_xz = -FLT_MAX;
      for (unsigned int p = 0; p < poly1->translated_vertices.size(); p++) {
        glm::vec2 translated_vertex = glm::vec2(poly1->translated_vertices[p].x, poly1->translated_vertices[p].z);
        float q = glm::dot(translated_vertex, glm::vec2(axisProjx, axisProjz));
        minR1_xz = std::min(minR1_xz, q);
        maxR1_xz = std::max(maxR1_xz, q);
      }

      float minR2_xz = FLT_MAX;
      float maxR2_xz = -FLT_MAX;
      for (unsigned int p = 0; p < poly2->translated_vertices.size(); p++) {
        glm::vec2 translated_vertex = glm::vec2(poly2->translated_vertices[p].x, poly2->translated_vertices[p].z);
        float q = glm::dot(translated_vertex, glm::vec2(axisProjx, axisProjz));
        minR2_xz = std::min(minR2_xz, q);
        maxR2_xz = std::max(maxR2_xz, q);
      }
 
      axisProjy = -(poly1->translated_vertices[b].z - poly1->translated_vertices[a].z);
      axisProjz = poly1->translated_vertices[b].y - poly1->translated_vertices[a].y;
 
      float minR1_yz = FLT_MAX;
      float maxR1_yz = -FLT_MAX;
      for (unsigned int p = 0; p < poly1->translated_vertices.size(); p++) {
        glm::vec2 translated_vertex = glm::vec2(poly1->translated_vertices[p].y, poly1->translated_vertices[p].z);
        float q = glm::dot(translated_vertex, glm::vec2(axisProjy, axisProjz));
        minR1_yz = std::min(minR1_yz, q);
        maxR1_yz = std::max(maxR1_yz, q);
      }
 
      float minR2_yz = FLT_MAX;
      float maxR2_yz = -FLT_MAX;
      for (unsigned int p = 0; p < poly2->translated_vertices.size(); p++) {
          glm::vec2 translated_vertex = glm::vec2(poly2->translated_vertices[p].y, poly2->translated_vertices[p].z);
          float q = glm::dot(translated_vertex, glm::vec2(axisProjy, axisProjz));
          minR2_yz = std::min(minR2_yz, q);
          maxR2_yz = std::max(maxR2_yz, q);
      }
        // seperation between shadows of objects found => no collision possible, exit early
        if (!(maxR2_xy >= minR1_xy && maxR1_xy >= minR2_xy 
          && maxR2_xz >= minR1_xz && maxR1_xz >= minR2_xz 
          && maxR2_yz >= minR1_yz && maxR1_yz >= minR2_yz)) {
            return false;
        }
      
    }
  }

  return true;
}
