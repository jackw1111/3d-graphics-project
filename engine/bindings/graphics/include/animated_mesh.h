
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

struct AnimatedMeshWrap : AnimatedMesh, boost::python::wrapper<AnimatedMesh>
{
  AnimatedMeshWrap();

  AnimatedMeshWrap(std::vector<VertexTransform> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

  int Draw(AnimatedShader shader);
  int DrawInstanced(AnimatedShader shader, std::vector<mat4> modelMatrices);
  std::vector<std::vector<glm::vec3>> getTriangles(glm::mat4 model);
};

void wrap_AnimatedMesh();

struct VertexTransformWrap : VertexTransform, boost::python::wrapper<VertexTransform> {
    VertexTransformWrap();
    VertexTransformWrap(float x, float y, float z);
    bool  operator==(const VertexTransformWrap v);
    VertexTransformWrap operator+(const VertexTransformWrap &v);

};

void wrap_VertexTransform();
