#include "../../bindings/include/physics.h"

CollisionBoxWrap::CollisionBoxWrap(double x, double y, double z, double gravity, double rx, double ry, double rz, double sx, double sy, double sz)
 :     CollisionBox(x, y, z, gravity, rx, ry, rz, sx, sy, sz) { }

void wrap_CollisionBox() {
    python::class_<CollisionBoxWrap, boost::noncopyable>("CollisionBox", python::init<double, double, double, double, double, double, double, double, double, double>())
    	.def("set_state", &CollisionBox::setState)
    	.add_property("model_matrix", &CollisionBox::getModelMatrix, &CollisionBox::setModelMatrix)
   		;
    
}