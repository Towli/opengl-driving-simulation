#ifndef _CubeMap_H
#define _CubeMap_H

#include "gl\glew.h"
#include "GameObject.h"
#include "images\imageLoader.h"
#include "images\nvImage.h"

class Shader;

class CubeMap : public GameObject
{
private:
	Shader* cubeMapShader;
	GLuint m_vaoID, m_vboID;
	GLuint texture;
	vector<const GLchar*> faces;
public:
	CubeMap(Shader* shader);
	CubeMap();
	void createCubeMap();
	void constructGeometry();
	void render();
	GLuint getCubeMap();
};

#endif _CubeMap_H