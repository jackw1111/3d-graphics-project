#ifndef ANIMATED_MESH_H
#define ANIMATED_MESH_H

#include <assimp/Importer.hpp>

#include "math_utils.h"
#include "static_mesh.h"
#include "animated_shader.h"

#define NUM_BONES_PER_VEREX 4

/// @brief Internal data structure to hold bone data for AnimatedModel's
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

/// @brief Internal data structure to hold vertex data for AnimatedModel's
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

/*! @brief Type to store skinned mesh data.
 - AnimatedModel's can (and usually do) contain more than one AnimatedMesh
 - It is one of the atomic units that gets passed to the renderer to be efficiently drawn.
*/
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

    glm::vec3 min;
    glm::vec3 max;

    vector<vec3> animatedVertices = {};
    int setVertices(vector<mat4> _gBones);
};

#endif  /* ANIMATED_MESH_H */
