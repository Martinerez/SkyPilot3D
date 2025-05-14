#include "glfw_utils.h"
#include <GLFW/glfw3.h>  // Necesario para el tipo de datos GLFWwindow
#include <iostream>       // Incluir para usar std::cout y std::endl

// Implementaci�n de la funci�n framebuffer_size_callback
void GLFWUtils::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);  // Configurar el tama�o del framebuffer
    std::cout << "Nuevo tama�o de ventana: " << width << "x" << height << std::endl;
}