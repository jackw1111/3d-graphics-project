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

struct RectWrap : Rect, boost::python::wrapper<Rect>
{
  RectWrap(vec2 position, vec2 size, std::string filePath) : Rect(position, size, filePath) {};
  int setupWithImage(std::string imageLocation);
  int draw();
  int remove();
};

void wrap_Rect();
