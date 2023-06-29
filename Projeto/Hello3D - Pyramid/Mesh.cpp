#include "Mesh.h"
#include <GLFW/glfw3.h>
#include <chrono>
#include <thread>

extern std::vector<Mesh*> suzanneObjects;
void Mesh::initialize(GLuint VAO, int nVertices, Shader* shader, GLuint texID, glm::vec3 position, glm::vec3 scale, bool isSelected, float angle, glm::vec3 axis)
{//AQUI CIMA
	this->VAO = VAO;
	this->nVertices = nVertices;
	this->shader = shader;
	this->texID = texID;
	this->position = position; 
	this->scale = scale; 
	this->angle = angle; 
	this->axis = axis;
	this->isSelected = isSelected;

	if (isSelected) {
		animateSize(1.1f, 1.0f);
	}


}

void Mesh::update()
{
	angle = angle + angleIncrement;
	glm::mat4 model = glm::mat4(1); 
	model = glm::translate(model, position); 
	model = glm::rotate(model, glm::radians(this->angle), this->axis); 
	model = glm::scale(model, scale); 
	shader->setMat4("model", glm::value_ptr(model)); 



}

void Mesh::draw()
{
	glBindTexture(GL_TEXTURE_2D, texID);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, nVertices);
	glBindVertexArray(0);
}

void Mesh::rotate(float angle, glm::vec3 axis)
{
	this->angle = angle;
	this->axis = axis;

	update();
}

void Mesh::rotateContinuous(float angleIncrement, const glm::vec3& axis)
{
	//this->angle += angleIncrement;
	this->angleIncrement = angleIncrement;
	this->axis = axis;
	update();
}

void Mesh::stopRotateContinuous()
{
	angleIncrement = 0.0f;
}

void Mesh::move(const glm::vec3& translation)
{
	position += translation;
	update();
}


void Mesh::animateSize(float scaleFactor, float duration) {
	if (!isSelected) {
		// Only animate if the object is selected
		return;
	}

	if (isAnimating) {
		// Animation is already in progress, stop the current animation
		stopAnimation();
	}

	targetScale = scaleFactor;
	originalScale = scale.x;
	animationDuration = duration;

	isAnimating = true;
	isGrowing = true;
	animationStartTime = glfwGetTime();
}

void Mesh::updateAnimation() {
	if (!isAnimating) {
		return;
	}

	float currentTime = static_cast<float>(glfwGetTime());
	float elapsedTime = currentTime - animationStartTime;

	if (isGrowing) {
		// Growing phase
		if (elapsedTime >= animationDuration) {
			// Transition to shrinking phase
			elapsedTime -= animationDuration;
			isGrowing = false;
			animationStartTime = currentTime;
		}
		else {
			// Calculate the interpolated scale for growing
			float t = elapsedTime / animationDuration;
			float currentScale = glm::mix(originalScale, targetScale, t);
			scale = glm::vec3(currentScale);
		}
	}
	else {
		// Shrinking phase
		if (elapsedTime >= animationDuration) {
			// Transition back to growing phase
			elapsedTime -= animationDuration;
			isGrowing = true;
			animationStartTime = currentTime;
		}
		else {
			// Calculate the interpolated scale for shrinking
			float t = elapsedTime / animationDuration;
			float currentScale = glm::mix(targetScale, originalScale, t);
			scale = glm::vec3(currentScale);
		}
	}
}

void Mesh::setSelected(bool selected) {
	isSelected = selected;

	if (!isSelected) {
		// If deselected, stop the animation and reset to original scale
		stopAnimation();
		scale = glm::vec3(originalScale);
	}
}

void Mesh::stopAnimation() {
	isAnimating = false;
	isGrowing = true;
	animationStartTime = 0.0;
	animationDuration = 0.0;
}
