#ifndef STATIC_SHADER_H
#define STATIC_SHADER_H

#define GLM_ENABLE_EXPERIMENTAL
// #define GLM_FORCE_SSE2
// #define GLM_FORCE_ALIGNED
// #define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
// #include <glm/glm.hpp>
// #include <xmmintrin.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "glad/glad.h"
#include <stdexcept>


float to_float(uint32_t f);

/// @brief A shader class used to light StaticModel type objects (ie. with no animations)
class StaticShader
{

public:
    int _isInit = false;
    int _isBound = false;
    unsigned int ID;
    /// @brief A shader class used to light StaticModel type objects (ie. with no animations)
    StaticShader();
    StaticShader(const StaticShader &other);
    StaticShader& operator=(StaticShader other);

    /// @brief initiliazation function for heap allocation and similar
    /// @param vertexPath the filepath of the vertex shader
    /// @param fragmentPath the filepath of the fragment shader
    int setup(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);

    /// @brief Stack allocation constructor
    /// @param vertexPath the filepath of the vertex shader
    /// @param fragmentPath the filepath of the fragment shader
    StaticShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);

    /// @brief activate the shader
    int use();

    /// @brief utility function for sending/setting booleans to GLSL
    /// @param name the uniform to query
    /// @param value the ID of the uniform
    void setBool(const std::string &name, bool value) const;

    /// @brief utility function for sending/setting ints to GLSL
    /// @param name the uniform to query
    /// @param value the ID of the uniform
    void setInt(const std::string &name, int value) const;

    /// @brief utility function for sending/setting floats to GLSL
    /// @param name the uniform to query
    /// @param value the ID of the uniform
    int setFloat(const std::string &name, float value) const;

    /// @brief utility function for sending/setting float arrays to GLSL
    /// @param name the uniform to query
    /// @param value the ID of the uniform
    int setFloatArray(const std::string &name, unsigned int sz, std::vector<float> v) const;

    /// @brief utility function for sending/setting vec2 to GLSL
    /// @param name the uniform to query
    /// @param sz size of the array
    /// @param v the data to send
    void setVec2(const std::string &name, const glm::vec2 &value) const;

    /// @brief utility function for sending/setting vec3 to GLSL
    /// @param name the uniform to query
    /// @param value the data to send
    int setVec3(const std::string &name, const glm::vec3& value) const;

    /// @brief utility function for sending/setting vec4 to GLSL
    /// @param name the uniform to query
    /// @param v the data to send
    void setVec4(const std::string &name, const glm::vec4 &value) const;

    /// @brief utility function for sending/setting vec4 to GLSL
    /// @param name the uniform to query
    /// @param x the data to send
    /// @param y the data to send
    /// @param z the data to send
    /// @param w the data to send
    void setVec4(const std::string &name, float x, float y, float z, float w) const;

    /// @brief utility function for sending/setting mat2 to GLSL
    /// @param name the uniform to query
    /// @param mat the matrix to send
    void setMat2(const std::string &name, const glm::mat2 &mat) const;

    /// @brief utility function for sending/setting mat3 to GLSL
    /// @param name the uniform to query
    /// @param mat the matrix to send
    void setMat3(const std::string &name, const glm::mat3 &mat) const;

    /// @brief utility function for sending/setting mat4 to GLSL
    /// @param name the uniform to query
    /// @param mat the matrix to send
    int setMat4(std::string name, glm::mat4);

    /// @brief utility function for sending/setting mat4 to GLSL
    /// @param name the uniform to query
    /// @param mat the matrix to send
    int _setMat4(const char* name, unsigned long mataddr);

private:
    /// @brief utility function for checking shader compilation/linking errors.
    /// @param shader the shader to query
    /// @param type the error type to query
    void checkCompileErrors(unsigned int shader, std::string type);
};

#endif
