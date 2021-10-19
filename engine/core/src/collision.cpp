#include "collision.h"
#include "entity.h"

Plane::Plane(const glm::vec3& origin, const glm::vec3& normal)
{
  this->origin = origin;
  this->normal = normal;
  equation[0] = normal[0];
  equation[1] = normal[1];
  equation[2] = normal[2];
  equation[3] = -glm::dot(origin, normal);
}

// Construct from triangle:
Plane::Plane(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
{
  normal = glm::cross(p2 - p1, p3 - p1);
  normal = normalize(normal);

  origin = p1;

  equation[0] = normal[0];
  equation[1] = normal[1];
  equation[2] = normal[2];
  equation[3] = -glm::dot(normal, origin);
}

double Plane::signedDistanceTo(const glm::vec3& point) const {

	return (glm::dot(point, normal)) + equation[3];
}

bool Plane::isFrontFacingTo(const glm::vec3& direction) const
{
	double d = glm::dot(normal, direction);
	return (d <= 0);
}

bool checkPointInTriangle(const glm::vec3& point,
                          const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2)
{
  // glm::vec3 u, v, w, vw, vu, uw, uv;

  //  u = p2 - p1;
  //  v = p3 - p1;
  //  w = point - p1;

  // vw = glm::cross(v, w);
  // vu = glm::cross(v, u);

  // if (glm::dot(vw, vu) < 0.0f) {
  //   return 0;
  // }

  // uw = glm::cross(u, w);
  // uv = glm::cross(u, v);

  // if (glm::dot(uw, uv) < 0.0f) {
  //   return 0;
  // }

  // float d = length(uv);
  // float r = length(vw) / d;
  // float t = length(uw) / d;

  // return ((r + t) <= 1.0f);

	  glm::vec3 u = v1 - v0;
	  glm::vec3 v = v2 - v0;
	  glm::vec3 N = glm::normalize(glm::cross(u, v));

	glm::vec3 edge0 = v1 - v0; 
	glm::vec3  edge1 = v2 - v1; 
	glm::vec3  edge2 = v0 - v2; 
	glm::vec3  C0 = point - v0; 
	glm::vec3  C1 = point - v1; 
	glm::vec3  C2 = point - v2; 
	if (glm::dot(N, glm::cross(edge0, C0)) > 0 && 
	    glm::dot(N, glm::cross(edge1, C1)) > 0 && 
	    glm::dot(N, glm::cross(edge2, C2)) > 0) return true; 

	return false;

}

glm::vec3 closestPointOnLineSegment(glm::vec3 A, glm::vec3 B, glm::vec3 Point)
{
  glm::vec3 AB = B - A;
  float t = glm::dot(Point - A, AB) / glm::dot(AB, AB);
  return A + std::min(std::max(t, 0.0f), 1.0f) * AB; 
}
// https://wickedengine.net/2020/04/26/capsule-collision-detection/
glm::vec3 closestPointOnTriangle(glm::vec3 externalPoint, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2)
{
	if (checkPointInTriangle(externalPoint, p0, p1, p2)) {
		return externalPoint;
	} else {
		  glm::vec3 reference_point;
		  // Edge 1:
		  glm::vec3 point1 = closestPointOnLineSegment(p0, p1, externalPoint);
		  glm::vec3 v1 = externalPoint - point1;
		  float distsq = dot(v1, v1);
		  float best_dist = distsq;
		  reference_point = point1;
		 
		  // Edge 2:
		  glm::vec3 point2 = closestPointOnLineSegment(p1, p2, externalPoint);
		  glm::vec3 v2 = externalPoint - point2;
		  distsq = dot(v2, v2);
		  if(distsq < best_dist)
		  {
		    reference_point = point2;
		    best_dist = distsq;
		  }
		 
		  // Edge 3:
		  glm::vec3 point3 = closestPointOnLineSegment(p2, p0, externalPoint);
		  glm::vec3 v3 = externalPoint - point3;
		  distsq = dot(v3, v3);
		  if(distsq < best_dist)
		  {
		    reference_point = point3;
		    best_dist = distsq;
		  }
	  	return reference_point;
	}
}

bool rayIntersectPlane(const glm::vec3 &n, const glm::vec3 &p0, const glm::vec3 &l0, const glm::vec3 &l, float &t) 
{ 
    // assuming vectors are all normalized
    float denom = glm::dot(n, l); 
    if (denom > 1e-6) { 
        glm::vec3 p0l0 = p0 - l0; 
        t = glm::dot(p0l0, n) / denom; 
        return (t >= 0); 
    } 
 
    return false; 
} 

bool getLowestRoot(float a, float b, float c, float maxR,
				   float* root) {
	// Check if a solution exists
	float determinant = b*b - 4.0f*a*c;

	// If determinant is negative it means no solutions.
	if (determinant < 0.0f) return false;

	// calculate the two roots: (if determinant == 0 then
	// x1==x2 but let’s disregard that slight optimization)
	float sqrtD = sqrt(determinant);
	float r1 = (-b - sqrtD) / (2*a);
	float r2 = (-b + sqrtD) / (2*a);

	// Sort so x1 <= x2
	if (r1 > r2) {
		float temp = r2;
		r2 = r1;
		r1 = temp;
	}
	// Get lowest root:
	if (r1 > 0 && r1 < maxR) {
		*root = r1;
		return true;
	}
	// It is possible that we want x2 - this can happen
	// if x1 < 0
	if (r2 > 0 && r2 < maxR) {
		*root = r2;
		return true;
	}
	// No (valid) solutions
	return false;
}

// Assumes: p1,p2 and p3 are given in ellisoid space:
void checkTriangle(CollisionPacket* colPackage,
	 const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
{
	// Make the Plane containing this triangle.
	Plane trianglePlane(p1,p2,p3);
	// Is triangle front-facing to the velocity vector?
	// We only check front-facing triangles
	// (your choice of course)

	if (trianglePlane.isFrontFacingTo(
						colPackage->normalizedVelocity)) {
		// Get interval of Plane intersection:
		double t0, t1;
		bool embeddedInPlane = false;
		// Calculate the signed distance from sphere
		// position to triangle Plane
		double signedDistToTrianglePlane =
		trianglePlane.signedDistanceTo(colPackage->basePoint);

		// cache this as we’re going to use it a few times below:
		float normalDotVelocity =
			glm::dot(trianglePlane.normal, colPackage->velocity);
		// if sphere is travelling parrallel to the Plane:
		if (normalDotVelocity == 0.0f) {
			if (fabs(signedDistToTrianglePlane) >= 1.0f) {
				// Sphere is not embedded in Plane.
				// No collision possible:
				return;
			}
			else {
				// sphere is embedded in Plane.
				// It intersects in the whole range [0..1]
				embeddedInPlane = true;
				t0 = 0.0;
				t1 = 1.0;
			}
		}
		else {
			// N glm::dot D is not 0. Calculate intersection interval:
			t0=(-1.0-signedDistToTrianglePlane)/normalDotVelocity;
			t1=( 1.0-signedDistToTrianglePlane)/normalDotVelocity;

			// Swap so t0 < t1
			if (t0 > t1) {
				double temp = t1;
				t1 = t0;
				t0 = temp;
			}

			// Check that at least one result is within range:
			if (t0 > 1.0f || t1 < 0.0f) {
				// Both t values are outside values [0,1]
				// No collision possible:
				return;
			}
			// Clamp to [0,1]
			if (t0 < 0.0) t0 = 0.0;
			if (t1 < 0.0) t1 = 0.0;
			if (t0 > 1.0) t0 = 1.0;
			if (t1 > 1.0) t1 = 1.0;
		}

		// OK, at this point we have two time values t0 and t1
		// between which the swept sphere intersects with the
		// triangle Plane. If any collision is to occur it must
		// happen within this interval.
		glm::vec3 collisionPoint;
		bool foundCollison = false;
		float t = 1.0;
		colPackage->t = t;
		// First we check for the easy case - collision inside
		// the triangle. If this happens it must be at time t0
		// as this is when the sphere rests on the front side
		// of the triangle Plane. Note, this can only happen if
		// the sphere is not embedded in the triangle Plane.
		if (!embeddedInPlane) {
			glm::vec3 PlaneIntersectionPoint =
					(colPackage->basePoint-normalize(trianglePlane.normal))
					+ (float)t0*colPackage->velocity;
			if (checkPointInTriangle(PlaneIntersectionPoint,
									 p1,p2,p3))
			{
				foundCollison = true;
				t = t0;
				colPackage->t = t;
				float distToCollision = t*length(colPackage->velocity);
				collisionPoint = PlaneIntersectionPoint;
				colPackage->nearestDistance = distToCollision;
				colPackage->intersectionPoint=collisionPoint;
				colPackage->foundCollision = true;
			}
			//std::cout << "collide with inside of triangle" << std::endl;
		}
		// if we haven’t found a collision already we’ll have to
		// sweep sphere against points and edges of the triangle.
		// Note: A collision inside the triangle (the check above)
		// will always happen before a vertex or edge collision!
		// This is why we can skip the swept test if the above
		// gives a collision!

		if (foundCollison == false) {
			// some commonly used terms:
			glm::vec3 velocity = colPackage->velocity;
			glm::vec3 base = colPackage->basePoint;
			float velocitySquaredLength = glm::dot(velocity, velocity);
			float a,b,c; // Params for equation
			float newT;

			// For each vertex or edge a quadratic equation have to
			// be solved. We parameterize this equation as
			// a*t^2 + b*t + c = 0 and below we calculate the
			// parameters a,b and c for each test.

			// Check against points:
			a = velocitySquaredLength;

			// P1
			b = 2.0*(glm::dot(velocity, base-p1));
			c = glm::dot(p1-base, p1-base) - 1.0;
			if (getLowestRoot(a,b,c, t, &newT)) {
				t = newT;
				colPackage->t = t;
				foundCollison = true;
				collisionPoint = p1;
			}

			// P2
			b = 2.0*(glm::dot(velocity, base-p2));
			c = glm::dot(p2-base, p2-base) - 1.0;
			if (getLowestRoot(a,b,c, t, &newT)) {
				t = newT;
				colPackage->t = t;
				foundCollison = true;
				collisionPoint = p2;
			}

			// P3
			b = 2.0*(glm::dot(velocity, base-p3));
			c = glm::dot(p3-base, p3-base) - 1.0;
			if (getLowestRoot(a,b,c, t, &newT)) {
				t = newT;
				colPackage->t = t;
				foundCollison = true;
				collisionPoint = p3;
			}

			// Check agains edges:

			// p1 -> p2:
			glm::vec3 edge = p2-p1;
			glm::vec3 baseToVertex = p1 - base;
			float edgeSquaredLength = glm::dot(edge, edge);
			float edgeDotVelocity = glm::dot(edge, velocity);
			float edgeDotBaseToVertex = glm::dot(edge, baseToVertex);

			// Calculate parameters for equation
			a = edgeSquaredLength*-velocitySquaredLength +
			edgeDotVelocity*edgeDotVelocity;
			b = edgeSquaredLength*(2*glm::dot(velocity, baseToVertex))-
			2.0*edgeDotVelocity*edgeDotBaseToVertex;
			c = edgeSquaredLength*(1-glm::dot(baseToVertex, baseToVertex))+
			edgeDotBaseToVertex*edgeDotBaseToVertex;

			// Does the swept sphere collide against infinite edge?
			if (getLowestRoot(a,b,c, t, &newT)) {
				// Check if intersection is within line segment:
				float f=(edgeDotVelocity*newT-edgeDotBaseToVertex)/
				edgeSquaredLength;
				if (f >= 0.0 && f <= 1.0) {
					// intersection took place within segment.
					t = newT;
					colPackage->t = t;
					foundCollison = true;
					collisionPoint = p1 + f*edge;
				}
			}

			// p2 -> p3:
			edge = p3-p2;
			baseToVertex = p2 - base;
			edgeSquaredLength = glm::dot(edge, edge);
			edgeDotVelocity = glm::dot(edge, velocity);
			edgeDotBaseToVertex =glm::dot(edge, baseToVertex);

			a = edgeSquaredLength*-velocitySquaredLength +
			edgeDotVelocity*edgeDotVelocity;
			b = edgeSquaredLength*(2*glm::dot(velocity, baseToVertex))-
			2.0*edgeDotVelocity*edgeDotBaseToVertex;
			c = edgeSquaredLength*(1-glm::dot(baseToVertex, baseToVertex))+
			edgeDotBaseToVertex*edgeDotBaseToVertex;

			if (getLowestRoot(a,b,c, t, &newT)) {
				float f=(edgeDotVelocity*newT-edgeDotBaseToVertex)/
				edgeSquaredLength;
				if (f >= 0.0 && f <= 1.0) {
					t = newT;
					colPackage->t = t;
					foundCollison = true;
					collisionPoint = p2 + f*edge;
				}
			}

			// p3 -> p1:
			edge = p1-p3;
			baseToVertex = p3 - base;
			edgeSquaredLength = glm::dot(edge, edge);
			edgeDotVelocity = glm::dot(edge, velocity);

			edgeDotBaseToVertex = glm::dot(edge, baseToVertex);

			a = edgeSquaredLength*-velocitySquaredLength +
			edgeDotVelocity*edgeDotVelocity;
			b = edgeSquaredLength*(2*glm::dot(velocity, baseToVertex))-
			2.0*edgeDotVelocity*edgeDotBaseToVertex;
			c = edgeSquaredLength*(1-glm::dot(baseToVertex, baseToVertex))+
			edgeDotBaseToVertex*edgeDotBaseToVertex;

			if (getLowestRoot(a,b,c, t, &newT)) {
				float f=(edgeDotVelocity*newT-edgeDotBaseToVertex)/
				edgeSquaredLength;
				if (f >= 0.0 && f <= 1.0) {
					t = newT;
					colPackage->t = t;
					//std::cout << "got here" << std::endl;
					foundCollison = true;
					collisionPoint = p3 + f*edge;
				}
			}
		}

		// Set result:
		if (foundCollison == true) {
			// distance to collision: ’t’ is time of collision
			float distToCollision = t*length(colPackage->velocity);
			// Does this triangle qualify for the closest hit?
			// it does if it’s the first hit or the closest
			if (colPackage->foundCollision == false ||
			distToCollision < colPackage->nearestDistance) {
				// Collision information nessesary for sliding
				colPackage->nearestDistance = distToCollision;
				colPackage->intersectionPoint=collisionPoint;
				colPackage->foundCollision = true;
				colPackage->t = t;
			}
		}
	} // if not backface

}
