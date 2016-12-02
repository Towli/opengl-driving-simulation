#pragma once

#include "shaders\Shader.h"
#include "3dStruct\threeDModel.h"
#include "glm\glm.hpp"
#include <math.h>

#define PI 3.1415926535897


class GameObject
{
private:
	vector<ThreeDModel*> models;
	ThreeDModel* model;
	Shader* shader;
	glm::vec3 position;
	float currentAngle;
	float turnSpeed;
	float speed;

public:
	GameObject(Shader* shader, ThreeDModel* model);
	GameObject(Shader* shader, vector<ThreeDModel*> models);
	GameObject();
	~GameObject();

	/* Rendering */
	void draw();

	/* ThreeDModel */
	ThreeDModel* getModel();
	vector<ThreeDModel*> getModels();

	/* Position & direction handling*/
	glm::vec3 getPosition();
	void setPosition(glm::vec3 position);
	void turn(int direction);
	float getDirection();

	/* Movement */
	void move(double deltaTime);
	void setSpeed(float speed);
	float getSpeed();
};

