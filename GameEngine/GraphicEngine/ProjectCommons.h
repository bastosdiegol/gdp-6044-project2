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
#define SMALLEST_DISTANCE 3

extern cProjectManager* g_ProjectManager;
extern FModManager* g_FModManager;
extern GLFWwindow* window;
cLuaBrain* pBrain = new cLuaBrain();
bool g_isBallDuplicated = false;
bool g_isBallFollowingCurve = false;

void projectStartingUp();
void projectNewGame();
void projectRunning();
void projectShutdown();
bool attachSoundToMesh(std::string meshName, std::string soundName);
void followCurve(cMeshObject* origin, cMeshObject* destination, int dt);

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
	cMeshObject* theTail = g_ProjectManager->m_selectedScene->m_mMeshes.find("Dragon Tail")->second;

	// ID = 41
	int ballID = theBall->getID();
	int tailID = theTail->getID();
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
	pBrain->RunScriptImmediately(" time = 0.0 ");

	std::string moveScriptFUNCTION =
		"function moveObject( objectID )							\n"	\
		"	isValid, x, y, z, vx, vy, vz = getObjectState(objectID) \n"	\
		"	if isValid then											\n"	\
		"		x = x + offsetx										\n"	\
		"		y = y + offsety										\n"	\
		"		z = z + offsetz										\n"	\
		"		offsetx = 0.0										\n"	\
		"		offsety	= 0.0										\n"	\
		"		offsetz	= 0.0										\n"	\
		"		setObjectState( objectID , x, y, z, vx, vy, vz )	\n"	\
		"	end														\n"	\
		"end";
	pBrain->RunScriptImmediately(moveScriptFUNCTION);

	std::string rotateScriptFUNCTION =
		"function rotateObject( objectID, nrx, nry, nrz )						\n"	\
		"	isValid, x, y, z, vx, vy, vz, rx, ry, rz = getObjectState(objectID) \n"	\
		"	if isValid then														\n"	\
		"		rx = rx + nrx													\n"	\
		"		ry = ry + nry													\n"	\
		"		rz = rz + nrz													\n"	\
		"		setObjectState( objectID , x, y, z, vx, vy, vz, rx, ry, rz )	\n"	\
		"	end																	\n"	\
		"end";
	pBrain->RunScriptImmediately(rotateScriptFUNCTION);

	std::string faceScriptFUNCTION =
		"function faceObject( objectID, DestinationID )																\n"	\
		"	isValidObj, xObj, yObj, zObj, vxObj, vyObj, vzObj, xSpin, ySpin, zSpin = getObjectState( objectID )		\n"	\
		"	isValidDest, xDest, yDest, zDest, vxDest, vyDest, vzDest, rx, ry, rz = getObjectState( DestinationID )	\n"	\
		"	if isValidObj and isValidDest then																		\n"	\
		"		xDirection = xDest - xObj																			\n"	\
		"		yDirection = yDest - yObj																			\n"	\
		"		zDirection = zDest - zObj																			\n"	\
		"		rx = rx + xDirection																						\n"	\
		"		ry = ry + yDirection																						\n"	\
		"		rz = rz + zDirection																						\n"	\
		"		setObjectState( objectID , xObj, yObj, zObj, 0.0, 0.0, 0.0, rx, ry, rz )										\n"	\
		"	end																										\n"	\
		"end";
	pBrain->RunScriptImmediately(faceScriptFUNCTION);

	std::string getDirectionScriptFUNCTION =
		"function getDirection( objectID, DestinationID )					\n"	\
		"	isValidObj, xObj, yObj, zObj = getObjectState( objectID )		\n"	\
		"	isValidDest, xDest, yDest = getObjectState( DestinationID )		\n"	\
		"	if isValidObj and isValidDest then								\n"	\
		"		xDirection = xDest - xObj									\n"	\
		"		yDirection = yDest - yObj									\n"	\
		"		zDirection = zDest - zObj									\n"	\
		"		return xDirection, yDirection, zDirection					\n"	\
		"	end																\n"	\
		"end";
	pBrain->RunScriptImmediately(getDirectionScriptFUNCTION);

	std::string moveScriptTowardsDestinationFUNCTION =
		"function moveObjectTowardsDestination( objectID, DestinationID )											\n"	\
		"	isValidObj, xObj, yObj, zObj, vxObj, vyObj, vzObj, xSpin, ySpin, zSpin = getObjectState( objectID )		\n"	\
		"	isValidDest, xDest, yDest, zDest, vxDest, vyDest, vzDest, rx, ry, rz = getObjectState( DestinationID )	\n"	\
		"	if isValidObj and isValidDest then																		\n"	\
		"		xDirection = xDest - xObj																			\n"	\
		"		yDirection = yDest - yObj																			\n"	\
		"		zDirection = zDest - zObj																			\n"	\
		"		magnitude = (xDirection * xDirection + yDirection * yDirection + zDirection * zDirection)^0.5		\n"	\
		"		if magnitude > "+ std::to_string(SMALLEST_DISTANCE) + " then										\n"	\
		"			xStep = xDirection / magnitude / 5																\n"	\
		"			yStep = yDirection / magnitude / 5																\n"	\
		"			zStep = zDirection / magnitude / 5																\n"	\
		"			rotateObject ( objectID, 0.0, 0.1, 0.0 )														\n"	\
		"			x = x + xStep																					\n"	\
		"			y = y + yStep																					\n"	\
		"			z = z + zStep																					\n"	\
		"			setObjectState( objectID, x, y, z, vx, vy, vz, xSpin, ySpin, zSpin )							\n"	\
		"			return true																						\n"	\
		"		else																								\n"	\
		"			return false																					\n"	\
		"		end																									\n"	\
		"	end																										\n"	\
		"end";
	pBrain->RunScriptImmediately(moveScriptTowardsDestinationFUNCTION);

	std::string moveScriptTowardsPointFUNCTION =
		"function moveObjectTowardsPoint( objectID, px, py, pz )													\n"	\
		"	isValidObj, xObj, yObj, zObj, vxObj, vyObj, vzObj, xSpin, ySpin, zSpin   = getObjectState( objectID )												\n"	\
		"	if isValidObj then																						\n"	\
		"		xDirection = px - xObj																				\n"	\
		"		yDirection = py - yObj																				\n"	\
		"		zDirection = pz - zObj																				\n"	\
		"		magnitude = (xDirection * xDirection + yDirection * yDirection + zDirection * zDirection)^0.5		\n"	\
		"		if magnitude > " + std::to_string(SMALLEST_DISTANCE) + " then										\n"	\
		"			xStep = xDirection / magnitude / 5																\n"	\
		"			yStep = yDirection / magnitude / 5																\n"	\
		"			zStep = zDirection / magnitude / 5																\n"	\
		"			rotateObject ( objectID, 0.0, 0.1, 0.0 )														\n"	\
		"			x = x + xStep																					\n"	\
		"			y = y + yStep																					\n"	\
		"			z = z + zStep																					\n"	\
		"			setObjectState( objectID, x, y, z, vx, vy, vz, xSpin, ySpin, zSpin )							\n"	\
		"			return true																						\n"	\
		"		else																								\n"	\
		"			return false																					\n"	\
		"		end																									\n"	\
		"	end																										\n"	\
		"end";
	pBrain->RunScriptImmediately(moveScriptTowardsPointFUNCTION);

	//std::string followObjectScriptFUNCTION =
	//	"function followObject( objectID, objToFollowID, xOffset, yOffset, zOffset )\n"							\
	//	"	print(\"offsets x y z\", x, y, z) " \
	//	"	isValidObj, xObj, yObj, zObj, vxObj, vyObj, vzObj = getObjectState( objectID ) \n"	\
	//	"	isValidFollow, xFollow, yFollow, zFollow, vxFollow, vyFollow, vzFollow = getObjectState( objToFollowID ) \n"	\
	//	"	if isValidObj and isValidFollow then \n"										\
	//	"		x =  xFollow + xOffset	\n"						\
	//	"		y =  yFollow + yOffset	\n"						\
	//	"		z =  zFollow + zOffset	\n"						\
	//	"		setObjectState( objectID, x, y, z, vx, vy, vz )	\n"		\
	//	"	end	\n"													\
	//	"end";
	//pBrain->RunScriptImmediately(followObjectScriptFUNCTION);

	std::string followObjectScriptFUNCTION =
		"function followObject( objectID, objToFollowID, xOffset, yOffset, zOffset )									\n"	\
		"	isValidObj, xObj, yObj, zObj, vxObj, vyObj, vzObj = getObjectState( objectID )								\n"	\
		"	isValidFollow, xFollow, yFollow, zFollow, vxFollow, vyFollow, vzFollow = getObjectState( objToFollowID )	\n"	\
		"	if isValidObj and isValidFollow then																		\n" \
		"		xDirection = xFollow - xObj																				\n"	\
		"		yDirection = yFollow - yObj																				\n"	\
		"		zDirection = zFollow - zObj																				\n"	\
		"		magnitude = (xDirection * xDirection + yDirection * yDirection + zDirection * zDirection) ^ 0.5			\n"	\
		"																												\n" \
		"		if magnitude > "+ std::to_string(SMALLEST_DISTANCE) + " then										\n"	\
		"			flag = moveObjectTowardsDestination( objectID , objToFollowID)										\n" \
		"		end																										\n"	\
		"	end																											\n"	\
		"end";
	pBrain->RunScriptImmediately(followObjectScriptFUNCTION);

	std::string followCurveScriptTowardsDestinationFUNCTION =
		"function moveObjectTowardsDestinationCurved( objectID, oriX, oriY, oriZ, DestinationID, controlX, controlY, controlZ, dt )		\n"	\
		"	isValidObj, xObj, yObj, zObj = getObjectState( objectID )												\n"	\
		"	isValidDest, xDest, yDest, zDest = getObjectState( DestinationID )										\n"	\
		"	if isValidObj and isValidDest then																		\n"	\
		"		print(\"objectID x y z \", objectID, xObj, yObj, zObj)												\n"	\
		"		print(\"DestinationID x y z \", DestinationID, xDest, yDest, zDest)									\n"	\
		"		xDirection = xDest - xObj																			\n"	\
		"		yDirection = yDest - yObj																			\n"	\
		"		zDirection = zDest - zObj																			\n"	\
		"		magnitude = (xDirection * xDirection + yDirection * yDirection + zDirection * zDirection) ^ 0.5		\n"	\
		"		if magnitude > " + std::to_string(SMALLEST_DISTANCE) + " then										\n"	\
		"			print(\"Time \", time)																			\n" \
		"			p1x = oriX + (controlX - oriX) * time															\n"	\
		"			p1y = oriY + (controlY - oriY) * time															\n"	\
		"			p1z = oriZ + (controlZ - oriZ) * time															\n"	\
		"			p2x = controlX + (xDest - controlX) * time														\n"	\
		"			p2y = controlY + (yDest - controlY) * time														\n"	\
		"			p2z = controlZ + (zDest - controlZ) * time														\n"	\
		"			x = p1x + (p2x - p1x) * time																	\n"	\
		"			y = p1y + (p2y - p1y) * time																	\n"	\
		"			z = p1z + (p2z - p1z) * time																	\n"	\
		"			setObjectState( objectID, x, y, z)														\n"	\
		"			print(\"Positions x y z\", x, y, z)																\n" \
		"			time = time + dt																				\n" \
		"		end																									\n"	\
		"	end																										\n"	\
		"end";
	pBrain->RunScriptImmediately(followCurveScriptTowardsDestinationFUNCTION);

	// This is like line 102, but is calling the Lua moveObject() funtion instead
	pBrain->LoadScript("key_callback", "moveObject(" + std::to_string(ballID) + ")");
	pBrain->LoadScript("move_towards", "moveObjectTowardsDestination(" + std::to_string(ballID) + ", " + std::to_string(tailID) + ")");

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
	pBrain->Update(0.5);

	cMeshObject* theBall = g_ProjectManager->m_selectedScene->m_mMeshes.find("Ball1")->second;
	cMeshObject* theTail = g_ProjectManager->m_selectedScene->m_mMeshes.find("Dragon Tail")->second;
	cMeshObject* theBrazier = g_ProjectManager->m_selectedScene->m_mMeshes.find("Brazier 01")->second;

	// Checks if the Ball arrived at the Tail
	if (glm::distance(theBall->m_position, theTail->m_position) < SMALLEST_DISTANCE) {
		// Stops the active moving toward script
		pBrain->DeleteScript("move_towards");
		// Checks if it was duplicated before
		if (g_isBallDuplicated == false) {
			cMeshObject* newBall = new cMeshObject(theBall->m_parentModel);
			// Stores temporarlly the new ball id
			unsigned int tempID = newBall->getID();
			// Copies the Ball to new Ball
			*newBall = *theBall;
			// Overwrite the copied Id with the original
			newBall->m_myID = tempID;
			// Gives it a new name
			newBall->m_meshName = "Ball2";
			// Shift the new Ball to the right side
			newBall->m_position += glm::vec3(-10.0f, 0.0f, 0.0f);
			// Set Light Red Colour
			newBall->m_RGBA_colour = glm::vec4(0.862745f, 0.078431f, 0.235294f, 0.0f);
			// Adds the new Ball to the mesh vec
			g_ProjectManager->m_selectedScene->m_mMeshes.try_emplace(newBall->m_meshName, newBall);
			// Set this Boolean to avoid another duplication
			g_isBallDuplicated = true;
			// Lets calculate the x y z offset between red ball and green ball
			float xOffset =  newBall->m_position.x - theBall->m_position.x;
			float yOffset =  newBall->m_position.y - theBall->m_position.y;
			float zOffset =  newBall->m_position.z - theBall->m_position.z;
			// Now we set the red ball to follow the green ball
			pBrain->LoadScript("follow_green", "followObject(" + std::to_string(newBall->getID()) + ", " + std::to_string(theBall->getID()) + ", " \
															   + std::to_string(xOffset) + ", " \
															   + std::to_string(yOffset) + ", " \
															   + std::to_string(zOffset) + ")");
			pBrain->LoadScript("face_green", "faceObject(" + std::to_string(newBall->getID()) + ", " + std::to_string(theBall->getID()) + ")");
			if (g_isBallFollowingCurve == false) {
				pBrain->LoadScript("move_towards_curved",
					"moveObjectTowardsDestinationCurved(" + std::to_string(theBall->getID()) + ", "
					+ std::to_string(theBall->m_position.x) + ", " \
					+ std::to_string(theBall->m_position.y) + ", " \
					+ std::to_string(theBall->m_position.z) + ", " \
					+ std::to_string(theBrazier->getID()) + ", " \
					+ std::to_string(-40) + ", " \
					+ std::to_string(0) + ", " \
					+ std::to_string(-40) + ", " \
					+ std::to_string(0.01) + ")");
				//followCurve(theBall, theBrazier, 1);
				g_isBallFollowingCurve = true;
			}
		}		
	}
	if (glm::distance(theBall->m_position, theBrazier->m_position) < SMALLEST_DISTANCE) {
		// Stops the active moving toward script
		pBrain->DeleteScript("move_towards_curved");
	}

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

inline void followCurve(cMeshObject* movable, cMeshObject* destination, int dt) {
	glm::vec3 origin = movable->m_position;
	glm::vec3 point1, point2, control(11, 0, -26);
	for (float i = 0; i < dt; i+=0.1) {
		
		point1.x = origin.x + (control.x - origin.x) * i;
		point1.y = origin.y + (control.y - origin.y) * i;
		point1.z = origin.z + (control.z - origin.z) * i;

		point2.x = control.x + (destination->m_position.x - control.x) * i;
		point2.y = control.y + (destination->m_position.y - control.y) * i;
		point2.z = control.z + (destination->m_position.z - control.z) * i;

		movable->m_position.x = point1.x + (point2.x - point1.x) * i;
		movable->m_position.y = point1.y + (point2.y - point1.y) * i;
		movable->m_position.z = point1.z + (point2.z - point1.z) * i;

		DEBUG_PRINT("DT: %f\n", i);
		DEBUG_PRINT("Movable x y z: %f %f %f \n\n", movable->m_position.x, movable->m_position.y, movable->m_position.z);
	}
	DEBUG_PRINT("Destination x y z: %f %f %f \n\n", destination->m_position.x, destination->m_position.y, destination->m_position.z);
}
