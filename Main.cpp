//WE start including the libraries we will use

#define GLM_ENABLE_EXPERIMENTAL
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
#include"Texture.h"
#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"Camera.h"
#include<filesystem>


int width = 800;
int height = 800;


float skyboxVertices[] =
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

unsigned int skyboxIndices[] = {

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



int main()
{
	//Initializing GLFW
	glfwInit();


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Creating the menu window
	GLFWwindow* window = glfwCreateWindow(width, height, "SkyPlane3D", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	//CENTERING the window in the screen

	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	int screenWidth = mode->width;
	int screenHeight = mode->height;

	int xpos = (screenWidth - width) / 2;
	int ypos = (screenHeight - height) / 2;

	glfwSetWindowPos(window, xpos, ypos);
	glfwMakeContextCurrent(window);
	gladLoadGL();

	//Configurating ImGui 
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	glViewport(0, 0, width, height);

	//Generates Shader objects for the skybox

	Shader shaderProgram("default.vert", "default.frag");
	Shader skyboxShader("skybox.vert", "skybox.frag");

	//Light related things

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	skyboxShader.Activate();
	glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);


	//Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	//Enables Cull Facing
	glEnable(GL_CULL_FACE);

	//Keeps front faces
	glCullFace(GL_FRONT);

	//Uses counter clock-wise standard
	glFrontFace(GL_CCW);

	//Creates camera object
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	//Replace this with your path (To the team)

	std::string parentDir = "C:/Users/DELL 5590/Desktop/SkyPilot3D/Resources";
	std::string facesCubemap[6] = {
		parentDir + "/right.png",
		parentDir + "/left.png",
		parentDir + "/top.png",
		parentDir + "/bottom.png",
		parentDir + "/front.png",
		parentDir + "/back.png"
	};


	//Variables to create periodic event for FPS displaying
	double prevTime = 0.0;
	double crntTime = 0.0;
	double timeDiff;

	//Keeps track of the amount of frames in timeDiff
	unsigned int counter = 0;

	//Create VAO, VBO, and EBO for the skybox
	unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	unsigned int cubeMapTexture;
	glGenTextures(1, &cubeMapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
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
		else
		{
			std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
			stbi_image_free(data);
		}
	}


	//Variable to know if we are on game mood
	bool gameMood = false;



	// Set the font size
	io.FontGlobalScale = 1.5f;  // new change to increase font size

	ImGui::StyleColorsDark();

	bool isSpanish = false;
	static float brightness = 1.0f;

	while (!glfwWindowShouldClose(window))
	{
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
		ImVec2 menuSize(600, 400);
		ImVec2 menuPos((windowWidth - menuSize.x) / 2, (windowHeight - menuSize.y) / 2);
		ImGui::SetNextWindowSize(menuSize);
		ImGui::SetNextWindowPos(menuPos);

		//set the color of the menu buttons 
		ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(0.843f, 0.502f, 1.0f, 1.0f);  // #d780ff
		ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = ImVec4(0.9f, 0.6f, 1.0f, 1.0f);
		ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = ImVec4(0.7f, 0.3f, 1.0f, 1.0f);


		// menu background color
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.95f));

		//Showing the menu if we are not on game mood

		if (!gameMood)
		{
			glClearColor(0.6f, 1.0f, 0.6f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
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
			if (ImGui::Button(isSpanish ? "JUGAR" : "PLAY", ImVec2(buttonWidth, buttonHeight))) {
				gameMood = true;
				GLFWmonitor* monitor = glfwGetPrimaryMonitor();
				const GLFWvidmode* mode = glfwGetVideoMode(monitor);

				glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
				glViewport(0, 0, mode->width, mode->height);

				width = mode->width;
				height = mode->height;
			}

			// button "About the game"
			ImGui::SetCursorPos(ImVec2(centerX, 160));
			static bool showAbout = false;
			if (ImGui::Button(isSpanish ? "Acerca del juego" : "About the game", ImVec2(buttonWidth, buttonHeight))) {
				showAbout = true;
			}

			// popup "About"
			if (showAbout) {
				ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
				ImGui::OpenPopup("How to play?");
				// reset the flag to close the popup after opening
				showAbout = false;
			}

			if (ImGui::BeginPopupModal("How to play?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImGui::TextColored(ImVec4(0.6f, 0.4f, 0.9f, 1.0f), "SkyPlane3D");
				ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Version 1.0");

				ImGui::Separator();

				if (isSpanish) {
					ImGui::Text("SkyPlane3D es un emocionante juego en tercera persona donde controlas un avion\n"
						"que sobrevuela una ciudad llena de obstaculos.\n\n"
						"Para maniobrar por los cielos, usa las siguientes teclas:\n"
						"  - W para subir\n"
						"  - S para bajar\n"
						"  - A para ir a la izquierda\n"
						"  - D para ir a la derecha\n"
						"  - G para iniciar el juego\n");
				}
				else {
					ImGui::Text("SkyPlane3D is an exciting third-person game where you control a plane\n"
						"soaring above a city filled with obstacles.\n\n"
						"To maneuver through the skies, use the following keys:\n"
						"  - W to ascend\n"
						"  - S to descend\n"
						"  - A to go left\n"
						"  - D to go right\n"
						"  - G to start the game\n");
				}

				if (ImGui::Button(isSpanish ? "Cerrar" : "Close")) {
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			static bool showOptions = false;

			ImGui::SetCursorPos(ImVec2(centerX, 220));
			if (ImGui::Button(isSpanish ? "Opciones" : "Options", ImVec2(buttonWidth, buttonHeight))) {
				showOptions = true;
			}

			if (showOptions) {
				ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
				ImGui::OpenPopup("OPTIONS");
				showOptions = false;
			}

			if (ImGui::BeginPopupModal("OPTIONS", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImGui::TextColored(ImVec4(0.9f, 0.6f, 0.3f, 1.0f), isSpanish ? "Ajustes del juego" : "Game Settings");
				ImGui::Separator();

				if (ImGui::Button(isSpanish ? "Salir del juego" : "Exit the game")) {
					exit(0);
				}

				ImGui::Spacing();

				if (ImGui::Button(isSpanish ? "Cambiar idioma (Espanol / Ingles)" : "Change language (English / Spanish)")) {
					isSpanish = !isSpanish;
				}

				ImGui::Spacing();

				ImGui::Text(isSpanish ? "Brillo:" : "Brightness:");
				//ImGui::SliderFloat("##brillo", &brightness, 0.0f, 1.0f, "%.2f");
				ImGui::SliderFloat(isSpanish ? "Brillo" : "Brightness", &brightness, 0.0f, 1.0f);

				ImGui::Spacing();

				if (ImGui::Button(isSpanish ? "Cerrar" : "Close")) {
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

			static bool showCredits = false;

			ImGui::SetCursorPos(ImVec2(centerX, 280));
			if (ImGui::Button(isSpanish ? "Creditos" : "Credits", ImVec2(buttonWidth, buttonHeight))) {
				showCredits = true;
			}

			// popup "credits"
			if (showCredits) {
				ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
				ImGui::OpenPopup(isSpanish ? "Creditos" : "Credits");
				showCredits = false;
			}

			if (ImGui::BeginPopupModal(isSpanish ? "Creditos" : "Credits", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImGui::TextColored(ImVec4(0.8f, 0.6f, 1.0f, 1.0f), isSpanish ? "Desarrollado por: " : "Developed by:");
				ImGui::Separator();
				ImGui::Text(" - Avalos Jasmin");
				ImGui::Text(" - Urbina Ashley");
				ImGui::Text(" - Briceno Sharon");
				ImGui::Text(" - Martinez Rebeca");


				ImGui::Spacing();
				ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), isSpanish ? "Grupo de Clase: 3T1-COM-S" : "Class group: 3T1-COM-S");

				ImGui::Spacing();
				if (ImGui::Button(isSpanish ? "Close" : "Cerrar")) {
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

			ImGui::End();

		}

		//GAME MOOD
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.Inputs(window);
		camera.updateMatrix(45.0f, 0.1f, 100.0f);

		//Skybox 

		glDepthFunc(GL_LEQUAL);
		skyboxShader.Activate();

		glUniform1f(glGetUniformLocation(skyboxShader.ID, "brightness"), brightness);

		glm::mat4 view = glm::mat4(glm::mat3(glm::lookAt(camera.Position, camera.Position + camera.Orientation, camera.Up)));
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);


		ImGui::PopStyleColor();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}


	//We delete all the objects we created

	shaderProgram.Delete();

	skyboxShader.Delete();

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;

}