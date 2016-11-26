#pragma once
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "GameObject.h"

class Camera
{
private:
	GameObject* targetObject;
	glm::mat4 viewingMatrix;
	glm::vec3 offset;
	glm::vec3 position;
	glm::vec3 lookAt;
	glm::vec3 up;

public:
	Camera(GameObject* targetObject);
	Camera();
	~Camera();

	void update();
	void follow();

	// Getter & Setter for Viewing Matrix
	glm::mat4 getViewingMatrix();
	void setViewingMatrix(glm::mat4 viewingMatrix);

	// Getters & Setters for LookAt attributes
	glm::vec3 getPosition();
	glm::vec3 getLookAt();
	glm::vec3 getUp();
	void setPosition(glm::vec3 position);
	void setLookAt(glm::vec3 lookAt);
	void setUp(glm::vec3 up);
};

