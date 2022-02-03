#include "animated_mesh.h"

VertexTransform::VertexTransform(){};
VertexTransform::VertexTransform(float x, float y, float z) {Position = glm::vec3(x, y, z);}
bool  VertexTransform::operator==(const VertexTransform v) { return this == &v; }

AnimatedMesh::AnimatedMesh(vector<VertexTransform> vertices, vector<unsigned int> indices, vector<Texture> textures){
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}

AnimatedMesh::AnimatedMesh(const AnimatedMesh& m) : StaticMesh(m) {
    m_pScene = m.m_pScene;
    globalInverseTransform = m.globalInverseTransform;
    textures = m.textures;
    vertices = m.vertices;
    Bones = m.Bones;
    indices = m.indices;
    m_Importer = m.m_Importer;

    setupMesh();
}


std::vector<float> AnimatedMesh::getAABB() {

    for(unsigned int i = 0; i < animatedVertices.size(); i++)
    {
        if (animatedVertices[i].x < min.x) {
            min.x = animatedVertices[i].x;
        } else if (animatedVertices[i].x > max.x) {
            max.x = animatedVertices[i].x;
        }
        if (animatedVertices[i].y < min.y) {
            min.y = animatedVertices[i].y;
        } else if (animatedVertices[i].y > max.y) {
            max.y = animatedVertices[i].y;
        }
        if (animatedVertices[i].z < min.z) {
            min.z = animatedVertices[i].z;
        } else if (animatedVertices[i].z > max.z) {
            max.z = animatedVertices[i].z;
        }
    }
    vector<float> limits = {min.x, min.y, min.z, max.x, max.y, max.z};
    return limits;
}

int AnimatedMesh::setVertices(vector<mat4> _gBones) {
    animatedVertices = {};
    std::vector<VertexTransform> verts = vertices;
    std::vector<unsigned int> inds = indices;

    for (unsigned int i = 0; i < inds.size(); i++) {
        int BoneID0 = verts.at(inds.at(i)).Bones.IDs[0];
        int BoneID1 = verts.at(inds.at(i)).Bones.IDs[1];
        int BoneID2 = verts.at(inds.at(i)).Bones.IDs[2];
        int BoneID3 = verts.at(inds.at(i)).Bones.IDs[3];

        float Weights0 = verts.at(inds.at(i)).Bones.Weights[0];
        float Weights1 = verts.at(inds.at(i)).Bones.Weights[1];
        float Weights2 = verts.at(inds.at(i)).Bones.Weights[2];
        float Weights3 = verts.at(inds.at(i)).Bones.Weights[3];

        mat4 BoneTransform = _gBones.at(BoneID0) * Weights0;
        BoneTransform     += _gBones.at(BoneID1) * Weights1;
        BoneTransform     += _gBones.at(BoneID2) * Weights2;
        BoneTransform     += _gBones.at(BoneID3) * Weights3;

        vec4 animatedPos= BoneTransform * vec4(verts.at(inds.at(i)).Position, 1.0);
        animatedVertices.push_back(animatedPos);
    }
    return 0;
}

void AnimatedMesh::Draw(AnimatedShader shader)
{
    glBindVertexArray(VAO);
    for(unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

     // Make sure the VAO is not changed from the outside
    glBindVertexArray(0);
}

// render the mesh
void AnimatedMesh::DrawInstanced(AnimatedShader shader, std::vector<mat4> modelMatrices)
{

    unsigned int diffuseNr  = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr   = 1;
    unsigned int heightNr   = 1;

    for(unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        string number;
        string name = textures[i].type;
        if(name == "texture_diffuse")
    number = std::to_string(diffuseNr++);
  else if(name == "texture_specular")
    number = std::to_string(specularNr++); // transfer unsigned int to stream
        else if(name == "texture_normal")
    number = std::to_string(normalNr++); // transfer unsigned int to stream
         else if(name == "texture_height")
      number = std::to_string(heightNr++); // transfer unsigned int to stream

                       // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
    glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), &modelMatrices.front(), GL_DYNAMIC_DRAW);


    glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, modelMatrices.size());

     // Make sure the VAO is not changed from the outside
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);

}


void AnimatedMesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &instanceBuffer);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexTransform), &vertices[0], GL_DYNAMIC_DRAW);
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTransform), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTransform), (void*)offsetof(VertexTransform, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTransform), (void*)offsetof(VertexTransform, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTransform), (void*)offsetof(VertexTransform, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTransform), (void*)offsetof(VertexTransform, Bitangent));
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(VertexTransform), (void*)offsetof(VertexTransform, Bones.IDs));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(VertexTransform), (void*)offsetof(VertexTransform, Bones.Weights));


    // instancing attributes (instance matrix)
    std::size_t vec4Size = sizeof(glm::vec4);
    glEnableVertexAttribArray(7); 
    glEnableVertexAttribArray(8); 
    glEnableVertexAttribArray(9);
    glEnableVertexAttribArray(10); 
    glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
    glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
    glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

    glVertexAttribDivisor(7, 1);
    glVertexAttribDivisor(8, 1);
    glVertexAttribDivisor(9, 1);
    glVertexAttribDivisor(10, 1);

    float minX = numeric_limits<float>::max();
    float maxX = numeric_limits<float>::min();
    float minY = numeric_limits<float>::max();
    float maxY = numeric_limits<float>::min();
    float minZ = numeric_limits<float>::max();
    float maxZ = numeric_limits<float>::min();
    min = vec3(minX, minY, minZ);
    max = vec3(maxX, maxY, maxZ);
}

void BoneData::AddBoneData(unsigned int BoneID, float Weight)
{
    for (unsigned int i = 0 ; i < sizeof(IDs)/sizeof(IDs[0]); i++) {
        if (Weights[i] == 0.0) {
            IDs[i]     = BoneID;
            Weights[i] = Weight;
            return;
        }
    }

    // should never get here - more bones than we have space for
    //assert(0);
}

AnimatedMesh::~AnimatedMesh()
{
    Clear();
}

void AnimatedMesh::Clear()
{
    for (unsigned int i = 0 ; i < textures.size() ; i++) {
       // if (textures[i]) { delete textures[i]; textures[i] = NULL; }
    }

    if (VBO != 0) {
        //glDeleteVertexArrays(1, &VBO);
        VBO = 0;
    }

    if (EBO != 0) {
        //glDeleteBuffers(1, &EBO);
        EBO = 0;
    }

    if (VAO != 0) {
        //glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
}





