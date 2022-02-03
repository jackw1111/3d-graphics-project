#include "line3d.h"

std::vector<Line3D*> Line3D::lines {};
int Line3D::shaderProgram;
unsigned int Line3D::VBO;
unsigned int Line3D::VAO;
vector<float> Line3D::vertices;
bool Line3D::setup = false;
bool Line3D::dirtyPositions = true;
bool Line3D::dirtyColors = true;
bool Line3D::dirtyLines = true;
mat4 Line3D::viewProjection;

// TO DO : pass color to shader for different colored lines

Line3D::Line3D() {
}

Line3D::Line3D(vec3 start, vec3 end) {

    //std::cout << "line constructor called!" << std::endl;

    startPoint = start;
    endPoint = end;
    color = vec3(0.6, 0.6, 0.6);

    if (!Line3D::setup) {
        Line3D::viewProjection = mat4(1.0);

        const char *vertexShaderSource = "#version 330 core\n"
            "layout (location = 0) in vec3 aPos;\n"
            "layout (location = 1) in vec3 aColor;\n"
            "out vec3 color;\n"
            "uniform mat4 viewProjection;\n"
            "void main()\n"
            "{\n"
            "   gl_Position = viewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
            "   color = aColor;\n"
            "}\0";
        const char *fragmentShaderSource = "#version 330 core\n"
            "in vec3 color;\n"
            "out vec4 FragColor;\n"
            "void main()\n"
            "{\n"
            "   FragColor = vec4(color,1);\n"
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
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
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
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        // link shaders
        Line3D::shaderProgram = glCreateProgram();
        glAttachShader(Line3D::shaderProgram, vertexShader);
        glAttachShader(Line3D::shaderProgram, fragmentShader);
        glLinkProgram(Line3D::shaderProgram);
        // check for linking errors
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        Line3D::vertices = {
        
        };
        
        glGenVertexArrays(1, &Line3D::VAO);
        glGenBuffers(1, &Line3D::VBO);
        glBindVertexArray(Line3D::VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Line3D::vertices), Line3D::vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        glBindVertexArray(0); 
        Line3D::setup = true;
    }

    Line3D::lines.push_back(this);

}

int Line3D::setCamera(mat4 cameraMatrix) {
    viewProjection = cameraMatrix;
    return 0;
}

int Line3D::setColor(vec3 col) {
    color = col;
    dirtyColors = true;
    return 0;
}

int Line3D::setProject(int p) {
    project = p;
    return 0;
}
int Line3D::getProject() {
    return project;
}

int Line3D::drawAllLines() {
    if (dirtyPositions || dirtyColors || dirtyLines) {
        Line3D::vertices = {};
        for (unsigned int i = 0; i < Line3D::lines.size(); i++) {
            vec3 start = Line3D::lines.at(i)->startPoint;
            vec3 end = Line3D::lines.at(i)->endPoint;
            vec3 color =  Line3D::lines.at(i)->color;

            vector<float> lineVertices = {
                 start.x, start.y, start.z, color.x, color.y, color.z,
                 end.x, end.y, end.z, color.x, color.y, color.z
            };

            Line3D::vertices.insert( Line3D::vertices.end(), lineVertices.begin(), lineVertices.end() );

        }     
        
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * Line3D::vertices.size(), Line3D::vertices.data(), GL_STATIC_DRAW);
        dirtyPositions = false;
        dirtyColors = false;
        dirtyLines = false;

    }


    //std::cout << "Drawing line!" << std::endl;
    glUseProgram(Line3D::shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(Line3D::shaderProgram, "viewProjection"), 1, GL_FALSE, &Line3D::viewProjection[0][0]);

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, vertices.size() / 6);


    return 0;
}

int Line3D::setEndpoints(vec3 start, vec3 end) {
    startPoint = start;
    endPoint = end;
    dirtyPositions = true;
    return 0;

}

Line3D::~Line3D() {

    //std::cout << "size of lines: " << Line3D::lines.size() << std::endl;
    //std::cout << "deleting line3D..." << std::endl;
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    // glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &VBO);
    // glDeleteProgram(shaderProgram);

    std::vector<Line3D*>::iterator found = std::find(Line3D::lines.begin(),Line3D::lines.end(),this);
    if (found != Line3D::lines.end()) {
        Line3D::lines.erase(found);
    }
    dirtyLines = true;
}

