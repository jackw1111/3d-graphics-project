#include "../../bindings/include/label.h"

    
void wrap_Label() {
    python::class_<LabelWrap, boost::noncopyable>("Label", python::init<std::string, glm::vec2, std::string, GLfloat>())
    .def("draw", &Label::draw)
    .def_readwrite("shader", &Label::shader)
    .def_readwrite("position", &Label::position)
    .def_readwrite("text", &Label::text)
    .def_readwrite("color", &Label::color)
    .def_readwrite("to_draw", &Label::toDraw)
    ;
}
