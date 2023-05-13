#include "line2d.h"

std::vector<Line2D*> Line2D::lines {};

Line2D::Line2D(vec2 start, vec2 end, float WIDTH, float HEIGHT) {

    float x1 = start.x;
    float y1 = start.y;
    float x2 = end.x;
    float y2 = end.y;
    w = WIDTH;
    h = HEIGHT;
 
    // convert 3d world space position 2d screen space position
    x1 = 2*x1 / w - 1;
    y1 = 2*y1 / h - 1;
 
    x2 = 2*x2 / w - 1;
    y2 = 2*y2 / h - 1;
 
    start.x = x1;
    start.y = y1;
    end.x = x2;
    end.y = y2;

    startPoint = start;
    endPoint = end;
    lineColor = vec3(1,1,1);

    const char *vertexShaderSource = "#version 300 es\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform mat4 MVP;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = MVP * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    const char *fragmentShaderSource = "#version 300 es\n"
        "precision mediump float;\n"
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

    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors

    // link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    vertices = {
         start.x, start.y, 0.0,
         end.x, end.y, 0.0,

    };
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)*vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 
    Line2D::lines.push_back(this);
}

int Line2D::setMVP(mat4 mvp) {
    MVP = mvp;
    return 1;
}

int Line2D::setColor(vec3 color) {
    lineColor = color;
    return 1;
}

void Line2D::setEndpoints(vec2 start, vec2 end) {

    startPoint = start;
    endPoint = end;

    float x1 = start.x;
    float y1 = start.y;
    float x2 = end.x;
    float y2 = end.y;
 
    // convert 3d world space position 2d screen space position
    x1 = 2*x1 / w - 1;
    y1 = 2*y1 / h - 1;
 
    x2 = 2*x2 / w - 1;
    y2 = 2*y2 / h - 1;
 
    start.x = x1;
    start.y = y1;
    end.x = x2;
    end.y = y2;

    vertices = {
         start.x, start.y, 0.0,
         end.x, end.y, 0.0
    };

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vertices.size(), vertices.data());

}

int Line2D::draw() {
    glUseProgram(shaderProgram);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, &MVP[0][0]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &lineColor[0]);

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 2);
    return 1;
}

Line2D::~Line2D() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}