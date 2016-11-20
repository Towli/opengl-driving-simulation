#include "Car.h"
#include "Box.h"

Car::Car()
{
}


Car::~Car()
{
}

glm::vec3 Car::getPosition()
{
	return this->position;
}

glm::vec3 Car::getRotation()
{
	return this->rotation;
}

void Car::setRotation(glm::vec3 newRotation)
{
	this->rotation = newRotation;
}