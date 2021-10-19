
#include "../../bindings/include/rect.h"

int RectWrap::setupWithImage(std::string imageLocation)
{
  return Rect::setupWithImage(imageLocation);
}
int RectWrap::draw() {
  return Rect::draw();
}

int RectWrap::remove() {
  return Rect::remove();
}

void wrap_Rect() {
    python::class_<RectWrap, boost::noncopyable>("Rect", python::init<vec2, vec2, std::string>())
    .def("setup_with_image", &Rect::setupWithImage)
    .def("setup_with_color", &Rect::setupWithColour)
    .def("draw", &Rect::draw)
    .def("remove", &Rect::remove)
    .def_readwrite("shader", &Rect::rectShader)
    .def_readwrite("position", &Rect::position)
    .def_readwrite("size", &Rect::size)
    .def_readwrite("color", &Rect::colour)
    .def_readwrite("to_draw", &Rect::toDraw)
    .def_readwrite("ordering", &Rect::ordering)
    .def_readwrite("shaded", &Rect::shaded)
    ;
}