#ifndef _LIGHT_H
#define _LIGHT_H

#include "math_utils.h"

#include <glm/glm.hpp>
#include <vector>

class Light {
public:
	glm::vec3 position;
	glm::vec3 color;

	Light(glm::vec3 position, glm::vec3 color);
	static std::vector<Light*> lights;
};



#endif