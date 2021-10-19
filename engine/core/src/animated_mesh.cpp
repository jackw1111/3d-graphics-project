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
    float minX = numeric_limits<float>::max();
    float maxX = numeric_limits<float>::min();
    float minY = numeric_limits<float>::max();
    float maxY = numeric_limits<float>::min();
    float minZ = numeric_limits<float>::max();
    float maxZ = numeric_limits<float>::min();
    for(unsigned int i = 0; i < vertices.size(); i++)
    {
        if (vertices[i].Position.x < minX) {
            minX = vertices[i].Position.x;
        } else if (vertices[i].Position.x > maxX) {
            maxX = vertices[i].Position.x;
        }
        if (vertices[i].Position.y < minY) {
            minY = vertices[i].Position.y;
        } else if (vertices[i].Position.y > maxY) {
            maxY = vertices[i].Position.y;
        }
        if (vertices[i].Position.z < minZ) {
            minZ = vertices[i].Position.z;
        } else if (vertices[i].Position.z > maxZ) {
            maxZ = vertices[i].Position.z;
        }
    }
    vector<float> limits = {minX, minY, minZ, maxX, maxY, maxZ};
    return limits;
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
    assert(0);
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





