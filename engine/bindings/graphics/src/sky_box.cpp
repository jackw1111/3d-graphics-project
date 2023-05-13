
#include "../../../bindings/graphics/include/sky_box.h"

void wrap_Skybox() {
    python::class_<Skybox>("Skybox")
      .def("load", &Skybox::load)
      .def("Draw", &Skybox::Draw)
      .def("bindTexture", &Skybox::bindTexture)
      .def_readwrite("load_skybox", &Skybox::loadSkybox)
      ;
}