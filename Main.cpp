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

    // Set the font size
    io.FontGlobalScale = 1.5f;  // new change to increase font size

    ImGui::StyleColorsDark();


    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    //Main loop
    glClearColor(0.6f, 1.0f, 0.6f, 1.0f);

    //Variable to know if we are on game mood
    bool isGameMode = false;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // ImGui Frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Set ImGui style
        ImGuiStyle& style = ImGui::GetStyle();

        // Set custom colors for the ImGui style
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.2f, 0.95f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.4f, 0.3f, 0.8f, 1.0f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.5f, 0.4f, 0.9f, 1.0f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.3f, 0.2f, 0.7f, 1.0f);
        style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        style.Colors[ImGuiCol_Separator] = ImVec4(0.6f, 0.6f, 0.7f, 0.5f);

        style.FrameRounding = 6.0f;
        style.WindowRounding = 8.0f;
        style.FrameBorderSize = 0.0f;
        style.PopupRounding = 6.0f;

        style.ItemSpacing = ImVec2(10, 20);
        style.WindowPadding = ImVec2(20, 20);
        style.Colors[ImGuiCol_WindowBg].w = 0.9f;

        // get window size
        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        // main menu
        ImVec2 menuSize(600, 300);
        ImVec2 menuPos((windowWidth - menuSize.x) / 2, (windowHeight - menuSize.y) / 2);
        ImGui::SetNextWindowSize(menuSize);
        ImGui::SetNextWindowPos(menuPos);

        //set the color of the menu buttons 
        ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(0.843f, 0.502f, 1.0f, 1.0f);  // #d780ff
        ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = ImVec4(0.9f, 0.6f, 1.0f, 1.0f);
        ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = ImVec4(0.7f, 0.3f, 1.0f, 1.0f);


        // menu background color
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.95f));

        if (!isGameMode) {
            ImGui::Begin("SkyPlane3D Menu", nullptr,
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoTitleBar
            );

            // title
            ImGui::SetCursorPosY(20);
            ImGui::SetCursorPosX((menuSize.x - ImGui::CalcTextSize("SkyPlane3D").x) / 2);
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 1.0f, 1.0f), "SkyPlane3D");

            // separation line
            ImGui::Separator();

            // center the buttons
            float buttonWidth = 280.0f;
            float buttonHeight = 40.0f;
            float centerX = (menuSize.x - buttonWidth) / 2;

            ImGui::SetCursorPos(ImVec2(centerX, 100));
            if (ImGui::Button("PLAY", ImVec2(buttonWidth, buttonHeight))) {
                const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
                glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
                isGameMode = true;
                glfwSetWindowTitle(window, "SkyPlane3D - Game Mode");
                glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            }

            // button "About the game"
            ImGui::SetCursorPos(ImVec2(centerX, 160));
            static bool showAbout = false;
            if (ImGui::Button("About the game", ImVec2(buttonWidth, buttonHeight))) {
                showAbout = true;
            }

            // popup "About"
            if (showAbout) {
                ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
                ImGui::OpenPopup("How to play?");
                // reset the flag to close the popup after opening
                showAbout = false;
            }

            // popup "How to play?"
            if (ImGui::BeginPopupModal("How to play?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {

                ImGui::TextColored(ImVec4(0.6f, 0.4f, 0.9f, 1.0f), "SkyPlane3D");
                ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Version 1.0");

                /*
                *ImGui::Text("SkyPlane3D");
                ImGui::Text("Version 1.0");
                */
                ImGui::Separator();
                ImGui::Text("SkyPlane3D is an exciting third - person game where you control a plane \nsoaring above a city filled with obstacles.\n\n"
                    "To maneuver through the skies, use the following keys:\n"
                    "  - W to ascend\n"
                    "  - S to descend\n"
                    "  - A to go left\n"
                    "  - D to go right\n"
                    "  - G to start the game\n");

                if (ImGui::Button("Close")) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }


            //For Sharon
            ImGui::SetCursorPos(ImVec2(centerX, 220));
            if (ImGui::Button("Options", ImVec2(buttonWidth, buttonHeight))) {
                // Actions for the button "options" can be added here
            }

            ImGui::End();

        }

        ImGui::PopStyleColor();

        // Rendering
        ImGui::Render();
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

}