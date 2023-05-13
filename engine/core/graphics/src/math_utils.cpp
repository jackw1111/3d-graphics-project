#include "math_utils.h"
#include "camera.h"
#include "static_model.h"
#include "line3d.h"


//extern Camera camera;
extern std::vector<StaticModel> models;
extern unsigned int WIDTH;
extern unsigned int HEIGHT;

std::vector<BoundingBox*> BoundingBox::allBoundingBoxes = {};

glm::vec3 find_orthogonal(glm::vec3 v) {
    if (fabs(v.x) >=  1.0f / sqrt(3.0f)){
        return glm::normalize(glm::vec3(v.y, -v.x, 0.0f));
    }else{
        return glm::normalize(glm::vec3(0.0f, v.z, -v.y));
    }
}

glm::mat3 formOrthogonalBasis(glm::vec3 const &v) 
{
    glm::vec3 a = find_orthogonal(v);
    glm::vec3 b = glm::cross(a,v);
    return glm::mat3(v,a,b);
}


float boxPlaneIntersect(BoundingBox obb, glm::mat4 modelMatrix, glm::vec3 planeNormal, glm::mat4 planeModelMatrix) {
    float smallest = 0.0f;
    glm::vec3 collisionPosition;
    obb.getTranslatedVertices(modelMatrix);
    for (unsigned int i = 0; i < obb.translated_vertices.size(); i++) {

        vec3 v = obb.translated_vertices.at(i);
        float d = dot( vec4(planeNormal.x, planeNormal.y, planeNormal.z, 0.0f), vec4(v.x, v.y, v.z, 1.0f) );
        if (d < 0) {
            if (d < smallest) {
                smallest = d;
                collisionPosition = obb.translated_vertices.at(i);
            }
        }
    }
    if (smallest < 0.0f) {
        glm::vec3 collisionNormal = normalize(planeNormal);
        glm::vec3 penetrationVector = collisionNormal * smallest;

        vec3 a = collisionPosition - penetrationVector;
        vec3 b = vec4(-1.0f,  1.0f, 0.0f, 1.0f) * planeModelMatrix;
        vec3 c = vec4(-1.0f, -1.0f, 0.0f, 1.0f) * planeModelMatrix;
        //vec3 d = vec4( 1.0f, -1.0f, 0.0f, 1.0f) * planeModelMatrix;
        vec3 e = vec4( 1.0f,  1.0f, 0.0f, 1.0f) * planeModelMatrix;
        if (dot(b, c - b) <= dot(a, c - b) && dot(a, c - b) <= dot(c, c-b) &&
        dot(b, e - b) <= dot(a, e - b) && dot(a, e - b) <= dot(e, e-b)) {
          return smallest;
        }
    }
    return 0.0f;
}

BoundingBox::BoundingBox(){
  setup(vec3(-0.5f), vec3(0.5f));
};

std::vector<glm::vec3> BoundingBox::getTranslatedVertices(glm::mat4 modelMatrix) {
    position = getPosition(modelMatrix);
    this->translated_vertices = {};
    for (unsigned int i = 0; i < vertices.size(); i+=3) {
        glm::vec3 v = glm::vec3(vertices.at(i), vertices.at(i+1), vertices.at(i+2));
        glm::vec4 _v = glm::vec4(v.x, v.y, v.z, 1.0);
        glm::vec4 tv = modelMatrix * _v;
        this->translated_vertices.push_back(glm::vec3(tv.x, tv.y, tv.z));
    }
    return this->translated_vertices;
}

BoundingBox::BoundingBox( const BoundingBox &obj) {
  for (unsigned int i = 0; i < obj.vertices.size(); i++) {
    vertices.push_back(obj.vertices.at(i));
  }
}

void BoundingBox::setup(glm::vec3 min, glm::vec3 max) {

    glGenQueries(1, &queryFront);
    glGenQueries(1, &queryBack);

    modelMatrix = mat4(1.0f);

    vertices = {
        max[0], min[1], min[2], // 1
        min[0], min[1], min[2], // 0
        max[0], max[1], min[2], // 2
        min[0], min[1], min[2], // 0
        min[0], max[1], min[2], // 3
        max[0], max[1], min[2], // 2

        min[0], min[1], max[2], // 0
        max[0], min[1], max[2], // 1
        max[0], max[1], max[2], // 2
        max[0], max[1], max[2], // 2
        min[0], max[1], max[2], // 3
        min[0], min[1], max[2], // 0

        min[0], max[1], max[2],
        min[0], max[1], min[2],
        min[0], min[1], min[2],
        min[0], min[1], min[2],
        min[0], min[1], max[2],
        min[0], max[1], max[2],

        max[0], max[1], min[2], // 1
        max[0], max[1], max[2], // 0
        max[0], min[1], min[2], // 2
        max[0], max[1], max[2], // 0
        max[0], min[1], max[2], // 3
        max[0], min[1], min[2], // 2

        min[0], min[1], min[2], //0
        max[0], min[1], min[2], //1
        max[0], min[1], max[2], //2
        max[0], min[1], max[2], //2
        min[0], min[1], max[2], //3
        min[0], min[1], min[2], //0

        max[0], max[1], min[2], //1
        min[0], max[1], min[2], //0
        max[0], max[1], max[2], //2`
        min[0], max[1], min[2], //0
        min[0], max[1], max[2], //3
        max[0], max[1], max[2], //2

    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    const char *vertexShaderSource = "#version 300 es\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "out vec4 outColor;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "   outColor = vec4(aPos,1.0f);\n"
        "}\0";
    const char *fragmentShaderSource = "#version 300 es\n"
        "precision mediump float;\n"
        "out vec4 FragColor;\n"
        "in vec4 outColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = outColor;\n"
        "}\n\0";

    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
    }
    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
    }
    // link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    BoundingBox::allBoundingBoxes.push_back(this);

}

void BoundingBox::setBoundary(glm::vec3 _min, glm::vec3 _max) {
  min = _min;
  max = _max;
  vertices = {
        max[0], min[1], min[2], // 1
        min[0], min[1], min[2], // 0
        max[0], max[1], min[2], // 2
        min[0], min[1], min[2], // 0
        min[0], max[1], min[2], // 3
        max[0], max[1], min[2], // 2

        min[0], min[1], max[2], // 0
        max[0], min[1], max[2], // 1
        max[0], max[1], max[2], // 2
        max[0], max[1], max[2], // 2
        min[0], max[1], max[2], // 3
        min[0], min[1], max[2], // 0

        min[0], max[1], max[2],
        min[0], max[1], min[2],
        min[0], min[1], min[2],
        min[0], min[1], min[2],
        min[0], min[1], max[2],
        min[0], max[1], max[2],

        max[0], max[1], min[2], // 1
        max[0], max[1], max[2], // 0
        max[0], min[1], min[2], // 2
        max[0], max[1], max[2], // 0
        max[0], min[1], max[2], // 3
        max[0], min[1], min[2], // 2

        min[0], min[1], min[2], //0
        max[0], min[1], min[2], //1
        max[0], min[1], max[2], //2
        max[0], min[1], max[2], //2
        min[0], min[1], max[2], //3
        min[0], min[1], min[2], //0

        max[0], max[1], min[2], //1
        min[0], max[1], min[2], //0
        max[0], max[1], max[2], //2
        min[0], max[1], min[2], //0
        min[0], max[1], max[2], //3
        max[0], max[1], max[2], //2
    };

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());

}

void BoundingBox::setMatrix(std::string name, glm::mat4 mat) {
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void BoundingBox::draw() {
    glUseProgram(shaderProgram);

    // render box
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

glm::vec3 getPosition(glm::mat4 mat){
  glm::vec4 v4 = mat[3];
  return glm::vec3(v4.x, v4.y, v4.z);
}

float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}  

glm::vec3 reflect(glm::vec3 I, glm::vec3 N) {
    return I  + N  * -2.0f * glm::dot(N, I);
}


//@brief
// the prototype now reflects glm::unproject as closely as possible
vec3 rayCast(double xpos, double ypos, mat4 view, mat4 projection, int WIDTH, int HEIGHT) {
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


float rayIntersectQuad(vec3 n, vec3 p0, vec3 l0, vec3 l, mat4 planeModelMatrix)
{
  // scratch a pixel
    // assuming vectors are all normalized
    float denom = dot(l,n);
    vec3 p0l0 = p0 - l0;
    float t = dot(p0l0, n) / denom;


    if (t > 0.0f) {
        glm::vec3 penetrationVector = n  * t;
        glm::vec3 collisionPosition = l0 + l * t;
        glm::vec3 a = collisionPosition - penetrationVector;
        glm::vec3 b = glm::vec4(-1.0f, 0.0f, 1.0f, 1.0f) * planeModelMatrix;
        glm::vec3 c = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f) * planeModelMatrix;
        glm::vec3 e = glm::vec4(1.0f, 0.0f, -1.0f, 1.0f) * planeModelMatrix;
        if (glm::dot(b, c - b) <= glm::dot(a, c - b) && glm::dot(a, c - b) <= glm::dot(c, c-b) &&
        glm::dot(b, e - b) <= glm::dot(a, e - b) && glm::dot(a, e - b) <= glm::dot(e, e-b)) {
            return t;
        } {
          return 0.0f;
        }
    }

    return t;
}
float minDistancePointToPlane(glm::vec3 plane_position, glm::vec3 plane_normal, glm::vec3 point) {
    return dot(point - plane_position, glm::normalize(plane_normal));
}

float lineSegmentTriangleIntersect(
    const vec3 &startPos, const vec3 &endPos,
    const vec3 &vertex0, const vec3 &vertex1, const vec3 &vertex2)
{

  glm::vec3 rayDir = glm::normalize(endPos - startPos);
  glm::vec3 triNormal = glm::normalize(glm::cross(vertex1 - vertex0, vertex2 - vertex0));

  float rti = rayTriangleIntersect(startPos, rayDir, vertex0, vertex1, vertex2);
  if (rti > 0.0f) {
    glm::vec3 planePoint = startPos + rayDir * rti;
    float s = minDistancePointToPlane(planePoint, triNormal, startPos);
    float e = minDistancePointToPlane(planePoint, triNormal, endPos);
    if ((s < 0.0f && e > 0.0f) || (s > 0.0f && e < 0.0f)) {
      return rti;
    }
    else {
      return 0.0f;
    }
  } else {
    return 0.0f;
  }
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

float rayTriangleIntersectNegative(
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
  if (t < 1/EPSILON) // ray intersection
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
// float rayObjectIntersect(glm::vec3 rayOrigin, glm::vec3 rayDirection, StaticObject object)
// {   
//   std::vector<float> intersect_values = {};
//   for (unsigned int j = 0; j < object.model.get()->meshes.size(); j++) {
//     std::vector<Vertex> vertices = object.model.get()->meshes[j].vertices;
//     for (unsigned int i = 0; i < vertices.size(); i+=3) {
//       vec3 v1 = glm::vec3(object.getModelMatrix() * glm::vec4(vertices.at(i+0).Position, 1.0));
//       vec3 v2 = glm::vec3(object.getModelMatrix() * glm::vec4(vertices.at(i+1).Position, 1.0));
//       vec3 v3 = glm::vec3(object.getModelMatrix() * glm::vec4(vertices.at(i+2).Position, 1.0));
//       float t = rayTriangleIntersect(rayOrigin, rayDirection, v1, v2, v3);
//       if (t) {
//         intersect_values.push_back(t);
//       }
//     }
//   }
//   if (intersect_values.size()) {
//     float shortest_distance = FLT_MAX;
//     for (unsigned int i = 0; i < intersect_values.size(); i++) {
//         if (intersect_values.at(i) < shortest_distance) {
//           shortest_distance = intersect_values.at(i);
//         }
//     }
//     return shortest_distance;
//   } else {
//     return 0.0f;
//   }
// }

// TO DO doesn't work!
CollisionInfo rayBoxIntersect(glm::vec3 rayOrigin, glm::vec3 rayDirection, BoundingBox &boundingBox)
{   
  CollisionInfo c;
  std::vector<float> intersect_values = {};
  std::vector<glm::vec3> normals = {};
  std::vector<glm::vec3> positions = {};

  for (unsigned int i = 0; i < boundingBox.translated_vertices.size(); i+=3) {
      vec3 v1 = boundingBox.translated_vertices.at(i + 0);
      vec3 v2 = boundingBox.translated_vertices.at(i + 1);
      vec3 v3 = boundingBox.translated_vertices.at(i + 2);
      float t = rayTriangleIntersectNegative(rayOrigin, rayDirection, v1, v2, v3);
      if (t) {
        intersect_values.push_back(t);
        glm::vec3 intersectionPosition = rayOrigin + rayDirection * t;
        positions.push_back(intersectionPosition);
        glm::vec3 normal = glm::normalize(glm::cross(v2-v1, v3-v1));
        // recalculate outward normals
        // if (dot(intersectionPosition - boundingBox.position, normal) < 0.0f) {
        //   normal *= -1.0f;
        // }
        normals.push_back(normal);
      }
  }

  if (intersect_values.size()) {
    float shortest_distance = FLT_MAX;
    glm::vec3 collidingNormal(0.0f, 0.0f, 0.0f);
    for (unsigned int i = 0; i < intersect_values.size(); i++) {
        if (intersect_values.at(i) < shortest_distance) {
          shortest_distance = intersect_values.at(i);
          c.depth = intersect_values.at(i);
          c.normal = normals.at(i);
          c.position = positions.at(i);
        }
    }
    return c;
  } else {
    return c;
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

CollisionInfo::CollisionInfo() {
  position = vec3(0,0,0);
  normal = vec3(0,0,0);
  depth = 0.0f;
}


CollisionInfo check_collision(BoundingBox boundingBox1, glm::mat4 modelMatrix1, BoundingBox boundingBox2, glm::mat4 modelMatrix2) {

  vec3 displacement(0.0f, 0.0f, 0.0f);
  float smallest = 0.0f;

  vec3 collidingNormal;
  vec3 collidingPosition;

  BoundingBox *poly1 = &boundingBox1;
  BoundingBox *poly2 = &boundingBox2;

  poly1->getTranslatedVertices(modelMatrix1);
  poly2->getTranslatedVertices(modelMatrix2);
  CollisionInfo c;


  float axisProjx, axisProjy, axisProjz;

  for (unsigned int shape = 0; shape < 2; shape++) {
    if (shape == 1) {
      poly1 = &boundingBox2;
      poly2 = &boundingBox1;
    }
    for (unsigned int a = 0; a < poly1->translated_vertices.size(); a++) {
      float b = (a + 1) % poly1->translated_vertices.size();
      axisProjx = -(poly1->translated_vertices[b].y - poly1->translated_vertices[a].y);
      axisProjy = poly1->translated_vertices[b].x - poly1->translated_vertices[a].x;

      float minR1_xy = FLT_MAX;
      float maxR1_xy = -FLT_MAX;
      for (unsigned int p = 0; p < poly1->translated_vertices.size(); p++) {
        vec2 translated_vertex = vec2(poly1->translated_vertices[p].x, poly1->translated_vertices[p].y);
        float q = dot(translated_vertex, vec2(axisProjx, axisProjy));
        minR1_xy = std::min(minR1_xy, q);
        maxR1_xy = std::max(maxR1_xy, q);
      }

      float minR2_xy = FLT_MAX;
      float maxR2_xy = -FLT_MAX;
      for (unsigned int p = 0; p < poly2->translated_vertices.size(); p++) {
        vec2 translated_vertex = vec2(poly2->translated_vertices[p].x, poly2->translated_vertices[p].y);
        float q = dot(translated_vertex, vec2(axisProjx, axisProjy));
        minR2_xy = std::min(minR2_xy, q);
        maxR2_xy = std::max(maxR2_xy, q);
      }
 
      axisProjx = -(poly1->translated_vertices[b].z - poly1->translated_vertices[a].z);
      axisProjz = poly1->translated_vertices[b].x - poly1->translated_vertices[a].x;
      float minR1_xz = FLT_MAX;
      float maxR1_xz = -FLT_MAX;
      for (unsigned int p = 0; p < poly1->translated_vertices.size(); p++) {
        vec2 translated_vertex = vec2(poly1->translated_vertices[p].x, poly1->translated_vertices[p].z);
        float q = dot(translated_vertex, vec2(axisProjx, axisProjz));
        minR1_xz = std::min(minR1_xz, q);
        maxR1_xz = std::max(maxR1_xz, q);
      }

      float minR2_xz = FLT_MAX;
      float maxR2_xz = -FLT_MAX;
      for (unsigned int p = 0; p < poly2->translated_vertices.size(); p++) {
        vec2 translated_vertex = vec2(poly2->translated_vertices[p].x, poly2->translated_vertices[p].z);
        float q = dot(translated_vertex, vec2(axisProjx, axisProjz));
        minR2_xz = std::min(minR2_xz, q);
        maxR2_xz = std::max(maxR2_xz, q);
      }
 
      axisProjy = -(poly1->translated_vertices[b].z - poly1->translated_vertices[a].z);
      axisProjz = poly1->translated_vertices[b].y - poly1->translated_vertices[a].y;
 
      float minR1_yz = FLT_MAX;
      float maxR1_yz = -FLT_MAX;
      for (unsigned int p = 0; p < poly1->translated_vertices.size(); p++) {
        vec2 translated_vertex = vec2(poly1->translated_vertices[p].y, poly1->translated_vertices[p].z);
        float q = dot(translated_vertex, vec2(axisProjy, axisProjz));
        minR1_yz = std::min(minR1_yz, q);
        maxR1_yz = std::max(maxR1_yz, q);
      }
 
      float minR2_yz = FLT_MAX;
      float maxR2_yz = -FLT_MAX;
      for (unsigned int p = 0; p < poly2->translated_vertices.size(); p++) {
          vec2 translated_vertex = vec2(poly2->translated_vertices[p].y, poly2->translated_vertices[p].z);
          float q = dot(translated_vertex, vec2(axisProjy, axisProjz));
          minR2_yz = std::min(minR2_yz, q);
          maxR2_yz = std::max(maxR2_yz, q);
      }

      // seperation between shadows of objects found => no collision possible, exit early


      if (!(maxR2_xy >= minR1_xy && maxR1_xy >= minR2_xy 
        && maxR2_xz >= minR1_xz && maxR1_xz >= minR2_xz 
        && maxR2_yz >= minR1_yz && maxR1_yz >= minR2_yz)) {

          return c;
      } else {
        for (unsigned int i = 0; i < boundingBox1.translated_vertices.size(); i++) {
          vec3 rayOrigin = boundingBox1.translated_vertices.at(i);
          vec3 rayDirection = boundingBox1.velocity;
          CollisionInfo boxCollisionInfo = rayBoxIntersect(rayOrigin, rayDirection, boundingBox2);
          float t = boxCollisionInfo.depth;

          if ((t < smallest && t < 0.0f) || smallest == 0.0f) {
            smallest = t;
            displacement = boundingBox1.velocity;
            collidingNormal = boxCollisionInfo.normal;
            collidingPosition = boxCollisionInfo.position;

          }
        }
        for (unsigned int i = 0; i < boundingBox2.translated_vertices.size(); i++) {
          vec3 rayOrigin = boundingBox2.translated_vertices.at(i);
          vec3 rayDirection = -boundingBox1.velocity;
          CollisionInfo boxCollisionInfo = rayBoxIntersect(rayOrigin, rayDirection, boundingBox1);
          float t = boxCollisionInfo.depth;
          if ((t < smallest && t < 0.0f) || smallest == 0.0f) {
            smallest = t;
            displacement = boundingBox1.velocity * -1.0f;
            collidingNormal = boxCollisionInfo.normal;
            collidingPosition = boxCollisionInfo.position;

          }
        }

          vec3 rayOrigin = boundingBox1.position;
          vec3 rayDirection = boundingBox1.velocity;
          CollisionInfo boxCollisionInfo = rayBoxIntersect(rayOrigin, rayDirection, boundingBox2);
          CollisionInfo boxCollisionInfo2 = rayBoxIntersect(rayOrigin, rayDirection, boundingBox1);
          float t = boxCollisionInfo.depth + boxCollisionInfo2.depth;
          if ((t < smallest && t < 0.0f) || smallest == 0.0f) {

            smallest = t;
            displacement = boundingBox1.velocity;
            collidingNormal = boxCollisionInfo.normal;
            collidingPosition = boxCollisionInfo.position;

          }

          rayOrigin = boundingBox2.position;
          rayDirection = -boundingBox1.velocity;
          boxCollisionInfo = rayBoxIntersect(rayOrigin, rayDirection, boundingBox1);
          boxCollisionInfo2 = rayBoxIntersect(rayOrigin, rayDirection, boundingBox2);

          t = boxCollisionInfo.depth + boxCollisionInfo2.depth;
          if ((t < smallest && t < 0.0f) || smallest == 0.0f) {
            smallest = t;
            displacement = boundingBox1.velocity * -1.0f;
            collidingNormal = boxCollisionInfo2.normal;
            collidingPosition = boxCollisionInfo.position;

          }

      }
      
    }
  }
  c.normal = collidingNormal;
  //std::cout << "3. " << to_string(c.normal) << std::endl;

  c.depth = smallest;
  c.position = collidingPosition;

  return c;
}
