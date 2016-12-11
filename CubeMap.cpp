#include "CubeMap.h"
#include "shaders\Shader.h"

CubeMap::CubeMap(Shader* shader) : GameObject()
{
	constructGeometry();
	this->faces.push_back("Models/CubeMap/textures/left.png");
	this->faces.push_back("Models/CubeMap/textures/right.png");
	this->faces.push_back("Models/CubeMap/textures/top.png");
	this->faces.push_back("Models/CubeMap/textures/bottom.png");
	this->faces.push_back("Models/CubeMap/textures/back.png");
	this->faces.push_back("Models/CubeMap/textures/front.png");
	createCubeMap();
	this->cubeMapShader = shader;
}

CubeMap::CubeMap() {}

void CubeMap::createCubeMap()
{
	glGenTextures(1, &this->texture);
	glActiveTexture(GL_TEXTURE0);

	int width, height;
	//unsigned char* image;
	nv::Image image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture);
		for (GLuint i = 0; i < this->faces.size(); i++)
		{
			if (image.loadImageFromFile(faces.at(i)))
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, image.getInternalFormat(), image.getWidth(), image.getHeight(), 0, image.getFormat(), image.getType(), image.getLevel(0));
				cout << "Loaded CubeMap image: " << faces[i] << endl;
			}
			else
			{
				cout << "Failed to load CubeMap image: " << i + 1 << endl;
			}
			image.~Image();
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}


void CubeMap::render()
{
	glBindVertexArray(this->m_vaoID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void CubeMap::constructGeometry()
{
	GLfloat vertices[] =
	{
		-1000.0f,  1000.0f, -1000.0f,
		-1000.0f, -1000.0f, -1000.0f,
		1000.0f, -1000.0f, -1000.0f,
		1000.0f, -1000.0f, -1000.0f,
		1000.0f,  1000.0f, -1000.0f,
		-1000.0f,  1000.0f, -1000.0f,

		-1000.0f, -1000.0f,  1000.0f,
		-1000.0f, -1000.0f, -1000.0f,
		-1000.0f,  1000.0f, -1000.0f,
		-1000.0f,  1000.0f, -1000.0f,
		-1000.0f,  1000.0f,  1000.0f,
		-1000.0f, -1000.0f,  1000.0f,

		1000.0f, -1000.0f, -1000.0f,
		1000.0f, -1000.0f,  1000.0f,
		1000.0f,  1000.0f,  1000.0f,
		1000.0f,  1000.0f,  1000.0f,
		1000.0f,  1000.0f, -1000.0f,
		1000.0f, -1000.0f, -1000.0f,

		-1000.0f, -1000.0f,  1000.0f,
		-1000.0f,  1000.0f,  1000.0f,
		1000.0f,  1000.0f,  1000.0f,
		1000.0f,  1000.0f,  1000.0f,
		1000.0f, -1000.0f,  1000.0f,
		-1000.0f, -1000.0f,  1000.0f,

		-1000.0f,  1000.0f, -1000.0f,
		1000.0f,  1000.0f, -1000.0f,
		1000.0f,  1000.0f,  1000.0f,
		1000.0f,  1000.0f,  1000.0f,
		-1000.0f,  1000.0f,  1000.0f,
		-1000.0f,  1000.0f, -1000.0f,

		-1000.0f, -1000.0f, -1000.0f,
		-1000.0f, -1000.0f,  1000.0f,
		1000.0f, -1000.0f, -1000.0f,
		1000.0f, -1000.0f, -1000.0f,
		-1000.0f, -1000.0f,  1000.0f,
		1000.0f, -1000.0f,  1000.0f
	};

	glGenVertexArrays(1, &this->m_vaoID);
	glGenBuffers(1, &this->m_vboID);
	
	glBindVertexArray(this->m_vaoID);
		glBindBuffer(GL_ARRAY_BUFFER, this->m_vboID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);
}

GLuint CubeMap::getCubeMap()
{
	return this->texture;
}