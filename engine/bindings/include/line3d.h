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

struct Line3DWrap : Line3D, boost::python::wrapper<Line3D>
{
    Line3DWrap();
    Line3DWrap(vec3 start, vec3 end);

    int setModel(mat4 m);
    int setCamera(mat4 c);

    int setColor(vec3 col);

    int draw();

    int setEndpoints(vec3 startPoint, vec3 endPoint);


    // ~Line3DWrap();
};

void wrap_Line3D();