	#include "Camera.h"

	Camera::Camera(){}

	Camera::~Camera(){}

	Camera::Camera(GameObject* targetObject)
	{
		this->targetObject = targetObject;
		up = glm::vec3(0.0f, 1.0f, 0.0f);
		update();
	}

	void Camera::update()
	{
		float direction = targetObject->getDirection();
		offset = glm::vec3(10*glm::sin(glm::radians(direction)),-5.0f, 10*glm::cos(glm::radians(direction)));
		position = targetObject->getPosition() - offset;
		lookAt = targetObject->getPosition();
		viewingMatrix = glm::lookAt(position, lookAt, up);
	}

	void Camera::setPosition(glm::vec3 position)
	{
		this->position = position;
	}

	void Camera::setLookAt(glm::vec3 lookAt)
	{
		this->lookAt = lookAt;
	}

	void Camera::setUp(glm::vec3 up)
	{
		this->up = up;
	}

	glm::vec3 Camera::getPosition()
	{
		return this->position;
	}

	glm::vec3 Camera::getLookAt()
	{
		return this->lookAt;
	}

	glm::vec3 Camera::getUp()
	{
		return this->up;
	}

	glm::mat4 Camera::getViewingMatrix()
	{
		return this->viewingMatrix;
	}

	void Camera::setViewingMatrix(glm::mat4 viewingMatrix)
	{
		this->viewingMatrix = viewingMatrix;
	}