#pragma once

#include "shaders\Shader.h"
#include "3dStruct\threeDModel.h"
#include "glm\glm.hpp"
#include <math.h>

class GameObject
{
private:
	ThreeDModel model;
	Shader* shader;
	glm::vec3 position;
	float currentAngle;
	float turnSpeed;
	float speed;

public:
	GameObject(Shader* shader, ThreeDModel model);
	GameObject();
	~GameObject();

	/* Rendering */
	void draw();

	/* Positioning */
	glm::vec3 getPosition();
	void setPosition(glm::vec3 position);
};

