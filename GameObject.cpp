#include "GameObject.h"


GameObject::GameObject(Shader* shader, ThreeDModel model)
{
	this->shader = shader;
	//position = ?
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

glm::vec3 GameObject::getPosition() 
{
	return this->position;
}

void GameObject::setPosition(glm::vec3 position) 
{
	this->position = position;
}