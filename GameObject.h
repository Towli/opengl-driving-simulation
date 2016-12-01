#pragma once

#include "shaders\Shader.h"
#include "3dStruct\threeDModel.h"
#include "glm\glm.hpp"
#include <math.h>

#define PI 3.1415926535897


class GameObject
{
private:
	ThreeDModel* model;
	Shader* shader;
	glm::vec3 position;
	float currentAngle;
	float turnSpeed;
	float speed;

public:
	GameObject(Shader* shader, ThreeDModel* model);
	GameObject();
	~GameObject();

	/* Rendering */
	void draw();

	/* ThreeDModel */
	ThreeDModel* getModel();

	/* Position & direction handling*/
	glm::vec3 getPosition();
	void setPosition(glm::vec3 position);
	void turn(int direction);
	float getDirection();

	/* Movement */
	void move();
	void setSpeed(float speed);
	float getSpeed();
};

