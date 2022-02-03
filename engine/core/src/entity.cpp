#include "entity.h"


#include <vector>
#include <limits>
#include <chrono>

using glm::vec3;

CharacterEntity::CharacterEntity() {
	initialized = false;
}


CharacterEntity::CharacterEntity(std::vector<StaticModel> models, std::vector<mat4> _modelTransforms, glm::vec3 radius)
{
	setup(models, _modelTransforms, radius);
}

CharacterEntity::CharacterEntity(const CharacterEntity &entity)
{
	position = entity.position;
	velocity = entity.velocity;
	radius = entity.radius;
	collisionPackage = entity.collisionPackage;
	modelTransforms = entity.modelTransforms;
	models = entity.models;
	initialized = true;
}
glm::vec3 CharacterEntity::getPosition() {
 assert(initialized);
  return position;
}
int CharacterEntity::setPosition(glm::vec3 pos) {
 assert(initialized);
  position = pos;
  return 1;
}

glm::vec3 CharacterEntity::getVelocity() {
  assert(initialized);
  return velocity;
}
int CharacterEntity::setVelocity(glm::vec3 vel) {
	assert(initialized);
	velocity = vel;
	return 1;
}

int CharacterEntity::addModel(std::vector<StaticModel> models, std::vector<mat4> _modelTransforms) {
	 this->models.insert(this->models.end(), models.begin(), models.end());
	 this->modelTransforms.insert(this->modelTransforms.end(), _modelTransforms.begin(), _modelTransforms.end());

	 for (unsigned int j = 0; j < this->modelTransforms.size(); j++) {
	  	for (unsigned int n = 0; n < this->models.size(); n++) {
		  	for (unsigned int i = 0; i < this->models[n].meshes.size(); i++) {

				std::vector<std::vector<vec3>> t = this->models[0].meshes[i].getTriangles(mat4(1.0));
				for (unsigned int k = 0; k < t.size(); k++) {
					glm::vec4 a(t[k][0], 1.0f);
					glm::vec4 b(t[k][1], 1.0f);
					glm::vec4 c(t[k][2], 1.0f);
					a = this->modelTransforms[j] * a;
					b = this->modelTransforms[j] * b;
					c = this->modelTransforms[j] * c;
					triangles.push_back({vec3(a), vec3(b), vec3(c)});
				}
			}
		}

	  }

	this->closestRegion = NULL;
	this->cube.setup(0, this->position, 10000000.0, triangles);
	initialized = true;
	return 1;
}

int CharacterEntity::addAnimatedObject(AnimatedObject aObject) {
	animated_triangles = {};
	 for (unsigned int j = 0; j < 1; j++) {
	  	for (unsigned int n = 0; n < 1; n++) {
		  	for (unsigned int i = 0; i < aObject.model.get()->meshes.size(); i++) {

				std::vector<std::vector<vec3>> t = aObject.getAnimatedVertices();
				std::cout << t.size() << std::endl;
				if (t.size() != 0){

					for (unsigned int k = 0; k < t.size(); k++) {
						for (unsigned int v = 0; v < t.at(k).size(); v+=3) {

							glm::vec4 a(t.at(k).at(v+0), 1.0f);
							glm::vec4 b(t.at(k).at(v+1), 1.0f);
							glm::vec4 c(t.at(k).at(v+2), 1.0f);
							a = aObject.getModelMatrix() * a;
							b = aObject.getModelMatrix() * b;
							c = aObject.getModelMatrix() * c;
							animated_triangles.push_back({vec3(a), vec3(b), vec3(c)});
						}
					}					
				}

			}
		}

	  }

	this->closestRegion = NULL;
	std::vector< std::vector< vec3 >> all_tris = animated_triangles;
	all_tris.insert(all_tris.end(), triangles.begin(), triangles.end());
	this->cube.setup(0, this->position, 10000000.0, all_tris);
	initialized = true;
	return 1;
}

int CharacterEntity::setup(std::vector<StaticModel> models, std::vector<mat4> _modelTransforms, glm::vec3 radius)
{
  this->radius = radius;
  this->position = glm::vec3(0.0, 0.0, 0.0);
  this->velocity = glm::vec3(0.0, 0.0, 0.0);

  grounded = 0;

  line = Line3D(glm::vec3(0,0,0), glm::vec3(0,0,0));
  //std::cout << models[0].meshes.size() << std::endl;

 	//this->addModel(this->models, this->modelTransforms);

	return 0;
}

void CharacterEntity::collideAndSlide(const glm::vec3& gravity)
{
	assert(initialized);
  // Do collision detection:
	collisionPackage.R3Position = position;
	collisionPackage.R3Velocity = velocity;

	collisionPackage.eRadius = radius;

	// calculate position and velocity in eSpace
	glm::vec3 eSpacePosition = collisionPackage.R3Position/
	collisionPackage.eRadius;
	glm::vec3 velocity = collisionPackage.R3Velocity/
	collisionPackage.eRadius;
	// no gravity
    //velocity[1] = 0.0f;

	// Iterate until we have our final position.
	collisionPackage.collisionRecursionDepth = 0;

	int g = grounded;
	glm::vec3 finalPosition;
	//this->collideWithWorld(finalPosition, eSpacePosition, velocity);
	finalPosition = sphereSweep(eSpacePosition, velocity);
	grounded = g;

	// Add gravity pull:
	// To remove gravity uncomment from here .....

	// Set the new R3 position (convert back from eSpace to R3)
	//collisionPackage.R3Position = finalPosition * collisionPackage.eRadius;
	//collisionPackage.R3Velocity = gravity;

    // convert velocity to e-space
	velocity = gravity / collisionPackage.eRadius;
	// gravity iteration
	collisionPackage.collisionRecursionDepth = 0;
	finalPosition = sphereSweep(finalPosition, velocity);
	//collideWithWorld(finalPosition, finalPosition, velocity);

	// ... to here
	// finally set entity position
	position = finalPosition * collisionPackage.eRadius;

}

glm::vec3 CharacterEntity::sphereSweep(glm::vec3 pos, glm::vec3 vel) {
	assert(initialized);

	float unitScale = unitsPerMeter / 100.0f;
	float veryCloseDistance = 0.005f * unitScale;

	glm::vec3 dest = pos + vel;
	Plane firstPlane;

	for (unsigned int i = 0; i < 3; ++i) {

		collisionPackage.velocity = vel;
	    collisionPackage.normalizedVelocity = glm::normalize(vel);
		collisionPackage.basePoint = pos;
		collisionPackage.foundCollision = false;
	    collisionPackage.nearestDistance = FLT_MAX;

		checkCollision();
		if (collisionPackage.foundCollision == false) {
	  		return dest;
		}
		//glm::vec3 touchPoint = pos + vel * collisionPackage.t;
		float dist = glm::length(vel) * collisionPackage.t;
		float shortDist = std::max(dist - veryCloseDistance, 0.0f);
		pos += glm::normalize(vel) * shortDist;
		glm::vec3 nearPoint = pos + vel * shortDist;


		glm::vec3 slidePlaneOrigin =
				collisionPackage.intersectionPoint;
		slidePlaneNormal =
				nearPoint-collisionPackage.intersectionPoint;
		slidePlaneNormal = glm::normalize(slidePlaneNormal);
		Plane slidingPlane(slidePlaneOrigin, slidePlaneNormal);

		if (i == 0) {
			float longRadius = 1.0f + veryCloseDistance;
			firstPlane = slidingPlane;
			dest -= (float)(firstPlane.signedDistanceTo(dest) - longRadius) * firstPlane.normal;
			vel = dest - pos;
		} else if (i == 1) {
			Plane secondPlane = slidingPlane;
			glm::vec3 crease = glm::normalize(glm::cross(firstPlane.normal, secondPlane.normal));
			float dis = glm::dot(dest - pos, crease);
			vel = dis * crease;
			dest = pos + vel;
		}

	}

	return pos;
}

void CharacterEntity::collideWithWorld(glm::vec3& out, const glm::vec3& pos, const glm::vec3& vel)
{
	assert(initialized);
	// All hard-coded distances in this function is
	// scaled to fit the setting above..
	float unitScale = unitsPerMeter / 100.0f;
	float veryCloseDistance = 0.005f * unitScale;

	// do we need to worry?
	if (collisionPackage.collisionRecursionDepth > 5)
		return;

	// Ok, we need to worry:
	collisionPackage.velocity = vel;
    collisionPackage.normalizedVelocity = glm::normalize(vel);
	collisionPackage.basePoint = pos;
	collisionPackage.foundCollision = false;
    collisionPackage.nearestDistance = FLT_MAX;

	// Check for collision (calls the collision routines)
	// Application specific!!
    checkCollision();

	// If no collision we just move along the velocity
	if (collisionPackage.foundCollision == false) {
    out = pos + vel;
	  return;
	}
	// *** Collision occured ***
	// The original destination point
	glm::vec3 destinationPoint = pos + vel;
	glm::vec3 newBasePoint = pos;
	// only update if we are not already very close
	// and if so we only move very close to intersection..not
	// to the exact spot.

	if (collisionPackage.nearestDistance >= veryCloseDistance)
	{

		glm::vec3 v = (float)MIN(length(vel),  collisionPackage.nearestDistance - veryCloseDistance) * vel;
		newBasePoint = collisionPackage.basePoint + v;

		// Adjust polygon intersection point (so sliding
		// Plane will be unaffected by the fact that we
		// move slightly less than collision tells us)

		v = glm::normalize(v);

		collisionPackage.intersectionPoint -= v * veryCloseDistance;

	}

	// Determine the sliding Plane
	glm::vec3 slidePlaneOrigin =
			collisionPackage.intersectionPoint;
	slidePlaneNormal =
			newBasePoint-collisionPackage.intersectionPoint;
	slidePlaneNormal = glm::normalize(slidePlaneNormal);
	//line.setEndpoints(slidePlaneOrigin, slidePlaneOrigin + slidePlaneNormal);
	//line.color = vec3(0,1,0);
	//std::cout << to_string(slidePlaneOrigin) << ", " << to_string(slidePlaneNormal) << std::endl;

	Plane slidingPlane(slidePlaneOrigin, slidePlaneNormal);

	// Again, sorry about formatting.. but look carefully ;)
	glm::vec3 newDestinationPoint = destinationPoint -
	(float)slidingPlane.signedDistanceTo(destinationPoint)*
	slidePlaneNormal;

	// Generate the slide vectpr, which will become our new
	// velocity vector for the next iteration
	glm::vec3 newVelocityVector = newDestinationPoint -
						collisionPackage.intersectionPoint;

	out = newBasePoint;

	// if (collisionPackage.intersectionPoint[1] <= pos[1]-collisionPackage.eRadius[1] && vel[1] <= 0.0f)
	// 	grounded = 1;

	if (collisionPackage.intersectionPoint[1] <= pos[1]-collisionPackage.eRadius[1]+2.4f) {
		grounded = 1;
	}
	if (abs(dot(slidePlaneNormal, vec3(0,1,0)) < 0.5f)) {
		grounded = 0;
	}

	if (collisionPackage.foundCollision && !grounded) {
		sliding = 1;
	} else {
		sliding = 0;
	}



	// Recurse:

	// dont recurse if the new velocity is very small
	if (length(newVelocityVector) < veryCloseDistance) {
		return;
	}

    collisionPackage.collisionRecursionDepth++;
    collideWithWorld(out, out, newVelocityVector);
}

int CharacterEntity::checkCollision()
{		

	// TO DO Capsule collision
	//assert(initialized);
	std::vector<std::vector<vec3>> triangles = this->cube.getTriangleSet(this->position);

	if (triangles.size() == 0) {
		return 0;
	}
	for (unsigned int i = 0; i < triangles.size(); i++) {
		vec3 v1 = triangles.at(i).at(0);
		vec3 v2 = triangles.at(i).at(1);
		vec3 v3 = triangles.at(i).at(2);

		vec3 a = v1 / this->collisionPackage.eRadius;
		vec3 b = v2 / this->collisionPackage.eRadius;
		vec3 c = v3 / this->collisionPackage.eRadius;
		checkTriangle(&this->collisionPackage, a, b, c);
	}
	return 0;
}

int CharacterEntity::update()
{
  assert(initialized);
  this->grounded = 0;
  this->sliding = 0;

  this->collideAndSlide(this->gravity);

  return 0;
}

void CharacterEntity::remove() {
    delete this;
}

