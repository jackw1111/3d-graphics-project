#include "../../bindings/include/entity.h"


boost::shared_ptr<CharacterEntity> create_characterentity(std::vector<StaticModel> models, std::vector<mat4> modelTransforms, glm::vec3 radius)
{
  return boost::shared_ptr<CharacterEntity>(
    new CharacterEntity(models, modelTransforms, radius),
    boost::mem_fn(&CharacterEntity::remove));
}


CharacterEntityWrap::CharacterEntityWrap(std::vector<StaticModel> models, std::vector<mat4> modelTransforms, vec3 radius) : CharacterEntity(models, modelTransforms, radius) {}


int CharacterEntityWrap::update()
{
  if (override update = this->get_override("update")) {
    return update();
  }
  return CharacterEntity::update();
}


int CharacterEntityWrap::default_update()
{
  return CharacterEntity::update();
}

int CharacterEntityWrap::setup(std::vector<StaticModel> models, std::vector<mat4> modelTransforms, vec3 radius)
{

  if (boost::python::override setup = this->get_override("setup")) {
    return setup(models, modelTransforms, radius);
  }
  return CharacterEntity::setup(models, modelTransforms, radius);
}

int CharacterEntityWrap::default_setup(std::vector<StaticModel> models, std::vector<mat4> modelTransforms, vec3 radius)
{
  return CharacterEntity::setup(models, modelTransforms, radius);
}

int CharacterEntityWrap::checkCollision()
{
  if (boost::python::override checkCollision = this->get_override("checkCollision")) {
    return checkCollision();
  }
  return CharacterEntity::checkCollision();
}

int CharacterEntityWrap::default_checkCollision()
{
  return CharacterEntity::checkCollision();
}

void wrap_CharacterEntity() {
    python::class_<CharacterEntity, boost::shared_ptr<CharacterEntity> >("CharacterEntity", python::no_init)
    .def("__init__", python::make_constructor(&create_characterentity))
    .def("update", &CharacterEntity::update)
    .def("setup", &CharacterEntity::setup)
    .def("checkCollision",&CharacterEntity::checkCollision)
    .def("add_model",&CharacterEntity::addModel)
    .def_readwrite("position", &CharacterEntity::position)
    .def_readwrite("velocity", &CharacterEntity::velocity)
    .def_readwrite("modelTransforms", &CharacterEntity::modelTransforms)
    .def_readwrite("collisionPackage", &CharacterEntity::collisionPackage)
    .def_readwrite("grounded", &CharacterEntity::grounded)
    .def_readwrite("Yaw", &CharacterEntity::Yaw)
    .def_readwrite("Front", &CharacterEntity::Front)
    .def_readwrite("Right", &CharacterEntity::Right)
    .def_readwrite("colliding_normal", &CharacterEntity::slidePlaneNormal)
    ;
}