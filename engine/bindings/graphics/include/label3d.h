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

struct Label3DWrap : Label3D, boost::python::wrapper<Label3D>
{
  Label3DWrap(std::string text, glm::vec2 position, std::string font, GLfloat scale) : Label3D(text, position, font, scale){};
  int draw();
};

void wrap_Label3D();
