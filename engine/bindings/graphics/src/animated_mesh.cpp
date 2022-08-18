#include "../../../bindings/graphics/include/animated_mesh.h"

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
    python::class_<AnimatedMesh, boost::noncopyable>("AnimatedMesh", python::init<>())
    .def(python::init<std::vector<VertexTransform>, std::vector<unsigned int>, std::vector<Texture>>())
    .def("setupMesh", &AnimatedMesh::setupMesh)
    .def("Draw", &AnimatedMesh::Draw)
    .def("getAABB", &AnimatedMesh::getAABB)
    .def("DrawInstanced", &AnimatedMesh::DrawInstanced)
    .def("getTriangles", &AnimatedMesh::getTriangles)
    .def_readwrite("vertices", &AnimatedMesh::vertices)
    .def_readwrite("indices", &AnimatedMesh::indices)
    .def_readwrite("textures", &AnimatedMesh::textures)
    .def_readwrite("bounding_box", &AnimatedMesh::boundingBox)
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
