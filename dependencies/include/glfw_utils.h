#ifndef GLFW_UTILS_H
#define GLFW_UTILS_H

#include <GLFW/glfw3.h>

// Definir la clase GLFWUtils con funciones útiles
class GLFWUtils {
public:
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};

#endif // GLFW_UTILS_H