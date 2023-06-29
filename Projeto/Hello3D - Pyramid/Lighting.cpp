#include "lighting.h"
#include <glm/gtc/constants.hpp>


Lighting::Lighting(const glm::vec3& initialPos, float initialAngle, float initialRadius, float initialAngularSpeed)
    : lightPos(initialPos), angle(initialAngle), radius(initialRadius), angularSpeed(initialAngularSpeed)
{
}

void Lighting::setLightPos(const glm::vec3& pos)
{
    lightPos = pos;
}

void Lighting::performCircularMovement(GLFWwindow* window)
{
    // Get the current time
    float currentTime = glfwGetTime();

    // Calculate new light position for circular movement
    lightPos.x = sin(angle) * radius;
    lightPos.z = cos(angle) * radius;

    // Increase the angle for the circular movement based on elapsed time
    // Increase the angle for the circular movement based on elapsed time
    float deltaTime = currentTime - angle;
    angle = currentTime;
    angle += glm::radians(angularSpeed) * deltaTime;
}

void Lighting::setLightUniforms(const Shader& shader) const
{
    shader.setVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);
}
