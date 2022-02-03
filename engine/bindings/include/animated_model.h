
#include "engine.h"

#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/detail/operator_id.hpp>
#include <boost/make_shared.hpp>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

using namespace boost;
using namespace boost::python;


struct AnimatedModelWrap : AnimatedModel, boost::python::wrapper<AnimatedModel>
{
  AnimatedModelWrap(string path) : AnimatedModel(path) {};
  int Draw();
  int DrawInstanced(std::vector<mat4> modelTransforms);
  int getFrame(unsigned int uniqueID, float currentFrame);
  void setFrames(float start, float end);

};
void wrap_AnimatedModel();
