#include "../../../bindings/graphics/include/label3d.h"

    
void wrap_Label3D() {
    python::class_<Label3DWrap, boost::noncopyable>("Label3D", python::init<std::string, glm::vec2, std::string, GLfloat>())
    .def("draw", &Label3D::draw)
    .def_readwrite("shader", &Label3D::shader)
    .def_readwrite("position", &Label3D::position)
    .def_readwrite("text", &Label3D::text)
    .def_readwrite("color", &Label3D::color)
    .def_readwrite("to_draw", &Label3D::toDraw)
    ;
}
