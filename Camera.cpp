	#include "Camera.h"
#include "glm/ext.hpp"

	Camera::Camera(){}

	Camera::~Camera(){}

	Camera::Camera(GameObject* targetObject, Type type)
	{
		this->type = type;
		this->targetObject = targetObject;
		up = glm::vec3(0.0f, 1.0f, 0.0f);
		position = glm::vec3(0.0f, 20.0f, 0.0f);
		update();
	}

	void Camera::update()
	{
		switch (type)
		{
			case Type::FIRST:
				updateFPSView();
				break;
			case Type::THIRD:
				updateTPSView();
				break;
			case Type::STATIC:
				updateStaticView();
				break;
			case Type::ACTION:
				updateActionView();
				break;
			default:
				updateStaticView();
				break;
		}
		viewingMatrix = glm::lookAt(position, lookAt, up);
	}

	void Camera::updateFPSView()
	{
		float direction = targetObject->getDirection();
		position = targetObject->getPosition() + glm::vec3(0.0f, 6.0f, 0.0f);
		lookAt = position + glm::vec3(glm::sin(glm::radians(direction)), 0.0f, glm::cos(glm::radians(direction)));
	}

	void Camera::updateTPSView()
	{
		float direction = targetObject->getDirection();
		offset = glm::vec3(35 * glm::sin(glm::radians(direction)), -10.0f, 35 * glm::cos(glm::radians(direction)));
		position = targetObject->getPosition() - offset;
		lookAt = targetObject->getPosition();
	}

	void Camera::updateActionView()
	{
		position.y = 20.0f;
		glm::vec2 distance = getDistance(targetObject);
		if (distance.x > 200 || distance.y > 200 || distance.x < -200 || distance.y < -200) {
			position = glm::vec3(targetObject->getPosition().x, 20.0f, targetObject->getPosition().z);
		}
		lookAt = targetObject->getPosition() + glm::vec3(0.0f, 1.0f, 5.0f);	// adding this vector is a hacky fix to prevent linker error!
	}

	void Camera::updateStaticView()
	{
		position = glm::vec3(targetObject->getPosition().x, 70.0f, targetObject->getPosition().z);
		lookAt = targetObject->getPosition() + glm::vec3(0.1f, 0.1f, 0.0f);
	}

	glm::vec2 Camera::getDistance(GameObject* targetObject) {
		float targetX = targetObject->getPosition().x;
		float targetZ = targetObject->getPosition().z;
		
		return glm::vec2(position.x - targetX, position.z - targetZ);
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

	void Camera::setType(Type type)
	{
		this->type = type;
	}

	Type Camera::getType()
	{
		return this->type;
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