#pragma once

#include "Geometry/Sphere.h"
#include "Geometry/Box.h"

class CollisionTest
{
private:

public:
	CollisionTest();
	~CollisionTest();

	static bool euclDistanceTest();
	static bool sphereOnBoxTest(Sphere sphere, Box box)
	{
		float radius = sphere.getRadius();

		// Get half extents of box (width/2, height/2)
		glm::vec2 halfExtents = glm::vec2(box.getCentre().x - box.getMin().x, box.getCentre().y - box.getMin().y);

		// Get distance vector from centres of Sphere and Box
		glm::vec3 distance = sphere.getCentre() - box.getCentre();

		distance.x -= (radius - halfExtents.x);
		distance.y -= (radius - halfExtents.y);

		// Square distance vector for positive values (explicitly for efficiency)
		distance.x *= distance.x;
		distance.y *= distance.y;

		if (distance.x < 1 || distance.y < 1)
			return true;

	}
};

