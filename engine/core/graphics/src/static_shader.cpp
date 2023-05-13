#include "static_shader.h"

const char* _vShader = 
"#version 300 es\n"
"layout (location = 0) in vec3 aPos;\n"
"void main(){\n"
"    gl_Position = vec4(0,0,0,1);\n"
"}\n";
const char* _fShader =
"#version 300 es\n"
"precision mediump float;\n"
"out vec4 FragColor;\n"
"void main(){\n"
"    FragColor = vec4(1,0,0,1);\n"
"}\n";


float to_float(uint32_t f) 
{
    union int_float {
        uint32_t i;
        float f;
    } tofloat;

    tofloat.i = f;
    return tofloat.f;
}


StaticShader::StaticShader() {
    //_isInit = false;
}

StaticShader::StaticShader(const StaticShader &other){
    _isInit = other._isInit;
    _isBound = other._isBound;

}
StaticShader& StaticShader::operator=(StaticShader other){ return *this; }

StaticShader::StaticShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
    _isInit = setup(vertexPath, fragmentPath, geometryPath);
}

int StaticShader::setup(const char* vertexShader, const char* fragmentShader, const char* geometryPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;

    // ensure ifstream objects can throw exceptions:
    // vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    // fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    // gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // open files
        // vShaderFile.open(vertexPath);
        // fShaderFile.open(fragmentPath);
        // std::stringstream vShaderStream, fShaderStream;

        // // read file's buffer contents into streams
        // vShaderStream << vShaderFile.rdbuf();
        // fShaderStream << fShaderFile.rdbuf();
        // // close file handlers
        // vShaderFile.close();
        // fShaderFile.close();

        // // convert stream into string
        // vertexCode = vShaderStream.str();
        // fragmentCode = fShaderStream.str();

        // // if geometry shader path is present, also load a geometry shader
        // if(geometryPath != nullptr)
        // {

        //     gShaderFile.open(geometryPath);
        //     std::stringstream gShaderStream;
        //     gShaderStream << gShaderFile.rdbuf();
        //     gShaderFile.close();
        //     geometryCode = gShaderStream.str();
        // }

    }
    catch (std::ifstream::failure &e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexShader;
    const char * fShaderCode = fragmentShader;

    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // fragment AnimatedShader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");


    // if geometry shader is given, compile geometry shader
    unsigned int geometry;
    if(geometryPath != nullptr)
    {
        const char * gShaderCode = geometryCode.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        checkCompileErrors(geometry, "GEOMETRY");
    }

    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    if(geometryPath != nullptr)
        glAttachShader(ID, geometry);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if(geometryPath != nullptr) {
        geometry = 1;
        glDeleteShader(geometry);
    }

    _isInit = true;
    _isBound = false;
    return 1;

}
// activate the shader
// ------------------------------------------------------------------------
int StaticShader::use()
{
    // if (!_isInit) {
    //     throw std::runtime_error("StaticShader has not been initialized!");
    // }
    glUseProgram(ID);
    _isBound = true;
    return 1;
}
// utility uniform functions
// ------------------------------------------------------------------------
void StaticShader::setBool(const std::string &name, bool value) const
{
    // if (!_isInit || !_isBound) {
    //     throw std::runtime_error("StaticShader has not been initialized or bound!");
    // }
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
// ------------------------------------------------------------------------
void StaticShader::setInt(const std::string &name, int value) const
{
    // if (!_isInit || !_isBound) {
    //     throw std::runtime_error("StaticShader has not been initialized or bound!");
    // }
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
int StaticShader::setFloat(const std::string &name, float value) const
{
    // if (!_isInit || !_isBound) {
    //     throw std::runtime_error("StaticShader has not been initialized or bound!");
    // }
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    return 0;
}

// ------------------------------------------------------------------------
int StaticShader::setFloatArray(const std::string &name, unsigned int sz, std::vector<float> v) const
{
    // if (!_isInit || !_isBound) {
    //     throw std::runtime_error("StaticShader has not been initialized or bound!");
    // }
    glUniform1fv(glGetUniformLocation(ID, name.c_str()), sz, v.data());
    return 0;
}
// ------------------------------------------------------------------------
void StaticShader::setVec2(const std::string &name, const glm::vec2 &value) const
{
    // if (!_isInit || !_isBound) {
    //     throw std::runtime_error("StaticShader has not been initialized or bound!");
    // }
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
// void StaticShader::setVec2(const std::string &name, float x, float y) const
// {
//     if (!_isInit) {
//         assert("StaticShader has not been initialized!");
//     }
//     glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
// }
// ------------------------------------------------------------------------
int StaticShader::setVec3(const std::string &name, const glm::vec3& value) const
{
    // if (!_isInit || !_isBound) {
    //     throw std::runtime_error("StaticShader has not been initialized or bound!");
    // }
    glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
    return 0;
}
// void StaticShader::setVec3(const std::string &name, float x, float y, float z) const
// {
//     if (!_isInit) {
//         assert("StaticShader has not been initialized!");
//     }
//     glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
// }
// ------------------------------------------------------------------------
void StaticShader::setVec4(const std::string &name, const glm::vec4 &value) const
{
    // if (!_isInit || !_isBound) {
    //     throw std::runtime_error("StaticShader has not been initialized or bound!");
    // }
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void StaticShader::setVec4(const std::string &name, float x, float y, float z, float w) const
{
    // if (!_isInit || !_isBound) {
    //     throw std::runtime_error("StaticShader has not been initialized or bound!");
    // }
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void StaticShader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
    // if (!_isInit || !_isBound) {
    //     throw std::runtime_error("StaticShader has not been initialized or bound!");
    // }

    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void StaticShader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    // if (!_isInit || !_isBound) {
    //     throw std::runtime_error("StaticShader has not been initialized or bound!");
    // }

    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
int StaticShader::setMat4(std::string name, glm::mat4 mat)
{
    // if (!_isInit || !_isBound) {
    //     throw std::runtime_error("StaticShader has not been initialized or bound!");
    // }

    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    return 0;
}


int StaticShader::_setMat4(const char* name, unsigned long mataddr)
{
    // if (!_isInit || !_isBound) {
    //     throw std::runtime_error("StaticShader has not been initialized or bound!");
    // }
    
    int* mat = (int*)mataddr;
    float fmat[16];
    for (int i = 0; i < 16; i++) {
        fmat[i] = to_float(mat[i]);
    }
    glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, fmat);
    return 0;
}

void StaticShader::checkCompileErrors(GLuint shader, std::string type)
{
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}
