#include "line3d.h"
std::vector<Line3D*> Line3D::lines {};

// TO DO : pass color to shader for different colored lines

Line3D::Line3D() {
}

Line3D::Line3D(vec3 start, vec3 end) {

    std::cout << "line constructor called!" << std::endl;

    startPoint = start;
    endPoint = end;
    model = mat4(1.0);
    viewProjection = mat4(1.0);

    color = vec3(0.6, 0.6, 0.6);

    const char *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform mat4 model;\n"
        "uniform mat4 viewProjection;\n"

        "void main()\n"
        "{\n"
        "   gl_Position = viewProjection * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"

        "}\0";
    const char *fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "uniform vec3 color;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(color, 1.0f);\n"
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
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    vertices = {
         start.x, start.y, start.z,
         end.x, end.y, end.z,

    };
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

    Line3D::lines.push_back(this);

}

int Line3D::setModel(mat4 m) {
    model = m;
    return 0;
}

int Line3D::setCamera(mat4 cameraMatrix) {
    viewProjection = cameraMatrix;
    return 0;
}

int Line3D::setColor(vec3 col) {
    color = col;
    return 0;
}

int Line3D::draw() {
    //std::cout << "Drawing line!" << std::endl;
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "viewProjection"), 1, GL_FALSE, &viewProjection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);

    glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &color[0]);

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 2);
    return 0;
}

int Line3D::setEndpoints(vec3 start, vec3 end) {
    startPoint = start;
    endPoint = end;
    vertices = {
         startPoint.x, startPoint.y, startPoint.z,
         endPoint.x, endPoint.y, endPoint.z,

    };
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices.data());
    return 0;

}

Line3D::~Line3D() {

    std::cout << "size of lines: " << Line3D::lines.size() << std::endl;
    std::cout << "deleting line3D..." << std::endl;
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    std::vector<Line3D*>::iterator found = std::find(Line3D::lines.begin(),Line3D::lines.end(),this);
    if (found != Line3D::lines.end()) {
        Line3D::lines.erase(found);
    }
}



