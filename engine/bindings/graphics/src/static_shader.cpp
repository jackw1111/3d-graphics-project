#include "../../../bindings/graphics/include/static_shader.h"

int StaticShaderWrap::setup(const char* vertexPath, const char* fragmentPath)
{
  return StaticShader::setup(vertexPath, fragmentPath);
}
int StaticShaderWrap::setMat4(std::string name, glm::mat4 mat)
{
  return StaticShader::setMat4(name, mat);
}

// int StaticShaderWrap::setFloat(std::string name, float value)
// {
//   return StaticShader::setFloat(name, value);
// }

int StaticShaderWrap::setVec3(std::string name, glm::vec3 vec)
{
  return StaticShader::setVec3(name, vec);
}
int StaticShaderWrap::_setMat4(const char* name,unsigned long addr)
{
  return StaticShader::_setMat4(name, addr);
}

int StaticShaderWrap::use()
{
  return StaticShader::use();
}
//BOOST_PYTHON_OPAQUE_SPECIALIZED_TYPE_ID(GLFWwindow);

void wrap_StaticShader() {

  python::class_<StaticShaderWrap, boost::noncopyable>("StaticShader")
    .def("setup", &StaticShader::setup)
    .def("setMat4", &StaticShader::setMat4)
    .def("setFloat", &StaticShader::setFloat)
    .def("setFloatArray", &StaticShader::setFloatArray)
    .def("setVec2", &StaticShader::setVec2)
    .def("setVec3", &StaticShader::setVec3)
    .def("_setMat4", &StaticShader::_setMat4)
    .def("setInt", &StaticShader::setInt)
    .def("use", &StaticShader::use)
    ;

}
