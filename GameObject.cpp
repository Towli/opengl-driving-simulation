#include "GameObject.h"

GameObject::GameObject(Shader* shader, ThreeDModel model)
{
	this->shader = shader;
	position = glm::vec3(0, 0, 0);
	speed = 0.0;
	turnSpeed = 0.07;
	currentAngle = 0.0;
}

GameObject::GameObject()
{
	speed = 0.0;
	turnSpeed = 0.07;
	currentAngle = 0.0;
}

GameObject::~GameObject(){}

/* Should not be used when drawing with a ThreeDModel; only used for testing. */
void GameObject::draw(){}

void GameObject::move()
{
	speed *= 0.8;
	position.x += glm::sin(glm::radians((getDirection()))) * speed;
	position.z += glm::cos(glm::radians((getDirection()))) * speed;
}

void GameObject::turn(int direction)
{
	currentAngle += turnSpeed * direction;
}

float GameObject::getDirection()
{
	return currentAngle / PI * 180;
}

glm::vec3 GameObject::getPosition() 
{
	return this->position;
}

void GameObject::setPosition(glm::vec3 position) 
{
	this->position = position;
}

float GameObject::getSpeed()
{
	return this->speed;
}

void GameObject::setSpeed(float speed)
{
	this->speed = speed;
}