#ifndef STATIC_MESH_H
#define STATIC_MESH_H

#include <vector>
#include <random>
#include <glm/glm.hpp>

#include "static_shader.h"

using namespace std;
using glm::mat4;

enum OcclusionState {HIDDEN, VISIBLE, WAITING};

class Material {
public:
     //Material color lighting
    glm::vec4 Ka;
     //Diffuse reflection
    glm::vec4 Kd;
     //Mirror reflection
    glm::vec4 Ks;

    // Material& operator=(const Material &mat);
    // Material(const Material &mat);
};


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

struct Texture {
    bool operator==(const Texture t) { return this == &t; }
    unsigned int id;
    string type;
    string path;
};

class BCube {
public:
    unsigned int shaderProgram;
    unsigned int VBO, VAO;

    GLuint queryFront;
    GLuint queryBack;

    std::vector<glm::vec3> translated_vertices = {};
    std::vector<float> vertices = {};

    BCube(){};

    int getTranslatedVertices(glm::mat4 modelMatrix) {
        this->translated_vertices = {};
        for (unsigned int i = 0; i < vertices.size(); i+=3) {
            glm::vec3 v = glm::vec3(vertices.at(i), vertices.at(i+1), vertices.at(i+2));
            glm::vec4 _v = glm::vec4(v.x, v.y, v.z, 1.0);
            glm::vec4 tv = modelMatrix * _v;
            this->translated_vertices.push_back(glm::vec3(tv.x, tv.y, tv.z));
        }
        return 0;
    }

    void setup(glm::vec3 min, glm::vec3 max) {

        glGenQueries(1, &queryFront);
        glGenQueries(1, &queryBack);

        float verts[] = {
            max[0], min[1], min[2], // 1
            min[0], min[1], min[2], // 0
            max[0], max[1], min[2], // 2
            min[0], min[1], min[2], // 0
            min[0], max[1], min[2], // 3
            max[0], max[1], min[2], // 2

            min[0], min[1], max[2], // 0
            max[0], min[1], max[2], // 1
            max[0], max[1], max[2], // 2
            max[0], max[1], max[2], // 2
            min[0], max[1], max[2], // 3
            min[0], min[1], max[2], // 0

            min[0], max[1], max[2],
            min[0], max[1], min[2],
            min[0], min[1], min[2],
            min[0], min[1], min[2],
            min[0], min[1], max[2],
            min[0], max[1], max[2],

            max[0], max[1], min[2], // 1
            max[0], max[1], max[2], // 0
            max[0], min[1], min[2], // 2
            max[0], max[1], max[2], // 0
            max[0], min[1], max[2], // 3
            max[0], min[1], min[2], // 2

            min[0], min[1], min[2], //0
            max[0], min[1], min[2], //1
            max[0], min[1], max[2], //2
            max[0], min[1], max[2], //2
            min[0], min[1], max[2], //3
            min[0], min[1], min[2], //0

            max[0], max[1], min[2], //1
            min[0], max[1], min[2], //0
            max[0], max[1], max[2], //2
            min[0], max[1], min[2], //0
            min[0], max[1], max[2], //3
            max[0], max[1], max[2], //2

        };

        vertices = std::vector<float>(verts, verts+108);

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        const char *vertexShaderSource = "#version 330 core\n"
            "layout (location = 0) in vec3 aPos;\n"
            "uniform mat4 model;\n"
            "uniform mat4 view;\n"
            "uniform mat4 projection;\n"
            "out vec4 outColor;\n"
            "void main()\n"
            "{\n"
            "   gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
            "   outColor = vec4(aPos,1.0f);\n"
            "}\0";
        const char *fragmentShaderSource = "#version 330 core\n"
            "out vec4 FragColor;\n"
            "in vec4 outColor;\n"
            "void main()\n"
            "{\n"
            "   FragColor = outColor;\n"
            "}\n\0";

        // vertex shader
        int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        // check for shader compile errors
        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
        }
        // fragment shader
        int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        // check for shader compile errors
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
        }
        // link shaders
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        // check for linking errors
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void setMatrix(std::string name, glm::mat4 mat) {
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void draw() {
        glUseProgram(shaderProgram);

        // render box
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
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

    BCube boundingCube;
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
    std::vector<std::vector<glm::vec3>> getTriangles(glm::mat4 model);
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

