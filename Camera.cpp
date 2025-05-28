#include"Camera.h"



Camera::Camera(int width, int height, glm::vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Position = position;
}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane)
{
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	view = glm::lookAt(Position, Position + Orientation, Up);
	projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

	cameraMatrix = projection * view;
}


void Camera::Matrix(Shader& shader, const char* uniform)
{
	// Exports camera matrix
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

glm::vec3 minBounds = glm::vec3(-5.0f, -5.0f, -20.0f);
glm::vec3 maxBounds = glm::vec3(5.0f, 5.0f, -20.0f);

void Camera::Inputs(GLFWwindow* window)
{
	// Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Position += speed * glm::normalize(glm::cross(Orientation, Up));
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Position += speed * -glm::normalize(glm::cross(Orientation, Up));
	}

	// Ajusta velocidad con Shift
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		speed = 0.1f;
	}
	else
	{
		speed = 0.1f;
	}

	// Limitar posición a los rangos establecidos
	Position.x = glm::clamp(Position.x, minBounds.x, maxBounds.x);
	Position.y = glm::clamp(Position.y, minBounds.y, maxBounds.y);
	Position.z = glm::clamp(Position.z, minBounds.z, maxBounds.z);

	// Handles mouse inputs
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		if (firstClick)
		{
			glfwSetCursorPos(window, (width / 2), (height / 2));
			firstClick = false;
		}

		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// Calcula cuánto girar en X (horizontal)
		float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

		// Angulo de rotación (acumulativo)
		static float angle = 0.0f;
		angle += rotY;

		// Radio del círculo
		float radius = 20.0f;

		// Centro de la órbita (puede ser (0, 0, 0) o donde está el objeto)
		glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);

		// Nueva posición de la cámara en órbita (en eje XZ)
		Position.x = target.x + radius * sin(glm::radians(angle));
		Position.z = target.z + radius * cos(glm::radians(angle));

		// Miramos siempre al centro
		Orientation = glm::normalize(target - Position);

		glfwSetCursorPos(window, (width / 2), (height / 2));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		firstClick = true;
	}
}