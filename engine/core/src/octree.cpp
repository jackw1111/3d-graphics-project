

#include "octree.h"

#define MAX_TRIANGLES 1000


bool isTriangleInCube(vec3 position, float hw, std::vector<vec3> triangles) {
	std::vector<vec3> points = {vec3(-hw,hw,hw) + position, 
								vec3(hw,hw,hw) + position, 
								vec3(hw,hw,-hw) + position, 
								vec3(-hw,hw,-hw) + position, 

								vec3(-hw,-hw,hw) + position, 
								vec3(hw,-hw,hw) + position, 
								vec3(hw,-hw,-hw) + position, 
								vec3(-hw,-hw,-hw) + position};
	float xMin;
	float yMin;
	float zMin;
	float xMax;
	float yMax;
	float zMax;
	xMin = yMin = zMin = FLT_MIN;
	xMax = yMax = zMax = FLT_MAX;

	for (unsigned int i = 0; i < points.size(); i++) {
		vec3 point = points.at(i);
		if (xMin == FLT_MIN){
					xMin = point.x;}
		if (yMin == FLT_MIN){
					yMin = point.y;}
		if (zMin == FLT_MIN){
					zMin = point.z;}
		if (xMax == FLT_MAX){
					xMax = point.x;}
		if (yMax == FLT_MAX){
					yMax = point.y;}
		if (zMax == FLT_MAX){
					zMax = point.z;}

		if  (point.x < xMin){
					xMin = point.x;}
		if  (point.y < yMin){
					yMin = point.y;}
		if  (point.z < zMin){
					zMin = point.z;}
		if  (point.x > xMax){
					xMax = point.x;}
		if  (point.y > yMax){
					yMax = point.y;}
		if  (point.z > zMax){
					zMax = point.z;}
	}

	// check if at least one vertex is in bounds of cube
	for (unsigned int v = 0; v < triangles.size(); v++) {
		vec3 vertex = triangles.at(v);
		if (vertex.x >= xMin && vertex.x < xMax &&
			vertex.y >= yMin && vertex.y < yMax &&
			vertex.z >= zMin && vertex.z < zMax) {
			return true;
		}
	}
	return false;
}

Octree::Octree(int ind, vec3 pos, float hwidth, std::vector<std::vector<vec3>> tris) {
	setup(ind, pos, hwidth, tris);
}

void Octree::setup(int ind, vec3 pos, float hwidth, std::vector<std::vector<vec3>> tris) {
	this->index = ind;
	this->position = pos;
	this->hw = hwidth;
	this->triangles = tris;
	this->points = {vec3(-this->hw,this->hw,this->hw) + this->position, 
				   vec3(this->hw,this->hw,this->hw) + this->position, 
				   vec3(this->hw,this->hw,-this->hw) + this->position, 
				   vec3(-this->hw,this->hw,-this->hw) + this->position, 

				   vec3(-this->hw,-this->hw,this->hw) + this->position, 
				   vec3(this->hw,-this->hw,this->hw) + this->position, 
				   vec3(this->hw,-this->hw,-this->hw) + this->position, 
				   vec3(-this->hw,-this->hw,-this->hw) + this->position};
	this->children = {};
	if (this->triangles.size() > MAX_TRIANGLES) {
		this->createChildren();
		this->triangles = {};
	}
}

void Octree::createChildren() {
	std::vector<vec3> allPositions =  {vec3(-this->hw/2, this->hw/2, this->hw/2) + position,
					 vec3( this->hw/2, this->hw/2, this->hw/2) + position,
					 vec3( this->hw/2, this->hw/2,-this->hw/2) + position,
					 vec3(-this->hw/2, this->hw/2,-this->hw/2) + position,
					 vec3(-this->hw/2,-this->hw/2, this->hw/2) + position,
					 vec3( this->hw/2,-this->hw/2, this->hw/2) + position,
					 vec3( this->hw/2,-this->hw/2,-this->hw/2) + position,
					 vec3(-this->hw/2,-this->hw/2,-this->hw/2) + position};

	// split triangles into nodes

	for (unsigned int i = 0; i < allPositions.size(); i++) {
		vec3 position = allPositions.at(i);

		std::vector<std::vector<vec3>> node_triangles = {};
		for (unsigned int j = 0; j < triangles.size(); j++){
			std::vector<vec3> tri = triangles.at(j);
			if (isTriangleInCube(position, this->hw/2, tri)){
				node_triangles.push_back(tri);
			}
		}
		if (node_triangles.size() > 0) {
			this->children.push_back(Octree(0, position, this->hw/2, node_triangles));
		}
	}
}

// get the triangles to collide with for a particular point, pos.
std::vector<std::vector<glm::vec3>> Octree::getTriangleSet(vec3 pos) {
	// if node has no children, finished, return triangles
	Octree *currentCube = this;
	int counter = 0;
	while (true) {
		counter++;
		if (currentCube->children.size() == 0) {
			return (*currentCube).triangles;
		} else {
			// 	// keep refining the bounding cube until no more children to search
			for (unsigned int i = 0; i < currentCube->children.size(); i++) {
				Octree *n = &currentCube->children.at(i);
				if (isTriangleInCube(n->position, n->hw, {pos})){
					currentCube = n;
					break;
				}
			}
		}
		if (counter > 20) {
			return (*currentCube).triangles;
		}
	}
}