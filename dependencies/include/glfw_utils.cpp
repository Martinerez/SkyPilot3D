#include "glfw_utils.h"
#include <GLFW/glfw3.h>  // Necesario para el tipo de datos GLFWwindow
#include <iostream>       // Incluir para usar std::cout y std::endl

// Implementación de la función framebuffer_size_callback
void GLFWUtils::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);  // Configurar el tamaño del framebuffer
    std::cout << "Nuevo tamaño de ventana: " << width << "x" << height << std::endl;
}