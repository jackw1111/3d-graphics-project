#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <iomanip>

#include "_engine.h"

class VideoPlayer {
public:
	VideoPlayer();
	void play(std::string imgDirectory, float currentFrame, int framerate);
};