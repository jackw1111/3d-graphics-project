#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include <iostream>

#include <glm/glm.hpp>

#include "collision.h"
#include "animated_model.h"
#include "octree.h"
#include "line3d.h"
/*! @brief 
  Sphere collider for collision detection and response with
  objects in the world, used mostly for player controllers
*/
class CharacterEntity {
public:
  CharacterEntity();
  CharacterEntity(const CharacterEntity &entity);
  glm::vec3 getPosition();
  int setPosition(glm::vec3 pos);
  glm::vec3 getVelocity();
  int setVelocity(glm::vec3 vel);
  CharacterEntity(std::vector<StaticModel> models, std::vector<mat4> _modelTransforms, glm::vec3 radius);
  int setup(std::vector<StaticModel> models, std::vector<mat4> _modelTransforms, glm::vec3 radius);
  int update();
  int checkCollision();
  void collideAndSlide(const glm::vec3& gravity);
  void collideWithWorld(glm::vec3& out, const glm::vec3& pos, const glm::vec3& velocity);
  void remove();
  glm::vec3 sphereSweep(glm::vec3 pos, glm::vec3 vel);
  int addModel(std::vector<StaticModel> models, std::vector<mat4> _modelTransforms);
  int addAnimatedObject(AnimatedObject aObject);
  glm::vec3 position, velocity, radius;
  CollisionPacket collisionPackage;
  std::vector<StaticModel> models;
  int grounded;
  float Yaw = 0.0f;
  vec3 Front;
  vec3 Right;
  bool initialized = false;
  int debug = 0;
  std::vector<mat4> modelTransforms;
  
  Octree *closestRegion;
  Octree cube;

  Octree *aClosestRegion;
  Octree animatedOctree;

  std::vector<std::vector<vec3>> triangles = {};
  std::vector<std::vector<vec3>> animated_triangles = {};

  bool collide = true;
  glm::vec3 slidePlaneNormal;
  glm::vec3 gravity;
  int sliding;

  Line3D line;
};




#endif // ENTITY_H
