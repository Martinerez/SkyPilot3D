#include"Camera.h"

#define GLM_ENABLE_EXPERIMENTAL


Camera::Camera(int width, int height, glm::vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Position = position;
	defaultPosition = position;

}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane)
{
	// Initializes matrices since otherwise they will be the null matrix
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// Makes camera look in the right direction from the right position
	view = glm::lookAt(Position, Position + Orientation, Up);
	// Adds perspective to the scene
	projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

	// Sets new camera matrix
	cameraMatrix = projection * view;
}

void Camera::Matrix(Shader& shader, const char* uniform)
{
	// Exports camera matrix
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

void Camera::Inputs(GLFWwindow* window, float deltaTime, glm::vec3 personajePos)
{
	autoSpeed += 0.1f * deltaTime;
	glm::vec3 direction = Orientation * autoSpeed; // movimiento automático

	// Manejar entradas del teclado
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		direction += Orientation;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		direction += Up;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		direction -= Up;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		direction -= glm::normalize(glm::cross(Orientation, Up));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		direction += glm::normalize(glm::cross(Orientation, Up));

	// Calcular nueva posición
	glm::vec3 newPosition = Position + direction * speed * deltaTime;

	// Limitar con respecto a la posición actual
	float limitX = 1.5f;
	float limitY = 1.5f;

	float minX = Position.x - limitX;
	float maxX = Position.x + limitX;
	float minY = Position.y - limitY;
	float maxY = Position.y + limitY;

	// Aplicar límites
	newPosition.x = std::max(minX, std::min(maxX, newPosition.x));
	newPosition.y = std::max(minY, std::min(maxY, newPosition.y));

	// Actualizar posición final
	Position = newPosition;



	// Suaviza el movimiento interpolando la velocidad
	float smoothness = 20.0f;
	velocity = glm::mix(velocity, direction * speed, smoothness * deltaTime);

	Position += direction * deltaTime;


	// Handles mouse inputs
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		orbitMode = true;
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		orbitMode = false;
		firstClick = true;
	}

	if (orbitMode)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);


		// orbitCenter debe actualizarse con el personaje SIEMPRE (por si se movió)
		orbitCenter = personajePos;

		if (firstClick)
		{
			glfwSetCursorPos(window, width / 2, height / 2);
			firstClick = false;

		}



		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		float rotY = sensitivity * static_cast<float>(mouseX - (width / 2)) / width;
		angle += rotY;

		// Nueva posición en círculo
		Position.x = orbitCenter.x + radius * sin(glm::radians(angle));
		Position.z = orbitCenter.z + radius * cos(glm::radians(angle));
		Position.y = orbitCenter.y;// +1.0f;  // altura constante sobre el personaje

		// Recalcular dirección
		Orientation = glm::normalize(orbitCenter - Position);

		glfwSetCursorPos(window, width / 2, height / 2);
	}
	else
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}


}