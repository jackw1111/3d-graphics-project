
#include "../../../bindings/graphics/include/light.h"

void wrap_Light() {
    python::class_<LightWrap, boost::noncopyable>("Light", python::init<glm::vec3, glm::vec3>())
    .def_readwrite("position", &Light::position)
    .def_readwrite("color", &Light::color)
    ;
}
