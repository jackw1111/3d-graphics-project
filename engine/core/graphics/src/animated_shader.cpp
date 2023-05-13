#include "animated_shader.h"


AnimatedShader::AnimatedShader(const char* vertexPath, const char* fragmentPath) : StaticShader(vertexPath, fragmentPath)
{
    for (unsigned int i = 0; i < MAX_MODELS; i++) {
        for (unsigned int j = 0 ; j < MAX_BONES; j++) {
            stringstream Name;
            Name << "gBones[" << i * MAX_BONES + j << "]";
            m_boneLocation[i * MAX_BONES + j] = glGetUniformLocation(ID, Name.str().c_str());
        }
    }    
}
int AnimatedShader::setup(const char* vertexPath, const char* fragmentPath)
{
    StaticShader::setup(vertexPath, fragmentPath);
    for (unsigned int i = 0; i < MAX_MODELS; i++) {
        for (unsigned int j = 0 ; j < MAX_BONES; j++) {
            stringstream Name;
            Name << "gBones[" << i * MAX_BONES + j << "]";
            m_boneLocation[i * MAX_BONES + j] = glGetUniformLocation(ID, Name.str().c_str());
        }
    } 
    return 0;
}

void AnimatedShader::SetBoneTransform(unsigned int uniqueID, unsigned int Index, const mat4 &Transform)
{
    //assert(Index < MAX_BONES);
    glUniformMatrix4fv(m_boneLocation[uniqueID * MAX_BONES + Index], 1, GL_FALSE, &Transform[0][0]);
}
