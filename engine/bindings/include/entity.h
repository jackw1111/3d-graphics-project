#include "engine.h"

#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/detail/operator_id.hpp>
#include <boost/make_shared.hpp>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

using namespace boost;
using namespace boost::python;


struct CharacterEntityWrap : CharacterEntity, boost::python::wrapper<CharacterEntity>
{
  CharacterEntityWrap(std::vector<StaticModel> models, std::vector<mat4> modelTransforms, vec3 radius);
  int setup(std::vector<StaticModel> models, std::vector<mat4> modelTransforms, vec3 radius);
  int checkCollision();
  int update();
  int default_setup(std::vector<StaticModel> models, std::vector<mat4> modelTransforms, vec3 radius);
  int default_checkCollision();
  int default_update();
};

void wrap_CharacterEntity();

