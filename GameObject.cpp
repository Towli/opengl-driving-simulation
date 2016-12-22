#include "GameObject.h"

GameObject::GameObject()
{
	speed = 0.0;
	currentAngle = 0.0;
}

GameObject::~GameObject(){}

GameObject::GameObject(Shader* shader, ThreeDModel* model)
{
	this->model = model;
	this->shader = shader;
	position = glm::vec3(0, 0, 0);
	speed = 0.0;
	turnSpeed = 3.0f;
	currentAngle = 0.0;
	boundingSphere = Sphere(shader, position, 12.0f);
	outOfBounds = false;
	busted = false;
}

GameObject::GameObject(Shader* shader, vector<ThreeDModel*> models)
{
	this->models = models;
	this->shader = shader;
	position = glm::vec3(0, 0, 0);
	speed = 0.0;
	turnSpeed = 3.0f;
	currentAngle = 0.0;
	boundingSphere = Sphere(shader, position, 12.0f);
	outOfBounds = false;
	busted = false;
}

void GameObject::update()
{
	this->boundingSphere.setCentre(position);
}

/* Should not be used when drawing with a ThreeDModel; only used for testing. */
void GameObject::drawGeometry()
{
	boundingSphere.render();
}

void GameObject::setOutOfBounds(bool b)
{
	this->outOfBounds = b;
}

bool GameObject::isOutOfBounds()
{
	return this->outOfBounds;
}

bool GameObject::isBusted()
{
	return this->busted;
}

void GameObject::setBusted(bool b)
{
	this->busted = b;
}

void GameObject::respondToCollision(double deltaTime)
{
	// Move in opposite direction for short period of time, tapering off
	if (speed < 200.0f && speed != 0)
		this->speed = -speed;
	else 
	if (speed >= 200.0f) {
		busted = true;
		this->speed = 0.0f;
	}
}

void GameObject::move(double deltaTime)
{
	this->deltaTurnSpeed = (speed*0.3*deltaTime) * (turnSpeed * deltaTime);
	position.x += glm::sin(glm::radians((getDirection()))) * speed * deltaTime;
	position.z += glm::cos(glm::radians((getDirection()))) * speed * deltaTime;
	update();
}

void GameObject::turn(int direction)
{
	currentAngle += deltaTurnSpeed * direction;
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

ThreeDModel* GameObject::getModel()
{
	return this->model;
}

vector<ThreeDModel*> GameObject::getModels()
{
	return this->models;
}

Sphere GameObject::getBoundingSphere()
{
	return this->boundingSphere;
}