#pragma once
#include <FModManager.h>
#include "cProjectManager.h"
#include "cLuaBrain.h"

#ifdef _DEBUG
#define DEBUG_LOG_ENABLED
#endif
#ifdef DEBUG_LOG_ENABLED
#define DEBUG_PRINT(x, ...) printf(x, __VA_ARGS__)
#else
#define DEBUG_PRINT(x)
#endif

#define SOUND_FILE "media_proj_2_sounds.xml"

extern cProjectManager* g_ProjectManager;
extern FModManager* g_FModManager;
extern GLFWwindow* window;
cLuaBrain* pBrain = new cLuaBrain();

void projectStartingUp();
void projectNewGame();
void projectRunning();
void projectShutdown();
bool attachSoundToMesh(std::string meshName, std::string soundName);

void ProjectGameLoop() {
	switch (g_ProjectManager->m_GameLoopState) {
	case GameState::STARTING_UP:
		projectStartingUp();
		break;
	case GameState::NEW_GAME:
		projectNewGame();
		break;
	case GameState::RUNNING:
		projectRunning();
		break;
	case GameState::SHUTING_DOWN:
		projectShutdown();
		break;
	}
}

void projectStartingUp() {
	// Load all sounds from the XML File
	g_FModManager->loadSoundsFromFile(SOUND_FILE);
	// Attaching sounds to meshes
	attachSoundToMesh("Dragon Jaw",			"Devil");
	attachSoundToMesh("Dragon Rib",			"Devil");
	attachSoundToMesh("Dragon Tail",		"Devil");
	attachSoundToMesh("Brazier 01",			"Fire");
	attachSoundToMesh("Brazier 02",			"Resonance");
	attachSoundToMesh("Barrel",				"Drips");
	attachSoundToMesh("Window Wall",		"Wind");
	attachSoundToMesh("Wall T1 03",			"Wind");
	attachSoundToMesh("Light Pillar 01",	"Starlight");
	attachSoundToMesh("Light Pillar 02",	"Darkness");

	// Pass the address of (aka "a pointer to") 
	//	the vector of game object pointers
	// (i.e. it's NOT a copy of the vector)
	pBrain->SetObjectVector(&g_ProjectManager->m_selectedScene->m_mMeshes);

	cMeshObject* theBall = g_ProjectManager->m_selectedScene->m_mMeshes.find("Ball1")->second;

	// ID = 41
	int ballID = theBall->getID();
	DEBUG_PRINT("Char ID = %d \n", ballID);

	std::string myScriptText = "setObjectState(" + std::to_string(ballID) + ", " + 
		std::to_string(theBall->m_position.x) + ", " + 
		std::to_string(theBall->m_position.y) + ", " + 
		std::to_string(theBall->m_position.z) + ", " +		// Position
		" 0, 0, 0 " +		// Velocity
		")";
	pBrain->RunScriptImmediately(myScriptText);

	pBrain->RunScriptImmediately(" offsetx = 0.0 ");
	pBrain->RunScriptImmediately(" offsety = 0.0 ");
	pBrain->RunScriptImmediately(" offsetz = 0.0 ");

	std::string moveScriptDeltaTimeFUNCTION =
		"function moveObject( objectID )\n"							\
		"	isValid, x, y, z, vx, vy, vz = getObjectState(" + std::to_string(ballID) + ") \n"	\
		"	if isValid then \n"										\
		"		x = x + offsetx	\n"						\
		"		y = y + offsety	\n"						\
		"		z = z + offsetz	\n"						\
		"		offsetx = 0.0	\n"						\
		"		offsety	= 0.0	\n"						\
		"		offsetz	= 0.0	\n"						\
		"		setObjectState( " + std::to_string(ballID) + ", x, y, z, vx, vy, vz )	\n"		\
		"	end	\n"													\
		"end";
	pBrain->RunScriptImmediately(moveScriptDeltaTimeFUNCTION);

	// This is like line 102, but is calling the Lua moveObject() funtion instead
	pBrain->LoadScript("key_callback", "moveObject(" + std::to_string(ballID) + ")");

	g_ProjectManager->m_GameLoopState = GameState::RUNNING;
}

void projectNewGame() {
	g_ProjectManager->m_selectedScene->m_cameraEye = glm::vec3(0.0f, 10.0f, -46.0f);
	g_ProjectManager->m_selectedScene->m_cameraTarget = glm::vec3(0.0f, 5.0f, 0.0f);
	g_ProjectManager->m_selectedScene->m_mMeshes.find("Ball1")->second->m_position = glm::vec3(0.0f, 0.0f, -50.0f);
	g_ProjectManager->m_GameLoopState = GameState::RUNNING;
}

void projectRunning(){
	cMeshObject* controllableChar = g_ProjectManager->m_selectedScene->m_mMeshes.find("Ball1")->second;
	
	// Update will run any Lua script sitting in the "brain"
	pBrain->Update(1.0);

	if(controllableChar != nullptr)
		g_FModManager->tick(controllableChar->m_position);
}

void projectShutdown(){
	glfwSetWindowShouldClose(window, true);
}


bool attachSoundToMesh(std::string meshName, std::string soundName) {
	// Sets the sound for the dragon bones mesh
	cMeshObject* meshObj;
	// Grabs the Dragon Rib mesh
	std::map<std::string, cMeshObject*>::iterator itMesh = g_ProjectManager->m_selectedScene->m_mMeshes.find(meshName);
	if (itMesh != g_ProjectManager->m_selectedScene->m_mMeshes.end()) {
		meshObj = itMesh->second;
	} else {
		DEBUG_PRINT("Mesh %s not found!", meshName.c_str());
		return false;
	}
	// Grabs the Sound
	Sound* pSound = g_FModManager->getSound(soundName);
	// Play the Sound
	g_FModManager->play3DSound(soundName, meshObj->m_position, 0.05f);
	// Attach the Sound Channel to the Mesh
	meshObj->attached_sound = pSound->m_channel;

	return true;
}