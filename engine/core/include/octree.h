#ifndef OCTREE_H
#define OCTREE_H

#include <vector>
#include <iostream>

#include <glm/glm.hpp>
#include <vector>
#include <limits>
#include <chrono>
using glm::vec3;

#include "collision.h"
#include "animated_model.h"

bool isTriangleInCube(vec3 position, float hw, std::vector<vec3> triangles);

class Octree {
public:
  int index;
  vec3 position;
  float hw;
  std::vector<std::vector<vec3>> triangles;
  std::vector<vec3> points;
  std::vector<Octree> children;
  Octree(){};
  Octree(int ind, vec3 pos, float hwidth, std::vector<std::vector<vec3>> tris);
  void setup(int ind, vec3 pos, float hwidth, std::vector<std::vector<vec3>> tris);
  void createChildren();
  // get the triangles to collide with for a particular point, pos.
  std::vector<std::vector<vec3>> getTriangleSet(vec3 position);
};

#endif
