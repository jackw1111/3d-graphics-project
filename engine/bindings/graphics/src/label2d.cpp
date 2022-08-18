#include "../../../bindings/graphics/include/label2d.h"

    
void wrap_Label2D() {
    python::class_<Label2DWrap, boost::noncopyable>("Label2D", python::init<std::string, glm::vec2, std::string, GLfloat>())
    .def("draw", &Label2D::draw)
    .def_readwrite("shader", &Label2D::shader)
    .def_readwrite("position", &Label2D::position)
    .def_readwrite("text", &Label2D::text)
    .def_readwrite("color", &Label2D::color)
    .def_readwrite("to_draw", &Label2D::toDraw)
    ;
}
