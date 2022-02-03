#include "particle.h"
extern unsigned int WIDTH;
extern unsigned int HEIGHT;

std::vector<std::vector<Particle*>> Particle::particles {};
StaticShader Particle::rectShader;
unsigned int Particle::VBO;
unsigned int Particle::VAO;
unsigned int Particle::EBO;
vector<float> Particle::vertices;
vector<unsigned int> Particle::indices; 
bool Particle::setup = false;
mat4 Particle::viewProjection;
std::map<std::string, unsigned int> Particle::textureMap;

Particle::Particle(std::string _filePath, glm::mat4 _modelMatrix, unsigned int _cols, unsigned int _rows) {

    modelMatrix = _modelMatrix;
    filePath = _filePath;
    cols = _cols;
    rows = _rows;

    if (!Particle::setup) {
	    glGenVertexArrays(1, &Particle::VAO);
	    glGenBuffers(1, &Particle::VBO);
	    glGenBuffers(1, &Particle::EBO);


	    // set up vertex data (and buffer(s)) and configure vertex attributes
	    // ------------------------------------------------------------------
	    Particle::vertices = {
	        // positions             // texture coords
	        0.5f,  0.5f, 0.0f, 1.0f, 1.0f, // top right
	        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
	       -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
	       -0.5f,  0.5f, 0.0f, 0.0f, 1.0f  // top left 
	    };


	    Particle::indices = {  
	        0, 1, 3, // first triangle
	        1, 2, 3  // second triangle
	    };

	    // setup geometry storage
	    glBindVertexArray(Particle::VAO);

	    glBindBuffer(GL_ARRAY_BUFFER, Particle::VBO);
	    glBufferData(GL_ARRAY_BUFFER,  Particle::vertices.size() * sizeof(float), Particle::vertices.data(), GL_STATIC_DRAW);

	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Particle::indices.size() * sizeof(unsigned int), Particle::indices.data(), GL_STATIC_DRAW);

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
	    rectShader.setup("../../shaders/particle_shader.vs",
	                     "../../shaders/particle_shader.fs"); 

	    Particle::setup = true;
	}

    bool found = false;

	std::map<std::string, unsigned int>::iterator it;

	for (it = Particle::textureMap.begin(); it != Particle::textureMap.end(); it++) {
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

	    glActiveTexture(GL_TEXTURE0 + Particle::textureMap.size());

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
	    Particle::textureMap.insert(std::make_pair(filePath, texture));
    }
    found = false;
	for (unsigned int i = 0; i < Particle::particles.size(); i++) {
		if (Particle::particles.at(i).at(0)->filePath == filePath) {
			Particle::particles.at(i).push_back(this);
			found = true;
			break;
		}
	}
	if (!found) {
    	Particle::particles.push_back({this});
	}

}

int Particle::getFrame() {
	return frame;
}

void Particle::setFrame(int f) {
	frame = f;
	row = (int)(frame / cols);
    col = frame - cols * rows;
}


int Particle::setCamera(glm::mat4 proj_view) {
	rectShader.use();
	rectShader.setMat4("viewProjection", proj_view);
	return 1;
}


Particle::~Particle() {
	std::cout << "deleting particle..." << std::endl;
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    //glDeleteVertexArrays(1, &VAO);
    //glDeleteBuffers(1, &VBO);
    //glDeleteBuffers(1, &EBO);

}

int Particle::drawAllParticles(Camera active_camera, int order) {

    glDisable(GL_DEPTH_TEST);
    //glDisable(GL_CULL_FACE);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    //glDisable(GL_BLEND);

    // std::sort( Particle::particles.begin( ), Particle::particles.end( ), [ ]( Particle *lhs, Particle *rhs )
    // {
    //    return lhs->ordering < rhs->ordering;
    // });

    mat4 rotatedCamera = rotate(glm::mat4(1.0f), glm::radians(90.0f-active_camera.Yaw), vec3(0,1,0));
   	rotatedCamera = rotate(rotatedCamera, glm::radians(-active_camera.Pitch), vec3(1,0,0));


	for (unsigned int i = 0; i < Particle::particles.size(); i++) {


	    Particle::vertices = {};
	    Particle::indices = {};

	    rectShader.use();

	    glBlendFunc(GL_SRC_ALPHA, Particle::particles.at(i).at(0)->blendMode);

		for (unsigned int j = 0; j < Particle::particles.at(i).size(); j++) {


			if (Particle::particles.at(i).at(j)->order != order) {
				continue;
			}

			glm::mat4 modelMatrix = Particle::particles.at(i).at(j)->modelMatrix;
			unsigned int cols = Particle::particles.at(i).at(j)->cols;
			unsigned int rows = Particle::particles.at(i).at(j)->rows;

			unsigned int col = (unsigned int)Particle::particles.at(i).at(j)->col;
			unsigned int row = (unsigned int)Particle::particles.at(i).at(j)->row;

			float alpha = Particle::particles.at(i).at(j)->alpha;

		    glm::vec4 topRight = modelMatrix * rotatedCamera * glm::vec4(0.5f,  0.5f, 0.0f, 1.0f);
		    glm::vec4 bottomRight = modelMatrix * rotatedCamera * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);
		    glm::vec4 bottomLeft = modelMatrix * rotatedCamera * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
		    glm::vec4 topLeft = modelMatrix * rotatedCamera * glm::vec4(-0.5f,  0.5f, 0.0f, 1.0f);

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

	        Particle::indices.insert( Particle::indices.end(), particleIndices.begin(), particleIndices.end() );
	        Particle::vertices.insert( Particle::vertices.end(), particleVertices.begin(), particleVertices.end() );

		}


	    rectShader.setInt("texture1", i);

	    // render quad
	    glBindVertexArray(Particle::VAO);
	    glActiveTexture(GL_TEXTURE0 + i);
	    glBindTexture(GL_TEXTURE_2D, Particle::particles.at(i).at(0)->texture);
	    glBindBuffer(GL_ARRAY_BUFFER, Particle::VBO);
	    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * Particle::vertices.size(), Particle::vertices.data(), GL_STATIC_DRAW);
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Particle::indices.size() * sizeof(unsigned int), Particle::indices.data(), GL_STATIC_DRAW);
	    glDrawElements(GL_TRIANGLES, Particle::indices.size(), GL_UNSIGNED_INT, 0);
	    glBindVertexArray(0);
	}

    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

    return 0;
}

int Particle::remove() {
    //Particle::particles.erase(std::find(Particle::particles.begin(),Particle::particles.end(),this));
    return 0;
}