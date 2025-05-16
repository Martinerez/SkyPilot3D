//WE start including the libraries we will use

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>  
#include <glm/gtc/type_ptr.hpp>       
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <iostream>
#include "stb/stb_image.h"


//Size of menu window
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 800;

// Callback for changing the size

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}



int main() {
    //Initializing GLFW
    if (!glfwInit()) {
        std::cerr << "GLFW initialization failure \n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Creating the menu window

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed creating the window \n";
        glfwTerminate();
        return -1;
    }
    //CENTERING the window in the screen

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int screenWidth = mode->width;
    int screenHeight = mode->height;

    int xpos = (screenWidth - WIDTH) / 2;
    int ypos = (screenHeight - HEIGHT) / 2;

    glfwSetWindowPos(window, xpos, ypos);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initializing  GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed initializing GLAD \n";
        return -1;
    }

    //Configurating ImGui 

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();


    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    //Main loop
    glClearColor(0.6f, 1.0f, 0.6f, 1.0f);
    //Variable to know if we are on game mood
    bool isGameMode = false;


    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        //ImGui Frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Menu in the center
        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        //Menu size
        ImVec2 menuSize(500, 300);  
        //Menu centered
        ImVec2 menuPos((windowWidth - menuSize.x) / 2, (windowHeight - menuSize.y) / 2);  

        ImGui::SetNextWindowSize(menuSize);
        ImGui::SetNextWindowPos(menuPos);

        //Bottoms color
        ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(0.843f, 0.502f, 1.0f, 1.0f);  // #d780ff
        ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = ImVec4(0.9f, 0.6f, 1.0f, 1.0f);
        ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = ImVec4(0.7f, 0.3f, 1.0f, 1.0f);

        if (!isGameMode) {
        //Visible window
            ImGui::Begin("SkyPlane3D Menu", nullptr,
             ImGuiWindowFlags_NoResize |
             ImGuiWindowFlags_NoCollapse |
             ImGuiWindowFlags_NoSavedSettings
            );

            float buttonWidth = 280.0f;
            float buttonHeight = 40.0f;
            float centerX = (menuSize.x - buttonWidth) / 2;

            ImGui::SetCursorPos(ImVec2(centerX, 100));
            if (ImGui::Button("PLAY", ImVec2(buttonWidth, buttonHeight))) {
                const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
                glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
                isGameMode = true;
                glfwSetWindowTitle(window, "SkyPlane3D - Game Mode");

                //white background
                glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  
            }

            //About bottom centered
            ImGui::SetCursorPos(ImVec2(centerX, 160));
            if (ImGui::Button("About...", ImVec2(buttonWidth, buttonHeight))) {
               
                //What about bottom will do
            }

            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
}