#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "shaderClass.h"

class Camera
{
public:
    // Camera attributes
    glm::vec3 Position;
    glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 defaultPosition;
    bool orbitMode = false;
    glm::vec3 target = glm::vec3(0.0f);
    float angle = 0.0f;

    float radius = 5.0f;
    glm::vec3 orbitCenter = glm::vec3(0.0f);
    glm::vec3 savedPosition;

    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 cameraMatrix = glm::mat4(1.0f);

    // Window settings
    int width;
    int height;

    // Control flags
    bool firstClick = true;

    // Movement speed and mouse sensitivity
    float speed = 10.00f;
    float autoSpeed = 10.00f;
    float sensitivity = 10.00f;
    glm::vec3 velocity = glm::vec3(0.0f);

    // Constructor
    Camera(int width, int height, glm::vec3 position);

    // Main methods
    void updateMatrix(float FOVdeg, float nearPlane, float farPlane);
    void Matrix(Shader& shader, const char* uniform);
    void Inputs(GLFWwindow* window);
    //void Inputs(GLFWwindow* window, float deltaTime); // Modifica la firma
    void Inputs(GLFWwindow* window, float deltaTime, glm::vec3 personajePos);

};

#endif