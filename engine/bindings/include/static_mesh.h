
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

/// @private
struct StaticMeshWrap : StaticMesh, boost::python::wrapper<StaticMesh>
{
  StaticMeshWrap();

  StaticMeshWrap(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, Material mat);

  int Draw(StaticShader shader);
  std::vector<std::vector<glm::vec3>> getTriangles(glm::mat4 model);
};

void wrap_StaticMesh();


/// @private
struct VertexWrap : Vertex, boost::python::wrapper<Vertex> {
    VertexWrap();
    VertexWrap(float x, float y, float z);
    bool  operator==(const VertexWrap v);
    VertexWrap operator+(const VertexWrap &v);

};


void wrap_Vertex();
