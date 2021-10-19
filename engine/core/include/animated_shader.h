#ifndef ANIMATED_SHADER_H
#define	ANIMATED_SHADER_H

#include <sstream>

#include "math_utils.h"
#include "static_shader.h"

using std::stringstream;

class AnimatedShader : public StaticShader  {
public:
    static const unsigned int MAX_BONES = 50;
    // renderer drawInstances MAX_MODELS number of models at a time (because max 1024 mat4's sent to shader at one time)
    static const unsigned int MAX_MODELS = 20;
    AnimatedShader(){};
    AnimatedShader(const char* vertexPath, const char* fragmentPath);
    int setup(const char* vertexPath, const char* fragmentPath);
    void SetBoneTransform(unsigned int uniqueID, unsigned int Index, const mat4& Transform);

private:
	vector<vector<mat4> > bonesMatrix;
    GLuint m_boneLocation[MAX_MODELS][MAX_BONES];
};


#endif	/* ANIMATED_SHADER_H */
