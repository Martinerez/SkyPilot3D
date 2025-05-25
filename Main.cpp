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


float skyBoxVerticess [] =
{
    //Coordinates

    -1.0f,  -1.0f,  1.0f,
     1.0f,  -1.0f,  1.0f,
     1.0f,  -1.0f, -1.0f,
    -1.0f,  -1.0f, -1.0f,
    -1.0f,   1.0f,  1.0f,
     1.0f,   1.0f,  1.0f,
     1.0f,   1.0f, -1.0f,
    -1.0f,   1.0f, -1.0f
};

unsigned int skyBoxIndices[] = {

    //Right
    1, 2, 6,
    6, 5, 1,
    //Left
    0, 4, 7,
    7, 3, 0,
    //Top
    4, 5, 6,
    6, 7, 4,
    //Bottom
    0, 3, 2,
    2, 1, 0,
    //Back
    0, 1, 5,
    5, 4, 0,
    //Front
    3, 7, 6,
    6, 2, 3
};




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

    int xpos = (screenWidth - static_cast<int>(WIDTH)) / 2;
    int ypos = (screenHeight - static_cast<int>(HEIGHT)) / 2;

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

    unsigned int skyBoxVao, skyBoxVBO, skyBoxEBO;
    glGenVertexArrays(1, &skyBoxVao);
    glGenBuffers(1, &skyBoxVBO);
    glGenBuffers(1, &skyBoxEBO);
    glBindVertexArray(skyBoxVao);
    glBindBuffer(GL_ARRAY_BUFFER, skyBoxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyBoxVerticess), &skyBoxVerticess, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyBoxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyBoxIndices), &skyBoxIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    std::string parentDir = "./Resource Files/Textures/sky.jpg";


    std::string FacesCubemap[6] = {

        parentDir,
        parentDir,
        parentDir,
        parentDir,
        parentDir,
        parentDir
    };

    unsigned int cubeMapTexture;
    glGenTextures(1, &cubeMapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (unsigned int i = 0; i < 6; i++) {

        int width, height, nrChannels;
        unsigned char* data = stbi_load(FacesCubemap[i].c_str(), &width, &height, &nrChannels, 0);

        if (data) {
            stbi_set_flip_vertically_on_load(false);
            glTexImage2D(

                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGB,
                width,
                height,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                data
            );
            stbi_image_free(data);

        }
        else {
            std::cout << "Failed to load texture: " << FacesCubemap[i] << std::endl;
            stbi_image_free(data);
        }
    }



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