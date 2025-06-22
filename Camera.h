#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H
#define GLM_ENABLE_EXPERIMENTAL

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

#include"shaderClass.h"

class Camera
{
public:
	// Stores the main vectors of the camera
	glm::vec3 Position;
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 cameraMatrix = glm::mat4(1.0f);
	glm::vec3 defaultPosition;
	bool orbitMode = false;
	float angle = 0.0f;
	glm::vec3 target = glm::vec3(0.0f); // el punto alrededor del cual se orbita

	float radius = 5.0f;
	glm::vec3 orbitCenter = glm::vec3(0.0f);  // centro de órbita
	glm::vec3 savedPosition;                  // para recordar la posición antes del clic



	// Prevents the camera from jumping around when first clicking left click
	bool firstClick = true;

	// Stores the width and height of the window
	int width;
	int height;

	// Adjust the speed of the camera and it's sensitivity when looking around
	float speed = 10.00f;
	float autoSpeed = 1.0f;
	float sensitivity = 10.00f;
	glm::vec3 velocity = glm::vec3(0.0f); // Nueva variable para suavidad

	// Camera constructor to set up initial values
	Camera(int width, int height, glm::vec3 position);

	// Updates the camera matrix to the Vertex Shader
	void updateMatrix(float FOVdeg, float nearPlane, float farPlane);
	// Exports the camera matrix to a shader
	void Matrix(Shader& shader, const char* uniform);
	// Handles camera inputs
	//void Inputs(GLFWwindow* window); // Modifica la firma
	void Inputs(GLFWwindow* window, float deltaTime); // Modifica la firma
	void Inputs(GLFWwindow* window, float deltaTime, glm::vec3 personajePos);

};
#endif


