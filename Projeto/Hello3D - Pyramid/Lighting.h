#ifndef LIGHTING_H
#define LIGHTING_H

#include <glm/glm.hpp>
#include "Shader.h"
#include <GLFW/glfw3.h>

class Lighting
{
private:
    glm::vec3 lightPos;
    float angle;
    float radius;
    float angularSpeed;

public:
    Lighting(const glm::vec3& initialPos, float initialAngle, float initialRadius, float initialAngularSpeed);
    void setLightPos(const glm::vec3& pos);
    void performCircularMovement(GLFWwindow* window);
    void setLightUniforms(const Shader& shader) const;
};

#endif

