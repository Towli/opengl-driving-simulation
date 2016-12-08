#pragma once
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "GameObject.h"

enum class Type { FIRST = 1, THIRD = 2, STATIC = 3, ACTION = 4 };

class Camera
{
private:
	GameObject* targetObject;
	glm::mat4 viewingMatrix;
	glm::vec3 offset;
	glm::vec3 position;
	glm::vec3 lookAt;
	glm::vec3 up;
	Type type;

public:
	Camera(GameObject* targetObject, Type type);
	Camera();
	~Camera();

	void update();
	void follow();
	glm::vec2 getDistance(GameObject* targetObject);

	// Update views to for respective camera types
	void updateFPSView();
	void updateTPSView();
	void updateStaticView();
	void updateActionView();

	// Getter & Setter for Viewing Matrix
	glm::mat4 getViewingMatrix();
	void setViewingMatrix(glm::mat4 viewingMatrix);

	// Getters & Setters for LookAt attributes
	glm::vec3 getPosition();
	glm::vec3 getLookAt();
	glm::vec3 getUp();
	Type getType();
	void setPosition(glm::vec3 position);
	void setLookAt(glm::vec3 lookAt);
	void setUp(glm::vec3 up);
	void setType(Type type);
};

