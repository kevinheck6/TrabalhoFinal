#pragma once

#include <stb_image.h>

#include <vector>

#include "Mesh.h"

using namespace std;

class Object
{
public:
	Object() {}
	void initialize(string filePath, Shader* shader, glm::vec3 position = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1), float angle = 0.0, glm::vec3 axis = glm::vec3(0.0, 0.0, 1.0));
	void update();
	void draw();
protected:
	void loadObj(string filePath);
	int generateTexture(string filePath);
	GLuint generateVAO(vector <GLfloat> vertbuffer, int& nVertices);
	vector <Mesh> grupos;
	//Atributos que armazenam informações para aplicar as transforms no objeto
	glm::vec3 position;
	float angle;
	glm::vec3 axis;
	glm::vec3 scale;
	Shader* shader;

};

