
#include "../../../bindings/graphics/include/animated_shader.h"


int AnimatedShaderWrap::setup(const char* vertexPath, const char* fragmentPath)
{
  return AnimatedShader::setup(vertexPath, fragmentPath);
}
int AnimatedShaderWrap::setMat4(std::string name, glm::mat4 mat)
{
  return AnimatedShader::setMat4(name, mat);
}
int AnimatedShaderWrap::_setMat4(const char* name,unsigned long addr)
{
  return AnimatedShader::_setMat4(name, addr);
}
int AnimatedShaderWrap::use()
{
  return AnimatedShader::use();
}

void wrap_AnimatedShader() {
  python::class_<AnimatedShaderWrap, boost::noncopyable>("AnimatedShader")
    .def("setup", &AnimatedShader::setup)
    .def("setMat4", &AnimatedShader::setMat4)
    .def("_setMat4", &AnimatedShader::_setMat4)
    .def("use", &AnimatedShader::use)
    ;
}
