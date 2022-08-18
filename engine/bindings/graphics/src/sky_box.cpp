
#include "../../../bindings/graphics/include/sky_box.h"

unsigned int SkyboxWrap::loadCubemap(std::vector<std::string> faces)
{
  if (override loadCubemap = this->get_override("loadCubemap")) {
    this->get_override("loadCubemap")(faces);
  } else {
    Skybox::loadCubemap(faces);
  }
  return 0;
}


void SkyboxWrap::Draw(StaticShader skyboxShader)
{

  if (boost::python::override Draw = this->get_override("Draw")) {
    this->get_override("Draw")(skyboxShader);
  } else {
    Skybox::Draw(skyboxShader);
  }

}

void SkyboxWrap::load(std::vector<std::string> faces)
{

  if (boost::python::override load = this->get_override("load")) {
    this->get_override("load")(faces);
  } else {
    Skybox::load(faces);
  }

}

void SkyboxWrap::bindTexture()
{

  if (boost::python::override load = this->get_override("bindTexture")) {
    this->get_override("bindTexture")();
  } else {
    Skybox::bindTexture();
  }

}

void wrap_Skybox() {
    python::class_<SkyboxWrap, boost::noncopyable>("Skybox")
      .def("load", &SkyboxWrap::load)
      .def("Draw", &SkyboxWrap::Draw)
      .def("bindTexture", &SkyboxWrap::bindTexture)
      .def_readwrite("load_skybox", &Skybox::loadSkybox)
      ;
}