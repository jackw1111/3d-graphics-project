#include "light.h"

std::vector<Light*> Light::lights = {};

Light::Light(glm::vec3 pos, glm::vec3 col) {
	position = pos;
	color = col;
	Light::lights.push_back(this);
}

