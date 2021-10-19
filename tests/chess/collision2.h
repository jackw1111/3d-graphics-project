#ifndef _COLLISIONS2_H
#define _COLLISIONS2_H

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include "octree.h"
#include "static_model.h"

using glm::to_string;

glm::vec3 getPosition(glm::mat4 mat);

float spherePlaneCollision(glm::vec3 sphereCentre, float sphereRadius, glm::vec3 planeNormal, glm::vec3 pointOnPlane);

glm::vec3 getWorldSpace(glm::vec3 position, glm::mat4 mat);


bool rayTriangleCollision(glm::vec3 planeIntersection, glm::vec3 planeNormal, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);
glm::vec3 rayPlaneIntersection(glm::vec3 ray_position, glm::vec3 ray_direction, glm::vec3 plane_normal, glm::vec3 plane_position);
glm::vec3 closestPointOnLineSegment2(glm::vec3 A, glm::vec3 B, glm::vec3 point);
glm::vec3 sphereTriangleCollision(glm::vec3 sphereCentre, float sphereRadius, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2);


class CharacterEntity2 {
public:
	glm::vec3 displacement_vec;
	glm::vec3 velocity;
	glm::vec3 position;
	float sphere_radius;
	glm::vec3 top;
	glm::vec3 bottom;
	glm::mat4 object_model_mat;
	StaticModel scene_object;
	float movementSpeed;
	float gravitySpeed;

	Octree *closestRegion;
	Octree *cube;
	std::vector<std::vector<glm::vec3>> triangles = {};

	CharacterEntity2(StaticModel scene_object, glm::mat4 object_model_mat, glm::vec3 top, glm::vec3 bottom, float sphere_radius);
	bool capsuleTriangleCollision(glm::vec3 tip, glm::vec3 base, float sphere_radius,glm::vec3 p0, glm::vec3 p1, glm::vec3 p2);
	bool checkCollision2();
	int update(float deltaTime);

};



#endif