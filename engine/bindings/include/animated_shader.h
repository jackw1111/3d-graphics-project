
#include "_engine.h"

#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/detail/operator_id.hpp>
#include <boost/make_shared.hpp>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

using namespace boost;
using namespace boost::python;

struct AnimatedShaderWrap : AnimatedShader, boost::python::wrapper<AnimatedShader>
{
  int setup(const char* vertexPath, const char* fragmentPath);
  int setMat4(std::string name, glm::mat4 mat);
  int _setMat4(const char* name,unsigned long addr);
  int use();
};

void wrap_AnimatedShader();