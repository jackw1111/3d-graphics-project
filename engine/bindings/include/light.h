#include "_engine.h"

#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/detail/operator_id.hpp>
#include <boost/make_shared.hpp>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

using namespace boost;
using namespace boost::python;

struct LightWrap : Light, boost::python::wrapper<Light>
{
  LightWrap(glm::vec3 position, glm::vec3 color) : Light(position, color) {};
};

void wrap_Light();