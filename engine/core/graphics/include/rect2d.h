#ifndef RECT2D_H
#define RECT2D_H

#include <stb_image.h>
#include <algorithm>

#include "engine.h"
#include "camera.h"

/*! @brief 2D rectangle for use in UI/overlays/loading screens etc... */
class Rect2D {

public:

	unsigned char *data;
    int _width, _height, nrChannels;

    glm::vec4 topRight;
    glm::vec4 bottomRight;
    glm::vec4 bottomLeft;
    glm::vec4 topLeft;

    glm::vec2 position;
    glm::vec2 size;

    unsigned int texture;
    std::string filePath;
    unsigned int cols = 1;
    unsigned int rows = 1;

    float col = 1.0f;
    float row = 1.0f;

    float alpha = 1.0f;

    int ordering = 1;

    int toDraw = 1;

    int frame = 0;

    float orientation = 0.0f;

    unsigned int blendMode = GL_ONE_MINUS_SRC_ALPHA;

    ~Rect2D();

    Rect2D(glm::vec2 pos, glm::vec2 sz, std::string _filePath, unsigned int _cols, unsigned int _rows);

    glm::vec2 getPosition();
    void setPosition(glm::vec2 pos);


    float getOrientation();
    void setOrientation(float o);

    glm::vec2 getSize();
    void setSize(glm::vec2 pos);

    int getOrdering();
    void setOrdering(int o);

    int getFrame();
    void setFrame(int f);

    int getToDraw();
    void setToDraw(int d);

    int remove();

    static int drawAllRects(Camera active_camera, int order);
    static void setCamera(glm::mat4 cameraMatrix);

    static StaticShader rectShader;
    static unsigned int VBO, VAO, EBO;
    static std::vector<float> vertices;
    static std::vector<unsigned int> indices; 
    static bool setup;
    static glm::mat4 viewProjection;
    static std::vector<std::vector<Rect2D*>> rects;
    static std::map<std::string, unsigned int> textureMap;

};


#endif
