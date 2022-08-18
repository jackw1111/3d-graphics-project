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

struct Rect2DWrap : Rect2D, boost::python::wrapper<Rect2D>
{
  Rect2DWrap(glm::vec2 pos, glm::vec2 sz, std::string file, unsigned int cols, unsigned int rows) : Rect2D(pos, sz, file, cols, rows) {};
};

void wrap_Rect2D();
