
#include "../../../bindings/graphics/include/line2d.h"

Line2DWrap::Line2DWrap(vec2 start, vec2 end, float w, float h) : Line2D(start, end, w, h) {
    
}

void Line2DWrap::setEndpoints(vec2 start, vec2 end) {
	Line2D::setEndpoints(start, end);
}

void wrap_Line2D() {

    python::class_<Line2DWrap, boost::noncopyable>("Line2D", python::init<vec2, vec2, float, float>())
    .def("set_endpoints", &Line2DWrap::setEndpoints)
    .add_property("color", &Line2D::lineColor, &Line2D::setColor)
    .def_readwrite("start_point", &Line2DWrap::startPoint)
    .def_readwrite("end_point", &Line2DWrap::endPoint)
    ;
}