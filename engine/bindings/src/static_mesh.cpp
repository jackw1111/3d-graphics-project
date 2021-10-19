#include "../../bindings/include/static_mesh.h"

StaticMeshWrap::StaticMeshWrap() : StaticMesh() {};

StaticMeshWrap::StaticMeshWrap(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, Material mat) : StaticMesh(vertices, indices, textures, mat) {};

int StaticMeshWrap::Draw(StaticShader shader)
{
  return StaticMesh::Draw(shader);
}

std::vector<std::vector<glm::vec3>> StaticMeshWrap::getTriangles(glm::mat4 model) {
  return StaticMesh::getTriangles(model);
}



void wrap_StaticMesh() {
    python::class_<StaticMesh>("StaticMesh", python::init<>())
    .def(python::init<std::vector<Vertex>, std::vector<unsigned int>, std::vector<Texture>, Material>())
    .def("setupMesh", &StaticMesh::setupMesh)
    .def("getAABB", &StaticMesh::getAABB)
    .def("Draw", &StaticMesh::Draw)
    .def("getTriangles", &StaticMesh::getTriangles)
    .def("update_lighting", &StaticMesh::updateLighting)
    .def_readwrite("vertices", &StaticMesh::vertices)
    .def_readwrite("indices", &StaticMesh::indices)
    .def_readwrite("textures", &StaticMesh::textures)
    .def_readwrite("bounding_cube", &StaticMesh::boundingCube)

    ;


    python::class_<Texture, boost::noncopyable>("Texture", python::init<>())
            .def_readwrite("type", &Texture::type)
            .def_readwrite("path", &Texture::path)
            .def_readwrite("id", &Texture::id)
        ;

    python::class_<BCube, boost::noncopyable>("BoundingCube", python::init<>())
    .def("get_translated_vertices", &BCube::getTranslatedVertices)
    ;
}


VertexWrap::VertexWrap() : Vertex() {};
VertexWrap::VertexWrap(float x, float y, float z) {Position = glm::vec3(x, y, z);};

bool  VertexWrap::operator==(const VertexWrap v) { return this == &v; };

void wrap_Vertex() {

    python::class_<Vertex, boost::noncopyable>("Vertex", python::init<>())
    .def(python::init<float, float, float>())
    .def_readwrite("Position", &Vertex::Position)
    .def_readwrite("shading", &Vertex::shading)
    ;
}