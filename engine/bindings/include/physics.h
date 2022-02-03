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


struct CollisionBoxWrap : CollisionBox, boost::python::wrapper<CollisionBox>
{
    CollisionBoxWrap(double x, double y, double z, double gravity, double rx, double ry, double rz, double sx, double sy, double sz);
    void setState(double x, double y, double z, double gravity, double rx, double ry, double rz, double sx, double sy, double sz);
};

void wrap_CollisionBox();

