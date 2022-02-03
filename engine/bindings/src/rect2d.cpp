#include "../../bindings/include/rect2d.h"

void wrap_Rect2D() {

    python::class_<Rect2DWrap, boost::noncopyable>("Rect2D", python::init<glm::vec2, glm::vec2, std::string, unsigned, unsigned>())
    .add_property("frame", &Rect2D::getFrame, &Rect2D::setFrame)
    .def_readwrite("alpha", &Rect2D::alpha)
    .def_readwrite("blend_mode", &Rect2D::blendMode)
    .add_property("ordering", &Rect2D::getOrdering, &Rect2D::setOrdering)
	.add_property("size", &Rect2D::getSize, &Rect2D::setSize)
    .add_property("position", &Rect2D::getPosition, &Rect2D::setPosition)
    .add_property("to_draw", &Rect2D::getToDraw, &Rect2D::setToDraw)
	;
}