#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include <iostream>

#include <glm/glm.hpp>

#include "collision.h"
#include "animated_model.h"
#include "octree.h"

class CharacterEntity {
public:
  CharacterEntity();
  CharacterEntity(const CharacterEntity &entity);
  glm::vec3 getPosition();
  int setPosition(glm::vec3 pos);
  glm::vec3 getVelocity();
  int setVelocity(glm::vec3 vel);
  CharacterEntity(std::vector<StaticModel> models, std::vector<mat4> _modelTransforms, glm::vec3 radius);
  virtual int setup(std::vector<StaticModel> models, std::vector<mat4> _modelTransforms, glm::vec3 radius);
  virtual int update();
  virtual int checkCollision();
  void collideAndSlide(const glm::vec3& gravity);
  void collideWithWorld(glm::vec3& out, const glm::vec3& pos, const glm::vec3& velocity);
  void remove();

  int addModel(StaticModel statModel);

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
  std::vector<std::vector<vec3>> triangles = {};
  bool collide = true;
  glm::vec3 slidePlaneNormal;
};




#endif // ENTITY_H
