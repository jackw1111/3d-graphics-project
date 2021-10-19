#include "animated_shader.h"


AnimatedShader::AnimatedShader(const char* vertexPath, const char* fragmentPath) : StaticShader(vertexPath, fragmentPath)
{
    for (unsigned int j = 0; j < MAX_MODELS; j++) {
        for (unsigned int i = 0 ; i < sizeof(m_boneLocation[0])/sizeof(m_boneLocation[0][0]); i++) {
            stringstream Name;
            Name << "gBones[" << j << "][" << i << "]";
            m_boneLocation[j][i] = glGetUniformLocation(ID, Name.str().c_str());
        }
    }    
}
int AnimatedShader::setup(const char* vertexPath, const char* fragmentPath)
{
	StaticShader::setup(vertexPath, fragmentPath);
    for (unsigned int j = 0; j < MAX_MODELS; j++) {
        for (unsigned int i = 0 ; i < sizeof(m_boneLocation[0])/sizeof(m_boneLocation[0][0]); i++) {
            stringstream Name;
            Name << "gBones[" << j << "][" << i << "]";
            m_boneLocation[j][i] = glGetUniformLocation(ID, Name.str().c_str());
        }
    }        
    
    return 0;
}

void AnimatedShader::SetBoneTransform(unsigned int uniqueID, unsigned int Index, const mat4 &Transform)
{
    //assert(Index < MAX_BONES);
    glUniformMatrix4fv(m_boneLocation[uniqueID][Index], 1, GL_FALSE, &Transform[0][0]);
}


