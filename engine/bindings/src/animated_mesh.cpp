#include "../../bindings/include/animated_mesh.h"

AnimatedMeshWrap::AnimatedMeshWrap() : AnimatedMesh() {};

AnimatedMeshWrap::AnimatedMeshWrap(std::vector<VertexTransform> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) : AnimatedMesh(vertices, indices, textures) {};

int AnimatedMeshWrap::Draw(AnimatedShader shader)
{
  return AnimatedMeshWrap::Draw(shader);
}

int AnimatedMeshWrap::DrawInstanced(AnimatedShader shader, std::vector<glm::mat4> modelMatrices)
{
  return AnimatedMeshWrap::DrawInstanced(shader, modelMatrices);
}

std::vector<std::vector<glm::vec3>> AnimatedMeshWrap::getTriangles(glm::mat4 model) {
  return AnimatedMesh::getTriangles(model);
}

void wrap_AnimatedMesh() {
    python::class_<AnimatedMeshWrap, boost::noncopyable>("AnimatedMesh", python::init<>())
    .def(python::init<std::vector<VertexTransform>, std::vector<unsigned int>, std::vector<Texture>>())
    .def("setupMesh", &AnimatedMeshWrap::setupMesh)
    .def("Draw", &AnimatedMeshWrap::Draw)
    .def("getAABB", &AnimatedMeshWrap::getAABB)
    .def("DrawInstanced", &AnimatedMeshWrap::DrawInstanced)
    .def("getTriangles", &AnimatedMesh::getTriangles)
    .def_readwrite("vertices", &AnimatedMeshWrap::vertices)
    .def_readwrite("indices", &AnimatedMeshWrap::indices)
    .def_readwrite("textures", &AnimatedMeshWrap::textures)
    ;
}

VertexTransformWrap::VertexTransformWrap() : VertexTransform() {};
VertexTransformWrap::VertexTransformWrap(float x, float y, float z) {Position = glm::vec3(x, y, z);};

bool  VertexTransformWrap::operator==(const VertexTransformWrap v) { return this == &v; };

void wrap_VertexTransform() {

    python::class_<VertexTransform, boost::noncopyable>("VertexTransform", python::init<>())
    .def(python::init<float, float, float>())
    .def_readwrite("Position", &VertexTransform::Position)
    ;

}
