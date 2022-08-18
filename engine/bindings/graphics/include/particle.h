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

struct ParticleWrap : Particle, boost::python::wrapper<Particle>
{
  ParticleWrap(std::string file, glm::mat4 modelMatrix, unsigned int cols, unsigned int rows) : Particle(file, modelMatrix, cols, rows) {};
};

void wrap_Particle();
