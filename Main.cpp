//

#define SDL_MAIN_HANDLED
#include<filesystem>
namespace fs = std::filesystem;
//------------------------------

#include <SDL2/SDL.h>
//WE start including the libraries we will use
#define GLM_ENABLE_EXPERIMENTAL

#include <assimp/Importer.hpp>
#include<assimp/scene.h>
#include <assimp/postprocess.h>
#include"Model.h"    
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <chrono>
#include "Camera.h"

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


#pragma region AUDIO CONFIGURATIONS PT.1 BEGIN

Uint8* wavBuffer = nullptr;
Uint32 wavLength = 0;
Uint32 wavPosition = 0;
SDL_AudioDeviceID device = 0;

Uint8* clickBuffer = nullptr;
Uint32 clickLength = 0;
SDL_AudioDeviceID clickDevice = 0;


//Functions

void audioCallback(void* userdata, Uint8* stream, int len) {
	Uint32 remaining = wavLength - wavPosition;
	Uint32 toCopy = (remaining > (Uint32)len) ? (Uint32)len : remaining;

	if (toCopy > 0) {
		SDL_memcpy(stream, wavBuffer + wavPosition, toCopy);
		wavPosition += toCopy;
	}
	if (toCopy < (Uint32)len) {
		SDL_memset(stream + toCopy, 0, len - toCopy);
	}
}

void playAdventureSound() {
	//restart audio playback
	wavPosition = 0;
	//Cleans the audio queue and enqueues the audio data for playback
	SDL_ClearQueuedAudio(device);
	SDL_QueueAudio(device, wavBuffer, wavLength);
	//Play the audio device
	SDL_PauseAudioDevice(device, 0);
}

void stopAventureSound() {
	SDL_ClearQueuedAudio(device);
	SDL_PauseAudioDevice(device, 1);
}
void playClickSound() {
	SDL_ClearQueuedAudio(clickDevice);
	SDL_QueueAudio(clickDevice, clickBuffer, clickLength);
	SDL_PauseAudioDevice(clickDevice, 0);
}

Uint8* startBuffer = nullptr;
Uint32 startLength = 0;
SDL_AudioDeviceID startDevice = 0;

void starMenuSound() {
	SDL_ClearQueuedAudio(startDevice);
	SDL_QueueAudio(startDevice, startBuffer, startLength);
	SDL_PauseAudioDevice(startDevice, 0);
}
void stopMenuSound() {
	SDL_ClearQueuedAudio(startDevice);
	SDL_PauseAudioDevice(startDevice, 1);
}

Uint8* gameOverBuffer = nullptr;
Uint32 gameOverLength = 0;
SDL_AudioDeviceID gameOverDevice = 0;

void gameOverSound() {
	SDL_ClearQueuedAudio(gameOverDevice);
	SDL_PauseAudioDevice(gameOverDevice, 1);
}

void playGameOverSound() {
	SDL_ClearQueuedAudio(gameOverDevice);
	SDL_QueueAudio(gameOverDevice, gameOverBuffer, gameOverLength);
	SDL_PauseAudioDevice(gameOverDevice, 0);
}
void stopGameOverSound() {
	SDL_ClearQueuedAudio(gameOverDevice);
	SDL_PauseAudioDevice(gameOverDevice, 1);
}

#pragma endregion 
//END PT.1

//Variable to know if the game was paused
bool gameisPaused = false;
bool colision = false;
//Method to create the start menu
//Variables we´ll use
bool shouldExit = false;
static bool confirmExit = false;


#pragma region WINDOWS
void showMainMenu(GLFWwindow* window, bool& gameMood, bool& menuSoundPlayed, bool& isSpanish, float& brightness)
{

	if (shouldExit) {

		glfwDestroyWindow(window);
		glfwTerminate();
		exit(0);
	}

	if (!menuSoundPlayed) {
		starMenuSound();
		menuSoundPlayed = true;
	}
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

	//We get the window size
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	glViewport(0, 0, mode->width, mode->height);

	width = mode->width;
	height = mode->height;

	// main menu
	ImVec2 menuSize(600, 430);
	ImVec2 menuPos((width - menuSize.x) / 2, (height - menuSize.y) / 2);
	ImGui::SetNextWindowSize(menuSize);
	ImGui::SetNextWindowPos(menuPos);

	//set the color of the menu buttons 
	ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(0.843f, 0.502f, 1.0f, 1.0f);  // #d780ff
	ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = ImVec4(0.9f, 0.6f, 1.0f, 1.0f);
	ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = ImVec4(0.7f, 0.3f, 1.0f, 1.0f);


	// menu background color
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.95f));


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
		playClickSound();
		gameMood = true;

		//restartGame(camera, personajePos, modelos, gameIsOver, showGameOverWindowFlag, finalTime, gameMood);

		gameisPaused = false;
		colision = false;



		if (gameMood) {
			playAdventureSound();
		}
	}

	// button "About the game"
	ImGui::SetCursorPos(ImVec2(centerX, 160));
	static bool showAbout = false;
	if (ImGui::Button(isSpanish ? "Acerca del juego" : "About the game", ImVec2(buttonWidth, buttonHeight))) {
		playClickSound();
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
				"  - D para ir a la derecha\n");
		}
		else {
			ImGui::Text("SkyPlane3D is an exciting third-person game where you control a plane\n"
				"soaring above a city filled with obstacles.\n\n"
				"To maneuver through the skies, use the following keys:\n"
				"  - W to ascend\n"
				"  - S to descend\n"
				"  - A to go left\n"
				"  - D to go right\n");
		}

		if (ImGui::Button(isSpanish ? "Cerrar" : "Close")) {
			playClickSound();
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();

	}

	static bool showOptions = false;

	ImGui::SetCursorPos(ImVec2(centerX, 220));
	if (ImGui::Button(isSpanish ? "Opciones" : "Options", ImVec2(buttonWidth, buttonHeight))) {
		playClickSound();
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
			playClickSound();
			exit(0);
		}

		ImGui::Spacing();

		if (ImGui::Button(isSpanish ? "Cambiar idioma (Espanol / Ingles)" : "Change language (English / Spanish)")) {
			playClickSound();
			isSpanish = !isSpanish;
		}

		ImGui::Spacing();

		ImGui::Text(isSpanish ? "Brillo:" : "Brightness:");
		//ImGui::SliderFloat("##brillo", &brightness, 0.0f, 1.0f, "%.2f");
		ImGui::SliderFloat(isSpanish ? "Brillo" : "Brightness", &brightness, 0.0f, 1.0f);

		ImGui::Spacing();

		if (ImGui::Button(isSpanish ? "Cerrar" : "Close")) {
			playClickSound();
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();


	}

	static bool showCredits = false;

	ImGui::SetCursorPos(ImVec2(centerX, 280));
	if (ImGui::Button(isSpanish ? "Creditos" : "Credits", ImVec2(buttonWidth, buttonHeight))) {
		playClickSound();
		showCredits = true;
	}

	// popup "credits"
	if (showCredits) {
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::OpenPopup(isSpanish ? "Creditos" : "Credits");
		playClickSound();
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
			playClickSound();
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	ImGui::SetCursorPos(ImVec2(centerX, 340));

	if (ImGui::Button(isSpanish ? "Salir" : "Exit", ImVec2(buttonWidth, buttonHeight))) {
		playClickSound();
		confirmExit = true;
	}

	//Pop up comfirming the exit
	if (confirmExit) {
		ImGui::OpenPopup(isSpanish ? "Confirmar salida" : "Confirm Exit");
		confirmExit = false;
	}

	if (ImGui::BeginPopupModal(isSpanish ? "Confirmar salida" : "Confirm Exit", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text(isSpanish ? "Seguro que quieres salir del juego?" : "Are you sure you want to exit the game?");
		ImGui::Separator();

		if (ImGui::Button(isSpanish ? "Si" : "Yes", ImVec2(120, 0))) {
			playClickSound();
			stopAventureSound();
			stopMenuSound();
			shouldExit = true;
		}
		ImGui::SameLine();
		if (ImGui::Button(isSpanish ? "No" : "No", ImVec2(120, 0))) {
			playClickSound();
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::End();
	ImGui::PopStyleColor();

}


void showGameOverWindow(bool isSpanish, bool& showGameOverWindowFlag, bool& restartRequested, bool& exitToMenuRequested, int score) {
	if (!showGameOverWindowFlag) return;

	ImGuiIO& io = ImGui::GetIO();
	ImVec2 center = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
	ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Always);

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(30, 30));

	
	if (!ImGui::IsPopupOpen("GameOverPopup"))
		ImGui::OpenPopup("GameOverPopup");

	if (ImGui::BeginPopupModal("GameOverPopup", nullptr,
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar))
	{
		ImGui::Spacing();
		ImGui::Spacing();

		float titleWidth = ImGui::CalcTextSize(isSpanish ? "GAME OVER!" : "GAME OVER!").x;
		float posX = (ImGui::GetWindowSize().x - titleWidth) * 0.5f;

		//Centered text
		ImGui::SetCursorPosX(posX);
		ImGui::PushFont(io.Fonts->Fonts[0]);
		ImGui::Text(isSpanish ? "GAME OVER!" : "GAME OVER!");
		ImGui::PopFont();

		
		ImGui::SetCursorPosX(posX);
		ImGui::Text("%s: %d", isSpanish ? "Puntuacion" : "Score", score);

		ImGui::Spacing();
		ImGui::Spacing();


		float buttonWidth = 180;
		float buttonSpacing = 20;
		float totalWidth = (buttonWidth * 2) + buttonSpacing;
		ImGui::SetCursorPosX((ImGui::GetWindowSize().x - totalWidth) * 0.5f);

		if (ImGui::Button(isSpanish ? "Volver a jugar" : "Play Again", ImVec2(buttonWidth, 0)))
		{
			restartRequested = true;
			showGameOverWindowFlag = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine(0, buttonSpacing);

		if (ImGui::Button(isSpanish ? "Salir" : "Exit", ImVec2(buttonWidth, 0)))
		{
			exitToMenuRequested = true;
			showGameOverWindowFlag = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	ImGui::PopStyleVar();
	ImGui::PopStyleColor(2);
}

#pragma endregion

//Variables for the chronometer
auto startTime = std::chrono::high_resolution_clock::now();
auto pauseStart = startTime;
std::chrono::duration<double> pausedDuration(0);
std::chrono::duration<double> accumulatedTime(0);

bool gameOverSoundPlayed = false;

bool hayColision(glm::vec3 a, glm::vec3 b, float distanciaMinima)
{
	return glm::distance(a, b) < distanciaMinima;
}

//function for restarting the game
void restartGame(Camera& camera, glm::vec3& posAvion, std::vector<Model>& modelos,
                 bool& gameIsOver, bool& showGameOverWindowFlag,
                 std::chrono::duration<double>& finalTime, bool& gameMood)
{
	gameMood = true;
	gameIsOver = false;
	showGameOverWindowFlag = false;
	gameisPaused = false;
	gameOverSoundPlayed = false;
	bool colision = false;

	// Reiniciar posición de la cámara
	camera.Position = glm::vec3(0.0f, 0.0f, 2.0f);

	// Reiniciar posición del avión respecto a la cámara
	posAvion = glm::vec3(camera.Position.x, camera.Position.y - 0.5f, camera.Position.z - 5.0f);

	// Limpiar obstáculos y poner uno inicial
	modelos.clear();
	glm::vec3 firstPos = glm::vec3(0.0f, 0.0f, -60.0f);
	glm::mat4 firstTransform = glm::translate(glm::mat4(1.0f), firstPos);
	firstTransform = glm::scale(firstTransform, glm::vec3(0.5f));
	modelos.push_back(Model("Resources/Models/anillo/scene.gltf", firstTransform));

	// Reiniciar temporizadores
	startTime = std::chrono::high_resolution_clock::now();
	pauseStart = startTime;
	pausedDuration = std::chrono::duration<double>::zero();
	finalTime = std::chrono::duration<double>::zero();

	// Reiniciar música
	stopGameOverSound();
	playAdventureSound();
}




static bool gameIsOver = false;


int main(int argc, char* argv[])
{


#pragma region AUDIO CONFIGURATIONS PT.2 BEGIN

	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		std::cerr << "Error SDL_Init: " << SDL_GetError() << "\n";
		return -1;
	}

	SDL_AudioSpec wavSpec;
	SDL_AudioSpec obtainedSpec;

	if (!SDL_LoadWAV("assets/adventure.wav", &wavSpec, &wavBuffer, &wavLength))
	{
		std::cerr << "Error SDL_LoadWAV: " << SDL_GetError() << "\n";
		SDL_Quit();
		return -1;
	}

	device = SDL_OpenAudioDevice(nullptr, 0, &wavSpec, &obtainedSpec, 0);

	if (device == 0) {
		std::cerr << "Error SDL_OpenAudioDevice: " << SDL_GetError() << "\n";
		SDL_free(wavBuffer);
		SDL_Quit();
		return -1;
	}

	//Start paused
	SDL_PauseAudioDevice(device, 1);

	SDL_AudioSpec clickSpec;

	//REPLACE WITH YOUR PATH (MESSAGE TO THE TEAM)

	if (!SDL_LoadWAV("assets/mixkit-quick-win-video-game-notification-269.wav", &clickSpec, &clickBuffer, &clickLength)) {
		std::cerr << "Error SDL_LoadWAV: " << SDL_GetError() << "\n";
	}
	clickDevice = SDL_OpenAudioDevice(nullptr, 0, &clickSpec, nullptr, 0);

	//Start menu sound config.

	SDL_AudioSpec startSpec;

	if (!SDL_LoadWAV("assets/BeginMenu.wav", &startSpec, &startBuffer, &startLength)) {
		std::cerr << "Error SDL_LoadWAV (start): " << SDL_GetError() << "\n";
	}
	startDevice = SDL_OpenAudioDevice(nullptr, 0, &startSpec, nullptr, 0);


	//Game over sound
	SDL_AudioSpec gameOverspec;
	if (!SDL_LoadWAV("assets/GameOver.wav", &gameOverspec, &gameOverBuffer, &gameOverLength)) {
		std::cerr << "Error SDL_LoadWAV: " << SDL_GetError() << "\n";
	}

	gameOverDevice = SDL_OpenAudioDevice(nullptr, 0, &gameOverspec, nullptr, 0);

#pragma endregion AUDIO PT.2 END

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

	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

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
	std::string facesCubemap[6] = {
		"Resources/right.png",
		"Resources/left.png",
		"Resources/top.png",
		"Resources/bottom.png",
		"Resources/front.png",
		"Resources/back.png"
	};

	float modelSpacing = 40.0f;   // Ahora la separación es 40 unidades en Z
	float triggerDistance = 20.0f; // Solo se genera un nuevo modelo cada 20 unidades que avanza la cámara

	// Load in models
	Model personaje("Resources/Models/airplane/scene.gltf");

	// Carga de obstaculos
	std::vector<Model> modelos;
	glm::vec3 firstPos = glm::vec3(camera.Position.x, camera.Position.y, camera.Position.z - modelSpacing);
	glm::mat4 firstTransform = glm::translate(glm::mat4(1.0f), firstPos);
	firstTransform = glm::scale(firstTransform, glm::vec3(0.5f));

	modelos.push_back(Model("Resources/Models/anillo/scene.gltf", firstTransform));

	//float lastTriggerZ = 2.0f;     // Última posición en Z donde se generó un modelo
	float lastTriggerZ = camera.Position.z - triggerDistance;

	int cycleState = 0;

	float lastTriggerTime = 0.0f;
	float spawnInterval = 2.0f; // cada 2 segundos aparece un modelo nuevo
	float lastFrame = glfwGetTime();  // CORRECTO
	float deltaTime = 0.0f;




	//Variables to create periodic event for FPS displaying
	double prevTime = 0.0;
	double crntTime = 0.0;
	//double timeDiff;

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
	bool gamePaused = false;


	//Time played
	int accumulatedSeconds = 0;
	auto lastUpdateTime = startTime;

	//Variable to know if the menu sound was played
	bool menuSoundPlayed = false;

	//TEST
	// Carga de obstaculos


	float lastFrameTime = glfwGetTime();
	
	std::vector<glm::vec3> obstaculos = {
	        glm::vec3(0.0f, 0.0f, -60.0f),
			glm::vec3(5.0f, 2.0f, -90.0f),
			glm::vec3(-5.0f, -2.0f, -120.0f)
	};

	bool showGameOverWindowFlag = false;
	bool restartRequested = false;
	bool exitToMenuRequested = false;
	bool gameIsOver = false; 

	std::chrono::duration<double> finalTime = std::chrono::duration<double>::zero();
	static glm::vec3 posAvion = glm::vec3(0.0f, 0.0f, -50.0f);
	
	while (!glfwWindowShouldClose(window))
	{

		float currentFrameTime = glfwGetTime();
		float deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;
		glm::mat4 personajeTransform = glm::mat4(1.0f);
		glfwPollEvents();
		// ImGui Frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//GAME MOOD
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Configuring the chronometer that will appear while playing

		if (gameMood) {
			
			// Dibujar el personaje principal
			
			glm::vec3 personajePos = glm::vec3(camera.Position.x, camera.Position.y - 0.5f, camera.Position.z - 5.0f);
			personajeTransform = glm::translate(personajeTransform, personajePos);
			personajeTransform = glm::rotate(personajeTransform, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			personajeTransform = glm::scale(personajeTransform, glm::vec3(0.5f));

			if (gameMood && !gameisPaused)
			{
				for (auto& model : modelos)
				{
					if (hayColision(personajePos, model.getPosition(), 1.0f))  // puedes ajustar el valor
					{
						colision = true;
						gameIsOver = true;
						showGameOverWindowFlag = true;
						gameMood = false;

						stopAventureSound();
						if (!gameOverSoundPlayed)
						{
							playGameOverSound();
							gameOverSoundPlayed = true;
						}


						finalTime = std::chrono::high_resolution_clock::now() - startTime - pausedDuration;
						break;
					}
				}

				if (!colision)
				{
					camera.Inputs(window, deltaTime, personajePos);
				}

			}

			

			//Pause bottom start 
			// Our pause bottom is the M key
		  //Static variable to keep track of the previous state of the key
			static bool m_wasPressed = false;

			//We get the current state of the key
			bool m_pressed = glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS;

			//Only toggle the pause state if the key was just pressed 

			if (m_pressed && !m_wasPressed) {
				gameisPaused = !gameisPaused;
				playClickSound();

				if (gameisPaused) {
					//We register the time when the game was paused
					pauseStart = std::chrono::high_resolution_clock::now();
				}
				else {
					auto now = std::chrono::high_resolution_clock::now();
					pausedDuration += now - pauseStart;
				}
			}

			//Update the previous state of the key
			m_wasPressed = m_pressed;
			// Get current time point
			auto now = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> effectiveDuration;



#pragma region GAME OVER LOGIC

			//HERE WE WILL ENABLE IN THE MEANTIME THE MENU OF GAME OVER, it works when you lose

		   //static bool to control the window
		


	
			//imgui render
			int score = static_cast<int>(finalTime.count());
			showGameOverWindow(isSpanish, showGameOverWindowFlag, restartRequested, exitToMenuRequested, score);


			if (restartRequested) {
				playClickSound();
				restartGame(camera, personajePos, modelos, gameIsOver, showGameOverWindowFlag, finalTime, gameMood);

				restartRequested = false;
				gameMood = true;
			}


			if (exitToMenuRequested) {
				playClickSound();
				showGameOverWindowFlag = false;
				exitToMenuRequested = false;
				gameMood = false;  //Coming back to the start menu
				stopAventureSound();
				starMenuSound();
			}

			//END OF THE GAME OVER LOGIC

#pragma endregion

			if (gameIsOver) {

				effectiveDuration = finalTime;
			}
			else if (gameisPaused) {
				effectiveDuration = pauseStart - startTime - pausedDuration;
			}
			else {
				effectiveDuration = now - startTime - pausedDuration;
			}


			int total_seconds = static_cast<int>(effectiveDuration.count());
			int minutes = total_seconds / 60;
			int seconds = total_seconds % 60;

			ImGui::SetNextWindowPos(ImVec2(10, 10));
			ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_Always);
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(186, 150, 211, 255));

			ImGui::Begin("Chronometer", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

			ImGui::Text("Time: %02d:%02d", minutes, seconds);
			ImGui::PopStyleColor();

			ImGui::End();

			//end configuration of the chronometer
			// 
			//Menu that appears when the game is paused

			if (gameisPaused)
			{
				//tarMenuSound();

				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				ImVec2 pauseMenuSize(500, 350);
				ImVec2 pauseMenuPos((width - pauseMenuSize.x) / 2, (height - pauseMenuSize.y) / 2);
				ImGui::SetNextWindowSize(pauseMenuSize);
				ImGui::SetNextWindowPos(pauseMenuPos);
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.95f));


				ImGui::Begin("Pause Menu", nullptr,
					ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_NoCollapse |
					ImGuiWindowFlags_NoSavedSettings |
					ImGuiWindowFlags_NoTitleBar
				);

				ImGui::SetCursorPosY(20);
				ImGui::SetCursorPosX((pauseMenuSize.x - ImGui::CalcTextSize("Juego en pausa").x) / 2);
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.7f, 1.0f), isSpanish ? "Juego en pausa" : "Game Paused");
				ImGui::Separator();

				float buttonWidth = 260.0f;
				float buttonHeight = 40.0f;
				float centerX = (pauseMenuSize.x - buttonWidth) / 2;

				ImGui::SetCursorPos(ImVec2(centerX, 100));
				if (ImGui::Button(isSpanish ? "Continuar" : "Resume", ImVec2(buttonWidth, buttonHeight))) {
					playClickSound();
					//Get current time point
					auto now = std::chrono::high_resolution_clock::now();
					pausedDuration += now - pauseStart;
					gameisPaused = false;
					colision = false;
				}


				ImGui::SetCursorPos(ImVec2(centerX, 160));
				if (ImGui::Button(isSpanish ? "Reiniciar" : "Restart", ImVec2(buttonWidth, buttonHeight))) {
					playClickSound();
					restartGame(camera, personajePos, modelos, gameIsOver, showGameOverWindowFlag, finalTime, gameMood);

					gameisPaused = false;
					colision = false;
				}


				static bool confirmExitPauseMenu = false;

				ImGui::SetCursorPos(ImVec2(centerX, 220));

				if (ImGui::Button(isSpanish ? "Salir de la partida" : "End the game", ImVec2(buttonWidth, buttonHeight))) {
					playClickSound();
					confirmExitPauseMenu = true;

				}//POpup confirming the exit
				if (confirmExitPauseMenu) {
					ImGui::OpenPopup(isSpanish ? "Confirmar salida" : "Confirm Exit");
					confirmExitPauseMenu = false;
				}

				if (ImGui::BeginPopupModal(isSpanish ? "Confirmar salida" : "Confirm Exit", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
					ImGui::Text(isSpanish ? "¿Seguro que quieres terminar la partida?" : "Are you sure you want to leave the game?");
					ImGui::Separator();

					if (ImGui::Button(isSpanish ? "Si" : "Yes", ImVec2(120, 0))) {
						playClickSound();
						gameMood = false;
						gameisPaused = false;
						colision = false;
						stopAventureSound();
						starMenuSound();
					}
					ImGui::SameLine();
					if (ImGui::Button(isSpanish ? "No" : "No", ImVec2(120, 0))) {
						playClickSound();
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}

				ImGui::End();
				ImGui::PopStyleColor();
			}
			//End configuration of the pause bottom
		}

		//else {

			//startTime = std::chrono::high_resolution_clock::now();
		//}


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



		//Allowing the camera to move only 
		// if we are in game mood

	
		
	
		//Showing the menu if we are not on game mood

		if (!gameMood)
		{
			showMainMenu(window, gameMood, menuSoundPlayed, isSpanish, brightness);
		}

		else
		{
			SDL_PauseAudioDevice(startDevice, 1);
			menuSoundPlayed = false;
		}

		//std::vector<Model> modelos;
		//GAME MOOD
		
		float camX = camera.Position.x;
		float camY = camera.Position.y;
		float camZ = camera.Position.z;

		if (lastTriggerZ - camZ >= triggerDistance)
		{
			lastTriggerZ = camZ;

			// La posición X y Y del modelo va a ser la misma que la cámara
			float xOffset = camX;
			float yOffset = camY;


			glm::vec3 newPos = glm::vec3(xOffset, yOffset, camZ - modelSpacing);
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), newPos);
			transform = glm::scale(transform, glm::vec3(0.5f));

			modelos.push_back(Model("Resources/Models/anillo/scene.gltf", transform));

			if (modelos.size() > 4)
			{
				modelos.erase(modelos.begin(), modelos.begin() + 2);
			}

		}

		camera.updateMatrix(50.0f, 0.1f, 300.0f);

		// Draw the normal model
		personaje.Draw(shaderProgram, camera, personajeTransform);

		for (auto& model : modelos)
		{
			model.Draw(shaderProgram, camera);
		}

		//glEnable(GL_DEPTH_TEST);
		//glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//camera.Inputs(window);
		//camera.updateMatrix(45.0f, 0.1f, 100.0f);

		


		//for (auto& model : modelos)
		//{
			//modelAnillo.Draw(shaderProgram, camera, anilloTransform);
		//}

		//abandonedBuildingModel.Draw(shaderProgram, camera);
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
	SDL_CloseAudioDevice(device);
	SDL_CloseAudioDevice(clickDevice);
	SDL_FreeWAV(wavBuffer);
	SDL_FreeWAV(clickBuffer);
	SDL_Quit();

	shaderProgram.Delete();

	skyboxShader.Delete();

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;

}