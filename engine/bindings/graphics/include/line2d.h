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

struct Line2DWrap : Line2D, boost::python::wrapper<Line2D>
{
    Line2DWrap(vec2 start, vec2 end, float w, float h);

    void setEndpoints(vec2 start, vec2 end);

};

void wrap_Line2D();