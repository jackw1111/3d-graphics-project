#ifndef ANIMATED_MESH_H
#define ANIMATED_MESH_H

#include <iostream>
#include <vector>
#include <map>

#include <assimp/Importer.hpp>

#include "static_mesh.h"
#include "animated_shader.h"
#include "math_utils.h"


#define NUM_BONES_PER_VEREX 4

/// @brief testing
///        testing
struct BoneData
{
    unsigned int IDs[NUM_BONES_PER_VEREX];
    float Weights[NUM_BONES_PER_VEREX];

    BoneData()
    {
        Reset();
    };

    void Reset()
    {
        memset(IDs, 0, sizeof(IDs));
        memset(Weights, 0, sizeof(Weights));
    }

    void AddBoneData(unsigned int BoneID, float Weight);
};

struct VertexTransform {

    VertexTransform();
    VertexTransform(float x, float y, float z);

    bool operator==(const VertexTransform v);
    VertexTransform operator+(const VertexTransform &v);
    // position
    vec3 Position;
    // normal
    vec3 Normal;
    // texCoords
    vec2 TexCoords;
    // tangent
    vec3 Tangent;
    // bitangent
    vec3 Bitangent;
    // bones effect
    BoneData Bones;
};

class AnimatedMesh : public StaticMesh
{
public:
    AnimatedMesh(){};
    AnimatedMesh(vector<VertexTransform> vertices, vector<unsigned int> indices, vector<Texture> textures);
    AnimatedMesh(const AnimatedMesh& m);
    ~AnimatedMesh();

    void Draw(AnimatedShader shader);
    void DrawInstanced(AnimatedShader shader, std::vector<mat4> modelMatrices);
    void setupMesh();
    void Clear();
    std::vector<float> getAABB();

    const aiScene* m_pScene = NULL;
    mat4 globalInverseTransform;

    vector<Texture> textures;
    vector<VertexTransform> vertices;
    vector<BoneData> Bones;
    vector<unsigned int> indices;

    Assimp::Importer *m_Importer;

    unsigned int modelID = 0;
};




#endif  /* ANIMATED_MESH_H */
