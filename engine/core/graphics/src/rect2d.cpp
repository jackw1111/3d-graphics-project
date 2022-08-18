#include "rect2d.h"
extern unsigned int WIDTH;
extern unsigned int HEIGHT;

std::vector<std::vector<Rect2D*>> Rect2D::rects {};
StaticShader Rect2D::rectShader;
unsigned int Rect2D::VBO;
unsigned int Rect2D::VAO;
unsigned int Rect2D::EBO;
vector<float> Rect2D::vertices;
vector<unsigned int> Rect2D::indices; 
bool Rect2D::setup = false;
mat4 Rect2D::viewProjection;
std::map<std::string, unsigned int> Rect2D::textureMap;

glm::vec2 rotatePoint(glm::vec2 pt, float angle) {
    float x = pt.x;
    float y = pt.y;
    float _x = x * cos(radians(angle)) - y * sin(radians(angle));
    float _y = x * sin(radians(angle)) + y * cos(radians(angle));
    return glm::vec2(_x, _y);
}

Rect2D::Rect2D(glm::vec2 pos, glm::vec2 sz, std::string _filePath, unsigned int _cols, unsigned int _rows) {

    filePath = _filePath;
    cols = _cols;
    rows = _rows;
    setOrdering(1);

    position = pos;
    size = sz;

    float width = size.x;
    float height = size.y;

    float x = 2*position.x / WIDTH - 1;
    float y = 2*position.y / HEIGHT- 1;

    float hw = width / (float)WIDTH;
    float hh = height / (float)HEIGHT;


	topRight = glm::vec4(x + hw,  y + hh, 0.0f, 1.0f);
	bottomRight = glm::vec4(x + hw,  y - hh, 0.0f, 1.0f);
	bottomLeft = glm::vec4(x - hw,  y - hh, 0.0f, 1.0f);
	topLeft = glm::vec4(x - hw,  y + hh, 0.0f, 1.0f);

    if (!Rect2D::setup) {
	    glGenVertexArrays(1, &Rect2D::VAO);
	    glGenBuffers(1, &Rect2D::VBO);
	    glGenBuffers(1, &Rect2D::EBO);


	    // set up vertex data (and buffer(s)) and configure vertex attributes
	    // ------------------------------------------------------------------
	    Rect2D::vertices = {
	        // positions             // texture coords
	        0.5f,  0.5f, 0.0f, 1.0f, 1.0f, // top right
	        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
	       -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
	       -0.5f,  0.5f, 0.0f, 0.0f, 1.0f  // top left 
	    };


	    Rect2D::indices = {  
	        0, 1, 3, // first triangle
	        1, 2, 3  // second triangle
	    };

	    // setup geometry storage
	    glBindVertexArray(Rect2D::VAO);

	    glBindBuffer(GL_ARRAY_BUFFER, Rect2D::VBO);
	    glBufferData(GL_ARRAY_BUFFER,  Rect2D::vertices.size() * sizeof(float), Rect2D::vertices.data(), GL_STATIC_DRAW);

	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Rect2D::indices.size() * sizeof(unsigned int), Rect2D::indices.data(), GL_STATIC_DRAW);

	    // position attribute
	    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	    glEnableVertexAttribArray(0);
	    // texture coord attribute
	    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	    glEnableVertexAttribArray(1);
	    // alpha attribute
	    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));
	    glEnableVertexAttribArray(2);


	    glBindBuffer(GL_ARRAY_BUFFER, 0); 
	    glBindVertexArray(0); 

	    // setup textures
	    rectShader.setup("../../shaders/rect2d_shader.vs",
	                     "../../shaders/rect2d_shader.fs"); 

	    Rect2D::setup = true;
	}

    bool found = false;

	std::map<std::string, unsigned int>::iterator it;

	for (it = Rect2D::textureMap.begin(); it != Rect2D::textureMap.end(); it++) {
    	if (filePath == it->first) {
    		texture = it->second;
    		found = true;
    		break;
    	}
	}

    if (!found) {
	    //stbi_set_flip_vertically_on_load(true);  

	    data = stbi_load(filePath.c_str(), &_width, &_height, &nrChannels, STBI_rgb_alpha);
	    // couldn't find prev model object; load
	    glGenTextures(1, &texture);

	    glActiveTexture(GL_TEXTURE0 + Rect2D::textureMap.size());

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
	    Rect2D::textureMap.insert(std::make_pair(filePath, texture));
    }
    found = false;
	for (unsigned int i = 0; i < Rect2D::rects.size(); i++) {
		if (Rect2D::rects.at(i).at(0)->filePath == filePath) {
			Rect2D::rects.at(i).push_back(this);
			found = true;
			break;
		}
	}
	if (!found) {
    	Rect2D::rects.push_back({this});
	}

}

void Rect2D::setCamera(glm::mat4 proj_view) {
	rectShader.use();
	rectShader.setMat4("viewProjection", proj_view);
}

glm::vec2 Rect2D::getPosition() {
	return position;
}

void Rect2D::setPosition(glm::vec2 pos) {
	position = pos;

    glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f + ordering * 0.01));
    m = glm::rotate(m, glm::radians(orientation), glm::vec3(0,0,1));
    m = glm::scale(m, glm::vec3(size.x, size.y, 1.0f));

    // TO DO mat4 could be calculated in the vertex shader
    topRight = m * glm::vec4(0.5f,  0.5f, 0.0f, 1.0f);
    bottomRight = m * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);
    bottomLeft = m * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
    topLeft = m * glm::vec4(-0.5f,  0.5f, 0.0f, 1.0f);
}

float Rect2D::getOrientation() {
	return orientation;
}

void Rect2D::setOrientation(float o) {
	orientation = o;

    glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f + ordering * 0.01));
    m = glm::rotate(m, glm::radians(orientation), glm::vec3(0,0,1));
    m = glm::scale(m, glm::vec3(size.x, size.y, 1.0f));

    // TO DO mat4 could be calculated in the vertex shader
    topRight = m * glm::vec4(0.5f,  0.5f, 0.0f, 1.0f);
    bottomRight = m * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);
    bottomLeft = m * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
    topLeft = m * glm::vec4(-0.5f,  0.5f, 0.0f, 1.0f);
}

glm::vec2 Rect2D::getSize() {
	return size;
}

void Rect2D::setSize(glm::vec2 sz) {
	size = sz;

    glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f + ordering * 0.01));
    m = glm::rotate(m, glm::radians(orientation), glm::vec3(0,0,1));
    m = glm::scale(m, glm::vec3(size.x, size.y, 1.0f));

    // TO DO mat4 could be calculated in the vertex shader
    topRight = m * glm::vec4(0.5f,  0.5f, 0.0f, 1.0f);
    bottomRight = m * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);
    bottomLeft = m * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
    topLeft = m * glm::vec4(-0.5f,  0.5f, 0.0f, 1.0f);
}

int Rect2D::getToDraw() {
	return toDraw;
}
void Rect2D::setToDraw(int d) {
	toDraw = d;
}

int Rect2D::getFrame() {
	return frame;
}
// TO DO weird indexing bug to the correct frame
void Rect2D::setFrame(int f) {
	frame = f;
	row = (int)(frame / cols);
    col = frame - cols * rows;
}

int Rect2D::getOrdering() {
	return ordering;
}

void Rect2D::setOrdering(int o) {

	ordering = o;

    float width = size.x;
    float height = size.y;

    float x = position.x;//2*position.x / WIDTH - 1;
    float y = position.y;//2*position.y / HEIGHT- 1;

    float hw = width / 2.0f;//(float)WIDTH;
    float hh = height / 2.0f;//(float)HEIGHT;


	topRight = glm::vec4(x + hw,  y + hh, 0.0f + ordering * 0.01, 1.0f);
	bottomRight = glm::vec4(x + hw,  y - hh, 0.0f + ordering * 0.01, 1.0f);
	bottomLeft = glm::vec4(x - hw,  y - hh, 0.0f + ordering * 0.01, 1.0f);
	topLeft = glm::vec4(x - hw,  y + hh, 0.0f + ordering * 0.01, 1.0f);
}

Rect2D::~Rect2D() {
	std::cout << "deleting rect2d..." << std::endl;
	this->ordering = 0;
	//this->remove();
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    //glDeleteVertexArrays(1, &VAO);
    //glDeleteBuffers(1, &VBO);
    //glDeleteBuffers(1, &EBO);

}

int Rect2D::drawAllRects(Camera active_camera, int ordering) {

	glClear(GL_DEPTH_BUFFER_BIT);
    //glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    //glDisable(GL_BLEND);

    // std::sort( Rect2D::rects.begin( ), Rect2D::rects.end( ), [ ]( Rect2D *lhs, Rect2D *rhs )
    // {
    //    return lhs->ordering < rhs->ordering;
    // });

	for (unsigned int i = 0; i < Rect2D::rects.size(); i++) {

	    Rect2D::vertices = {};
	    Rect2D::indices = {};

	    rectShader.use();
	    std::vector<Rect2D*> rectBatch = Rect2D::rects.at(i);

	    glBlendFunc(GL_SRC_ALPHA, rectBatch.at(0)->blendMode);
		for (unsigned int j = 0; j < rectBatch.size(); j++) {
			Rect2D *uniqueRect = rectBatch.at(j);
			if (uniqueRect->toDraw) {

			// if (Rect2D::rects.at(i).at(j)->ordering != ordering) {
			// 	continue;
			// }

			unsigned int cols = uniqueRect->cols;
			unsigned int rows = uniqueRect->rows;

			unsigned int col = (unsigned int)uniqueRect->col;
			unsigned int row = (unsigned int)uniqueRect->row;

			float alpha = uniqueRect->alpha;

		    glm::vec4 topRight = uniqueRect->topRight;
		    glm::vec4 bottomRight = uniqueRect->bottomRight;
		    glm::vec4 bottomLeft = uniqueRect->bottomLeft;
		    glm::vec4 topLeft = uniqueRect->topLeft;

	        const float tw = float(1.0f/cols);
	        const float th = float(1.0f/rows);
	        const float tx = col * tw;
	        const float ty = row * th;

		    glm::vec2 topLeftTex =  glm::vec2(tx, ty);
		    glm::vec2 topRightTex =  glm::vec2(tx + tw, ty);
		    glm::vec2 bottomRightTex =  glm::vec2(tx + tw, ty + th);
		    glm::vec2 bottomLeftTex =  glm::vec2(tx, ty + th);


		    // set up vertex data (and buffer(s)) and configure vertex attributes
		    // ------------------------------------------------------------------

		    std::vector<float> particleVertices = {
		        // positions   // texture coords
		       topRight.x, topRight.y, topRight.z,           topRightTex.x, topRightTex.y, alpha, // top right
		       bottomRight.x, bottomRight.y, bottomRight.z,  bottomRightTex.x, bottomRightTex.y, alpha, // bottom right
		       bottomLeft.x, bottomLeft.y, bottomLeft.z,     bottomLeftTex.x, bottomLeftTex.y, alpha, // bottom left
		       topLeft.x, topLeft.y, topLeft.z,              topLeftTex.x, topLeftTex.y, alpha  // top left 

		    };

		    std::vector<unsigned int> particleIndices = {
		    	0 + j*4, 1 + j*4, 3 + j*4,
		    	1 + j*4, 2 + j*4, 3 + j*4
		    };

	        Rect2D::indices.insert( Rect2D::indices.end(), particleIndices.begin(), particleIndices.end() );
	        Rect2D::vertices.insert( Rect2D::vertices.end(), particleVertices.begin(), particleVertices.end() );
			}
		}


	    rectShader.setInt("texture1", i);

	    // render quad
	    glBindVertexArray(Rect2D::VAO);
	    glActiveTexture(GL_TEXTURE0 + i);
	    glBindTexture(GL_TEXTURE_2D, rectBatch.at(0)->texture);
	    glBindBuffer(GL_ARRAY_BUFFER, Rect2D::VBO);
	    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * Rect2D::vertices.size(), Rect2D::vertices.data(), GL_STATIC_DRAW);
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Rect2D::indices.size() * sizeof(unsigned int), Rect2D::indices.data(), GL_STATIC_DRAW);
	    glDrawElements(GL_TRIANGLES, Rect2D::indices.size(), GL_UNSIGNED_INT, 0);
	    glBindVertexArray(0);
	}

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

    return 0;
}

int Rect2D::remove() {
	unsigned int index = 0;
	for (unsigned int i = 0; i < Rect2D::rects.size(); i++) {
		if (Rect2D::rects.at(i).at(0)->filePath == filePath) {
			index = i;
		}
	}
    Rect2D::rects.at(index).erase(std::find(Rect2D::rects.at(index).begin(),Rect2D::rects.at(index).end(),this));
    return 0;
}