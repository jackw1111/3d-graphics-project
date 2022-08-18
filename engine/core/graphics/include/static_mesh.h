#ifndef STATIC_MESH_H
#define STATIC_MESH_H

#include <vector>
#include <random>
#include <glm/glm.hpp>

#include "static_shader.h"
#include "math_utils.h"

using namespace std;
using glm::mat4;

enum OcclusionState {HIDDEN, VISIBLE, WAITING};

/*! @brief Internal class to store data about how light affects the surface of the StaticMesh */
class Material {
public:
     //Material color lighting
    glm::vec4 Ka;
     //Diffuse reflection
    glm::vec4 Kd;
     //Mirror reflection
    glm::vec4 Ks;

    float shininess;

    // Material& operator=(const Material &mat);
    // Material(const Material &mat);
};

/*! @brief data structure to hold vertex data */
struct Vertex {
    /// @brief default constructor
    Vertex();
    /// @brief piecewise constructor
    Vertex(float x, float y, float z);
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;

    float shading;

    /// @brief compare Vertex instances by pointer
    bool operator==(const Vertex v);
    Vertex operator+(const Vertex &v);
};

/*! @brief data structure to hold texture data */
struct Texture {
    bool operator==(const Texture t) { return this == &t; }
    unsigned int id;
    string type;
    string path;
};



/// @brief basic Mesh object with no support for skinned animations so suitable for non moving objects like trees, buildings, etc...
class StaticMesh {
public:
    /*  Mesh Data  */
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    Material mats;
    float specularity = 0.0;

    // a ID that identifies which mesh it is in the total model meshes
    unsigned int modelID = 0;
    std::string filePath;

    unsigned int VAO;
    ///@brief Render data
    unsigned int VBO, EBO;

    BoundingBox boundingBox;
    vector<float> limits;

    /// @brief default constructor for heap allocatation (paired with setupMesh(...))
    StaticMesh();
    ~StaticMesh();
    /// @brief constructor with list of vertices, indices and textures
    StaticMesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, const Material &mat);
    /// @brief copy constructor
    StaticMesh(const StaticMesh& other);
    /// @brief copy assignment
    StaticMesh& operator=(StaticMesh other);
    /// @brief simple comparison that compares pointers for equality
    bool operator==(const StaticMesh s) {return this == &s; };
    /// @brief returns triangle vertices in local space
    /// @todo make sure to cleanup after this exits
    std::vector<std::vector<glm::vec3>> getTriangles(const glm::mat4 &model);
    /// @brief render the mesh
    int Draw(StaticShader shader);
    /// @todo reimplement instancing to take into consideration the
    /// culling pipeline, and send matrices that pass to the GPU. Also glBufferData is allocating memory every draw call
    void DrawInstanced(StaticShader shader, std::vector<mat4> modelTransforms);
    virtual std::vector<float> getAABB();
    unsigned int instanceBuffer;

    bool frustumCulled = false;
    OcclusionState occlusionState = VISIBLE;
    bool toDraw = false;
//protected:


    /*  Functions    */
    /// @brief initializes all the buffer objects/arrays
    void setupMesh();

    void updateLighting(glm::vec3 lightPos, glm::vec3 lightDir, float value);

};

#endif

