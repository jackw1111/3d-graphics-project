
#include "../../../bindings/graphics/include/line3d.h"

Line3DWrap::Line3DWrap() : Line3D() {
    
}

Line3DWrap::Line3DWrap(vec3 start, vec3 end) : Line3D(start, end) {
    
}


int Line3DWrap::setColor(vec3 col) {
    return Line3D::setColor(col);
}

int Line3DWrap::setEndpoints(vec3 start, vec3 end) {
    return Line3D::setEndpoints(start, end);
}


void wrap_Line3D() {

    python::class_<Line3DWrap, boost::noncopyable>("Line3D", python::init<>())
    .def(python::init<vec3, vec3>())
    .def("set_endpoints", &Line3DWrap::setEndpoints)
    .def_readwrite("start_point", &Line3DWrap::startPoint)
    .def_readwrite("end_point", &Line3DWrap::endPoint)
    .add_property("project", &Line3D::getProject, &Line3D::setProject)
    .add_property("color", &Line3DWrap::color, &Line3DWrap::setColor)
    ;
}