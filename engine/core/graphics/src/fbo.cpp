#include "fbo.h"
#include <stb_image.h>
#include <boost/python.hpp>


FBO::FBO() {

}
int FBO::setup(int width, int height, bool position_buffer=true, bool normal_buffer=true, bool color_buffer=true, bool depth_buffer=true) {
    WIDTH = width;
    HEIGHT = height;

    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    // screen quad VAO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);


    if (position_buffer) {
        createPositionBuffer();
    }
    if (normal_buffer) {
        createNormalBuffer();
    }
    if (color_buffer) {
        createColorBuffer();
    }

    if (depth_buffer) {
        createDepthBuffer();
    }


    // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return 0;
}


FBO::FBO(const char *path, const string &directory) {

}
FBO::FBO(int width, int height, bool position_buffer=true, bool normal_buffer=true, bool color_buffer=true, bool depth_buffer=true) {
    setup(width, height, position_buffer, normal_buffer, color_buffer, depth_buffer);
}

void FBO::handleError(int error) {
    if (error == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
        std::cout << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" << std::endl;
    } else if (error == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) {
        std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" << std::endl;
    } else if (error == GL_FRAMEBUFFER_UNSUPPORTED) {
        std::cout << "GL_FRAMEBUFFER_UNSUPPORTED" << std::endl;
    }
}

unsigned int FBO::getColorBuffer(){
    if (hasColorBuffer) {
        return color_texture;
    } else {
        throw std::runtime_error("color buffer has not been initialized!");
    }
}
unsigned int FBO::getDepthBuffer(){
    if (hasDepthBuffer) {
        return depth_texture;
    } else {
        throw std::runtime_error("depth buffer has not been initialized!");
    }
}
unsigned int FBO::getNormalBuffer(){
    if (hasNormalBuffer) {
        return normal_texture;
    } else {
        throw std::runtime_error("normal buffer has not been initialized!");
    }
}
unsigned int FBO::getPositionBuffer(){
    if (hasPositionBuffer) {
        return position_texture;
    } else {
        throw std::runtime_error("position buffer has not been initialized!");
    }
}

void FBO::createNormalBuffer() {

    // - normal color buffer
    glGenTextures(1, &normal_texture);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, normal_texture);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA, WIDTH, HEIGHT, GL_TRUE);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, normal_texture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
        int error = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        handleError(error);
    }else {
        hasNormalBuffer = true;
    }
}

void FBO::createColorBuffer() {
    
    // - color + specular color buffer
    glGenTextures(1, &color_texture);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, color_texture);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA, WIDTH, HEIGHT, GL_TRUE);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D_MULTISAMPLE, color_texture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
        int error = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        handleError(error);
    } else {
        hasColorBuffer = true;
    }
}

void FBO::createDepthBuffer() {
    // Create the depth buffer
    // glGenTextures(1, &depth_texture);
    // glBindTexture(GL_TEXTURE_2D, depth_texture);
    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);

    glGenTextures(1, &depth_texture);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depth_texture);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_DEPTH_COMPONENT, WIDTH, HEIGHT, GL_TRUE);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    // float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor); 

    // sampling outside of depth map results in 1.0
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor); 

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, depth_texture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
        int error = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        handleError(error);
    }else {
        hasDepthBuffer = true;
    }

}

void FBO::createPositionBuffer() {
    // create a color attachment texture for position
    glGenTextures(1, &position_texture);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, position_texture);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA16F, WIDTH, HEIGHT, GL_TRUE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, position_texture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
        int error = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        handleError(error);
    }
    else {
        hasPositionBuffer = true;
    }
}

FBO::~FBO() {
    glDeleteFramebuffers(1, &framebuffer);
}

void FBO::Draw(StaticShader shader) {
    glViewport(0, 0, WIDTH, HEIGHT);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}   


void FBO::bind(bool value) {
    // value of 0 binds back to default buffer
    if (value) {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}


