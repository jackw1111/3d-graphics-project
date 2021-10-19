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

struct FBOWrap : FBO, boost::python::wrapper<FBO>
{
  FBOWrap();
  FBOWrap(int width, int height, bool position_buffer, bool normal_buffer, bool color_buffer, bool depth_buffer);
  FBOWrap(const char *path, const string &directory);
  int setup(int width, int height, bool position_buffer, bool normal_buffer,  bool color_buffer, bool depth_buffer);

  void handleError(int error);

  void Draw(StaticShader shader);
  void bind(bool value);

  void createNormalBuffer();
  void createDepthBuffer();
  void createPositionBuffer();
  void createColorBuffer();

  unsigned int getColorBuffer();
  unsigned int getDepthBuffer();
  unsigned int getNormalBuffer();
  unsigned int getPositionBuffer();
};

void wrap_FBO();
