#include "../../../bindings/graphics/include/particle.h"

void wrap_Particle() {

    python::class_<ParticleWrap, boost::noncopyable>("Particle", python::init<std::string, glm::mat4, unsigned, unsigned>())
    .add_property("frame", &Particle::getFrame, &Particle::setFrame)
    .def_readwrite("model_matrix", &Particle::modelMatrix)
    .def_readwrite("alpha", &Particle::alpha)
    .def_readwrite("blend_mode", &Particle::blendMode)
    .def_readwrite("order", &Particle::order)
	;
}