#ifndef _FBO_H
#define _FBO_H

#include "static_model.h"

/*! @brief TODO
*/
class FBO {
public:
  unsigned int framebuffer;
  int WIDTH;
  int HEIGHT;
  unsigned int depth_texture;
  unsigned int position_texture;
  unsigned int color_texture;
  unsigned int normal_texture;

  bool hasColorBuffer = false;
  bool hasPositionBuffer = false;
  bool hasNormalBuffer = false;
  bool hasDepthBuffer = false;

  unsigned int quadVAO, quadVBO;
  FBO();
	FBO(int width, int height, bool position_buffer, bool normal_buffer,  bool color_buffer, bool depth_buffer);
	FBO(const char *path, const string &directory);
  int setup(int width, int height, bool position_buffer, bool normal_buffer,  bool color_buffer, bool depth_buffer);
  
  void handleError(int error);
	~FBO();
	void bind(bool value);
	void Draw(StaticShader shader);

  void createNormalBuffer();
  void createDepthBuffer();
  void createPositionBuffer();
  void createColorBuffer();

  unsigned int getColorBuffer();
  unsigned int getDepthBuffer();
  unsigned int getNormalBuffer();
  unsigned int getPositionBuffer();
};



#endif