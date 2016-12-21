#pragma once

#include "Geometry/Sphere.h"
#include "Geometry/Box.h"
#include <math.h>
#include <algorithm>

class CollisionTest
{
private:

public:
	CollisionTest();
	~CollisionTest();

	static bool euclDistanceTest();

	static float square(float x) {
		return x * x;
	}

	static bool sphereOOB(Sphere sphere, float minx, float minz, float maxx, float maxz) {
		glm::vec3 centre = sphere.getCentre();
		return ((centre.x < minx) || (centre.x > maxx) || (centre.z < minz) || (centre.z > maxz));
	}

	static bool sphereAABB(Sphere sphere, glm::vec3 min, glm::vec3 max) {

		float dist_squared = square(sphere.getRadius());
		
		if (sphere.getCentre().x < min.x) dist_squared -= square(sphere.getCentre().x - min.x);
		else if (sphere.getCentre().x > max.x) dist_squared -= square(sphere.getCentre().x - max.x);
		if (sphere.getCentre().y < min.y) dist_squared -= square(sphere.getCentre().y - min.y);
		else if (sphere.getCentre().y > max.y) dist_squared -= square(sphere.getCentre().y - max.y);
		if (sphere.getCentre().z < min.z) dist_squared -= square(sphere.getCentre().z - min.z);
		else if (sphere.getCentre().z > max.z) dist_squared -= square(sphere.getCentre().z - max.z);

		return dist_squared > 0;
	}

	static bool sphereOnBoxTest(Sphere sphere, Box* box)
	{
		float radius = sphere.getRadius();

		// Get half extents of box (width/2, height/2)
		glm::vec2 halfExtents = glm::vec2(box->getCentre().x - box->getMin().x, box->getCentre().y - box->getMin().y);

		// Get distance vector from centres of Sphere and Box
		glm::vec3 distance = sphere.getCentre() - box->getCentre();

		distance.x -= (radius - halfExtents.x);
		distance.y -= (radius - halfExtents.y);

		// Square distance vector for positive values (explicitly for efficiency)
		distance.x *= distance.x;
		distance.y *= distance.y;

		if (distance.x < 1 || distance.y < 1)
			return true;
		else
			return false;
	}

	static bool sphereOnBoxTest(Sphere sphere, glm::vec3 min, glm::vec3 max)
	{
		float radius = sphere.getRadius();

		// Calculate centre point from min-max values
		glm::vec3 centre = glm::vec3((min.x + max.x) / 2, (min.y + max.y) / 2, (min.z + max.z) / 2);

		// Get distance vector from centres of Sphere and min-max
		glm::vec3 distance = sphere.getCentre() - centre;

		// Get half extents of box (width/2, height/2)
		glm::vec2 halfExtents = glm::vec2(centre.x - min.x, centre.y - min.y);

		distance.x -= (radius - halfExtents.x);
		distance.y -= (radius - halfExtents.y);

		// Square distance vector for positive values (explicitly for efficiency)
		distance.x *= distance.x;
		distance.y *= distance.y;

		if (distance.x < 1 || distance.y < 1)
			return true;
		else
			return false;
		
	}
};

