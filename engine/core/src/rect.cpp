#include "rect.h"


extern unsigned int WIDTH;
extern unsigned int HEIGHT;

std::vector<Rect*> Rect::rects = {};

Rect::Rect(glm::vec2 pos, glm::vec2 sz, std::string filePath) {
    ordering = 1;
    colour = glm::vec3(1,1,1);
    position = pos;
    size = sz;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);


    float width = size.x;
    float height = size.y;

    float x = 2*position.x / WIDTH - 1;
    float y = 2*position.y / HEIGHT- 1;

    float hw = width / (float)WIDTH;
    float hh = height / (float)HEIGHT;


        // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // colors           // texture coords
        x + hw,  y + hh, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
        x + hw,  y - hh, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        x - hw,  y - hh, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        x - hw,  y + hh, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };


    unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    // setup geometry storage
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

    // setup textures
    rectShader.setup("../../shaders/rect_shader.vs",
                     "../../shaders/rect_shader.fs"); 

    Rect::rects.push_back(this);
    stbi_set_flip_vertically_on_load(true);  

    data = stbi_load(filePath.c_str(), &_width, &_height, &nrChannels, STBI_rgb_alpha);
    // couldn't find prev model object; load
    glGenTextures(1, &texture);
    setupWithImage(filePath);

}


Rect::~Rect() {
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    //glDeleteVertexArrays(1, &VAO);
    //glDeleteBuffers(1, &VBO);
    //glDeleteBuffers(1, &EBO);

}

int Rect::setupWithImage(std::string imageLocation) {


    glActiveTexture(GL_TEXTURE0);

    // load and create a texture 
    // -------------------------

    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);


    withImage = true;
    return 0;
}

void Rect::setupWithColour(glm::vec3 col) {

}

int Rect::draw() {

    float width = size.x;
    float height = size.y;
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    //glDisable(GL_BLEND);

    float x = 2*position.x / WIDTH - 1;
    float y = 2*position.y / HEIGHT- 1;

    float hw = width / (float)WIDTH;
    float hh = height / (float)HEIGHT;


        // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // colors           // texture coords
        x + hw,  y + hh, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
        x + hw,  y - hh, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        x - hw,  y - hh, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        x - hw,  y + hh, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };



    rectShader.use();
    rectShader.setInt("texture1", 0);
    rectShader.setInt("shaded", shaded);

    // render quad
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    return 0;
}

int Rect::remove() {
    Rect::rects.erase(std::find(Rect::rects.begin(),Rect::rects.end(),this));
    return 0;
}
