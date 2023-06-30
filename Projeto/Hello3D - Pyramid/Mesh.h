#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>
#include <vector>

#include "Shader.h"


class Mesh
{
public:
	Mesh() {}
	~Mesh() {}
	void initialize(GLuint VAO, int nVertices, Shader *shader, GLuint texID, glm::vec3 position = glm::vec3(0.0, 0.0, 0.0), glm::vec3 scale = glm::vec3(1.0, 1.0, 1.0), bool isSelected = false,float angle = 0.0, glm::vec3 axis = glm::vec3(0.0, 0.0, 1.0));
	void update();//AQUI CIMA
	void draw();
	void rotate(float angle, glm::vec3 axis);
	void rotateContinuous(float angleIncrement, const glm::vec3& axis);
	void move(const glm::vec3& translation);


	void animateSize(float scaleFactor, float duration);
	void updateAnimation();
	void setSelected(bool selected);

	void stopRotateContinuous();

	void setPosition(glm::vec3 position);


protected:
	bool shouldAnimationFinish();
	void stopAnimation();


	GLuint VAO;
	int nVertices;
	Shader* shader;
	GLuint texID;

	glm::vec3 position; 
	glm::vec3 scale;
	float angle;
	glm::vec3 axis;
	float angleIncrement;

	bool isSelected = false;
	bool isAnimating = false;
	bool isGrowing = true;
	double animationStartTime = 0.0;
	double animationDuration = 0.0;
	float targetScale = 1.1f;
	float originalScale = 1.0f;



	//bool selected = false;

};

