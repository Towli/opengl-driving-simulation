#pragma once

#include "glm/glm.hpp"
#include "shaders/Shader.h"


class Car
{
private:
	const float RUN_SPEED = 5.0f;
	const float TURN_SPEED = 0.5f;
	float currentRunSpeed;
	float currentTurnSpeed;
	glm::vec3 rotation;
	glm::vec3 position;
public:
	Car();
	Car(Shader* basicShader);	//* this should also take a ThreeDModel
	~Car();

	// Directions getters & setters
	glm::vec3 getPosition();
	glm::vec3 getRotation();
	void setRotation(glm::vec3 newRotation);

	// MISC getters & setters
	float getSpeed();
	void setSpeed(float speed);

	// Object movement
	void drive();

	//Drawing & updating
	void draw();
	void update();
};