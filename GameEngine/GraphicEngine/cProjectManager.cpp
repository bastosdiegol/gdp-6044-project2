#include <pugixml/pugixml.hpp>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include "cProjectManager.h"
#include "cMeshObject.h"

#ifdef _DEBUG
#define DEBUG_LOG_ENABLED
#endif
#ifdef DEBUG_LOG_ENABLED
#define DEBUG_PRINT(x, ...) printf(x, __VA_ARGS__)
#else
#define DEBUG_PRINT(x)
#endif

cProjectManager::cProjectManager(GLuint shaderID) {
	DEBUG_PRINT("cProjectManager::cProjectManager()\n");
	if (!LoadSaveFile()) {
		std::cout << "Project Save File named: #" << PROJECT_SAVE_FILE << " could not be loaded. Ending the application." << std::endl;
		exit(-1);
	}
	m_VAOManager = new cVAOManager(shaderID);
	m_lightManager = new cLightManager();
	isNewScene = false;
	m_selectedScene = nullptr;
	m_selectedMesh = nullptr;
	m_isGameLoopEnabled = false;
	m_GameLoopState = GameState::STARTING_UP;
}

cProjectManager::~cProjectManager() {
	DEBUG_PRINT("cProjectManager::~cProjectManager()\n");
	std::map<std::string, cScene*>::iterator itScenes;
	for (itScenes = this->m_mScenes.begin();
		 itScenes != this->m_mScenes.end();
		 itScenes++)
		delete itScenes->second;
	delete m_VAOManager;
	delete m_lightManager;
}

bool cProjectManager::LoadScene(std::string name) {
	DEBUG_PRINT("cProjectManager::LoadScene(%s)\n", name.c_str());
	// I don't know if I really want to unload the scenes
	//this->UnloadScene();
	
	// Unselect previous selected mesh and light
	this->m_selectedMesh = nullptr;
	this->m_selectedLight = nullptr;
	this->m_isGameLoopEnabled = false;
	m_GameLoopState = GameState::STARTING_UP;

	// Create a document object
	pugi::xml_document graphicsLibrary;
	// Load the XML file into the object
	pugi::xml_parse_result result = graphicsLibrary.load_file(PROJECT_SAVE_FILE);
	if (!result) {
		std::cout << "ProjectManager error: Failed to load file named #" << PROJECT_SAVE_FILE << std::endl;
		return false;
	}
	DEBUG_PRINT("Successfully loaded file named #%s\n", PROJECT_SAVE_FILE);

	// Gets all nodes of sound inside the soundlibrary
	pugi::xml_node scenes = graphicsLibrary.child("graphicsLibrary");

	DEBUG_PRINT("<graphicsLibrary>\n");
	// Iterates through each graphicsLibrary node
	for (pugi::xml_node sceneNode = scenes.child("scene");
		sceneNode;
		sceneNode = sceneNode.next_sibling("scene")) {
		// Checks if current sceneNode is the selected Node
		if (std::strcmp(sceneNode.attribute("title").value(), name.c_str()) == 0) {

			// Tries to find if the scene was already loaded
			std::map<std::string, cScene*>::iterator itScene = m_mScenes.find(name);
			if (itScene != m_mScenes.end()) {
				// Checks if the scene was already loaded
				if (itScene->second != nullptr) {
					this->m_selectedScene = itScene->second;
					DEBUG_PRINT("Scene loaded previously ... Skipping new loading...\n");
					// Sets new Scene bool
					isNewScene = true;
					return true; // Scene previously loaded FOUND
				}
			}

			// Creates a pointer to the new Scene
			cScene* newScene = new cScene(name);
			// Sets Scene Camera Eye
			newScene->m_cameraEye	 = glm::vec3(sceneNode.attribute("camEyeX").as_float(),
												 sceneNode.attribute("camEyeY").as_float(),
												 sceneNode.attribute("camEyeZ").as_float());
			// Sets Scene Camera Target
			newScene->m_cameraTarget = glm::vec3(sceneNode.attribute("camTarX").as_float(),
												 sceneNode.attribute("camTarY").as_float(),
												 sceneNode.attribute("camTarZ").as_float());
			// Sets the new scene as selected one
			this->m_selectedScene = newScene;

			DEBUG_PRINT("<scene title=\"%s\">\n", name.c_str());
			// Iterates through each Model node inside the selected Scene Node
			for (pugi::xml_node modelNode = sceneNode.child("model");
				modelNode;
				modelNode = modelNode.next_sibling("model")) {
				DEBUG_PRINT("	<model path=\"%s\" title=\"%s\" \\> \n", modelNode.attribute("path").value()
																	   , modelNode.attribute("title").value());
				// Checks if parameters provided by XML are safe
				if ((std::strcmp(modelNode.attribute("path").value(), "") == 0
					|| std::strcmp(modelNode.attribute("title").value(), "") == 0)) {
					continue;
				}
				cModel* newModel;
				// Checks if Model was already loaded by another scene
				newModel = m_VAOManager->findModel(modelNode.attribute("title").value());
				if (newModel == nullptr) {
					// Creates a new model and add its Meshes to the MeshObject List of the Scene
					newModel = m_VAOManager->PrepareNewModel(modelNode.attribute("title").value(), modelNode.attribute("path").value());
					m_VAOManager->LoadModelIntoVAO(newModel);
				} else {
					DEBUG_PRINT("Model previously loaded found ... Skipping the creation a new model ...\n");
				}
				// Iterates through each mesh using the same model
				for (pugi::xml_node meshNode = modelNode.child("mesh");
					meshNode;
					meshNode = meshNode.next_sibling("mesh")) {
					DEBUG_PRINT("		<mesh id=\"%s\" \\> \n", meshNode.attribute("id").value());
					cMeshObject* newMeshObj = new cMeshObject(newModel);
					newMeshObj->m_meshName = meshNode.attribute("id").value();
					// Now lets load the rest of info about the mesh
					// Reads Position
					pugi::xml_node meshInfo = meshNode.child("position");
					newMeshObj->m_position = glm::vec3(meshInfo.attribute("x").as_float(),
													   meshInfo.attribute("y").as_float(),
													   meshInfo.attribute("z").as_float());
					meshInfo = meshInfo.next_sibling();
					// Reads Rotation
					newMeshObj->m_rotation = glm::vec3(meshInfo.attribute("x").as_float(),
													   meshInfo.attribute("y").as_float(),
													   meshInfo.attribute("z").as_float());
					meshInfo = meshInfo.next_sibling();
					// Reads UseRGB?
					newMeshObj->m_bUse_RGBA_colour = meshInfo.attribute("value").as_bool();
					meshInfo = meshInfo.next_sibling();
					// Reads Colour
					newMeshObj->m_RGBA_colour = glm::vec4(meshInfo.attribute("r").as_float(),
														  meshInfo.attribute("g").as_float(),
														  meshInfo.attribute("b").as_float(),
														  1);
					meshInfo = meshInfo.next_sibling();
					// Reads Scale
					newMeshObj->m_scale = meshInfo.attribute("value").as_float();
					meshInfo = meshInfo.next_sibling();
					// Reads isWireframe
					newMeshObj->m_isWireframe = meshInfo.attribute("value").as_bool();
					meshInfo = meshInfo.next_sibling();
					// Reads doNotLight
					newMeshObj->m_bDoNotLight = meshInfo.attribute("value").as_bool();
					meshInfo = meshInfo.next_sibling();
					// Reads isVisible
					newMeshObj->m_bIsVisible = meshInfo.attribute("value").as_bool();
					meshInfo = meshInfo.next_sibling();
					// Reads isVisible
					newMeshObj->m_displayBoundingBox = meshInfo.attribute("value").as_bool();

					// Adds the newly created Mesh to the Scene Map of Meshes
					newScene->m_mMeshes.try_emplace(newMeshObj->m_meshName, newMeshObj);
				}
			}
			// Iterates through each Light Node
			for (pugi::xml_node lightNode = sceneNode.child("light");
				lightNode;
				lightNode = lightNode.next_sibling("light")) {
				// Creates new Instance of the Light
				cLight* newLight = new cLight();
				// Gets Light Friendly Name / ID
				newLight->m_friendlyName = lightNode.attribute("id").value();
				// Gets On/Off
				newLight->m_param2.x = lightNode.attribute("onoff").as_bool();
				// Gets Show Model
				newLight->m_showModel = lightNode.attribute("showmodel").as_bool();
				// Reads light info and store them
				pugi::xml_node lightInfo = lightNode.child("position");
				// Reads Position
				newLight->m_position.x = lightInfo.attribute("x").as_float();
				newLight->m_position.y = lightInfo.attribute("y").as_float();
				newLight->m_position.z = lightInfo.attribute("z").as_float();
				newLight->m_position.w = lightInfo.attribute("w").as_float();
				lightInfo = lightInfo.next_sibling();
				// Reads Diffuse
				newLight->m_diffuse.x = lightInfo.attribute("x").as_float();
				newLight->m_diffuse.y = lightInfo.attribute("y").as_float();
				newLight->m_diffuse.z = lightInfo.attribute("z").as_float();
				newLight->m_diffuse.w = lightInfo.attribute("w").as_float();
				lightInfo = lightInfo.next_sibling();
				// Reads Specullar
				newLight->m_specular.r = lightInfo.attribute("r").as_float();
				newLight->m_specular.g = lightInfo.attribute("g").as_float();
				newLight->m_specular.b = lightInfo.attribute("b").as_float();
				newLight->m_specular.a = lightInfo.attribute("a").as_float();
				lightInfo = lightInfo.next_sibling();
				// Reads Attenuation
				newLight->m_attenuation.x = lightInfo.attribute("x").as_float();
				newLight->m_attenuation.y = lightInfo.attribute("y").as_float();
				newLight->m_attenuation.z = lightInfo.attribute("z").as_float();
				newLight->m_attenuation.w = lightInfo.attribute("w").as_float();
				lightInfo = lightInfo.next_sibling();
				// Reads Direction
				newLight->m_direction.x = lightInfo.attribute("x").as_float();
				newLight->m_direction.y = lightInfo.attribute("y").as_float();
				newLight->m_direction.z = lightInfo.attribute("z").as_float();
				newLight->m_direction.w = lightInfo.attribute("w").as_float();
				lightInfo = lightInfo.next_sibling();
				// Reads Param1
				newLight->m_param1.x = lightInfo.attribute("x").as_float();
				newLight->m_param1.y = lightInfo.attribute("y").as_float();
				newLight->m_param1.z = lightInfo.attribute("z").as_float();
				newLight->m_param1.w = lightInfo.attribute("w").as_float();
				lightInfo = lightInfo.next_sibling();
				// Reads Param2
				newLight->m_param2.x = lightInfo.attribute("x").as_float();
				newLight->m_param2.y = lightInfo.attribute("y").as_float();
				newLight->m_param2.z = lightInfo.attribute("z").as_float();
				newLight->m_param2.w = lightInfo.attribute("w").as_float();

				// Adds the newly created Light to the Scene Map of Lights
				newScene->m_mLights.try_emplace(newLight->m_friendlyName, newLight);
			}
			// Gets MeshRelationship Node
			pugi::xml_node meshRelationship = sceneNode.child("meshRelationship");
			// Checks if it exists
			if (meshRelationship != NULL) {
				// Iterates through each parent
				for (pugi::xml_node meshReParent = meshRelationship.child("parent");
									meshReParent;
									meshReParent = meshReParent.next_sibling("parent")) {
					// Grabs the Parent mesh
					cMeshObject* meshParentObj = newScene->m_mMeshes.find(meshReParent.attribute("id").as_string())->second;
					// Iterates through each child
					for (pugi::xml_node meshReChild = meshReParent.child("child");
										meshReChild;
										meshReChild = meshReChild.next_sibling("child")) {
						std::string childMeshName = meshReChild.attribute("id").as_string();
						// Grabs the Child mesh
						cMeshObject* meshChildObj = newScene->m_mMeshes.find(childMeshName)->second;
						// Associeted the Parent mesh with the Child
						meshParentObj->vecChildMeshes.push_back(meshChildObj);
						// Removes it from the scene mesh list
						newScene->m_mMeshes.erase(newScene->m_mMeshes.find(childMeshName));
					}
				}
			}
			
			// Load UniformLocations for new Scene and set them on each light that's going to be used
			if(newScene->m_mLights.size() > 0)
				m_lightManager->LoadLightUniformLocations(m_VAOManager->m_shaderID, &newScene->m_mLights);
			// Adds the scene to the Map of scenes
			itScene = m_mScenes.find(name);
			if (itScene != m_mScenes.end()) {
				// Checks if the scene was already loaded
				itScene->second = newScene;
			} else {
				this->m_mScenes.try_emplace(name, newScene);
			}
		}
	}
	// Sets new Scene bool
	isNewScene = true;
	m_isGameLoopEnabled = true;
	return true;
}

void cProjectManager::UnloadScene() {
	DEBUG_PRINT("cProjectManager::UnloadScene()\n");
	// Cheks if theres no selected scene
	if (this->m_selectedScene == nullptr)
		return;

	std::map<std::string, cMeshObject*>::iterator itMeshes;
	for (itMeshes = this->m_selectedScene->m_mMeshes.begin();
		 itMeshes != this->m_selectedScene->m_mMeshes.end();
		 itMeshes++) {
		itMeshes->second->~cMeshObject();
		delete itMeshes->second;
	}

	this->m_selectedMesh = nullptr;
}

bool cProjectManager::SaveSelectedScene() {
	// Checks if there's a selected scene
	if (m_selectedScene == nullptr) {
		return false;
	}
	DEBUG_PRINT("cProjectManager::SaveSelectedScene()\n");
	// Create a document object
	pugi::xml_document graphicsLibrary;
	// Load the XML file into the object
	pugi::xml_parse_result result = graphicsLibrary.load_file(PROJECT_SAVE_FILE);
	if (!result) {
		std::cout << "ProjectManager error: Failed to load file named #" << PROJECT_SAVE_FILE << std::endl;
		return false;
	}
	DEBUG_PRINT("Successfully loaded file named #%s\n", PROJECT_SAVE_FILE);

	// Gets all nodes of sound inside the soundlibrary
	pugi::xml_node scenes = graphicsLibrary.child("graphicsLibrary");
	// Iterates through each graphicsLibrary node to find Selected Scene
	for (pugi::xml_node sceneNode = scenes.child("scene");
						sceneNode;
						sceneNode = sceneNode.next_sibling("scene")) {
		// Checks if current sceneNode is the selected Node
		if (std::strcmp(sceneNode.attribute("title").value(), m_selectedScene->m_name.c_str()) == 0) {
			// Found It - Sets camera attributes
			sceneNode.attribute("camEyeX").set_value(m_selectedScene->m_cameraEye.x);
			sceneNode.attribute("camEyeY").set_value(m_selectedScene->m_cameraEye.y);
			sceneNode.attribute("camEyeZ").set_value(m_selectedScene->m_cameraEye.z);
			sceneNode.attribute("camTarX").set_value(m_selectedScene->m_cameraTarget.x);
			sceneNode.attribute("camTarY").set_value(m_selectedScene->m_cameraTarget.y);
			sceneNode.attribute("camTarZ").set_value(m_selectedScene->m_cameraTarget.z);
			// Now Sets the Model attributes
			// Iterates through each Model of the scene
			for (pugi::xml_node modelNode = sceneNode.child("model");
								modelNode;
								modelNode = modelNode.next_sibling("model")) {
				// Iterates through each Mesh of this Model
				for (pugi::xml_node meshNode = modelNode.child("mesh");
									meshNode;
									meshNode = meshNode.next_sibling("mesh")) {
					std::map<std::string, cMeshObject*>::iterator itMesh = m_selectedScene->m_mMeshes.find(meshNode.attribute("id").as_string());
					// Checks if found the Selected Scene Obj
					if (itMesh == m_selectedScene->m_mMeshes.end()) {
						DEBUG_PRINT("Tried to find a Mesh to save and got nullptr. Mesh %s not saved.", meshNode.attribute("id").as_string());
						continue;
					}
					// Gets first Mesh Child Node - Position
					pugi::xml_node meshInfoNode = *meshNode.children().begin();
					// Sets Position
					meshInfoNode.attribute("x").set_value(itMesh->second->m_position.x);
					meshInfoNode.attribute("y").set_value(itMesh->second->m_position.y);
					meshInfoNode.attribute("z").set_value(itMesh->second->m_position.z);
					meshInfoNode = meshInfoNode.next_sibling();
					// Sets Rotation
					meshInfoNode.attribute("x").set_value(itMesh->second->m_rotation.x);
					meshInfoNode.attribute("y").set_value(itMesh->second->m_rotation.y);
					meshInfoNode.attribute("z").set_value(itMesh->second->m_rotation.z);
					meshInfoNode = meshInfoNode.next_sibling();
					// Sets Use Color
					meshInfoNode.attribute("value").set_value(itMesh->second->m_bUse_RGBA_colour);
					meshInfoNode = meshInfoNode.next_sibling();
					// Sets Color
					meshInfoNode.attribute("r").set_value(itMesh->second->m_RGBA_colour.r);
					meshInfoNode.attribute("g").set_value(itMesh->second->m_RGBA_colour.g);
					meshInfoNode.attribute("b").set_value(itMesh->second->m_RGBA_colour.b);
					meshInfoNode.attribute("a").set_value(itMesh->second->m_RGBA_colour.a);
					meshInfoNode = meshInfoNode.next_sibling();
					// Sets Scale
					meshInfoNode.attribute("value").set_value(itMesh->second->m_scale);
					meshInfoNode = meshInfoNode.next_sibling();
					// Sets Wireframe
					meshInfoNode.attribute("value").set_value(itMesh->second->m_isWireframe);
					meshInfoNode = meshInfoNode.next_sibling();
					// Sets DoNotLight
					meshInfoNode.attribute("value").set_value(itMesh->second->m_bDoNotLight);
					meshInfoNode = meshInfoNode.next_sibling();
					// Sets Visible
					meshInfoNode.attribute("value").set_value(itMesh->second->m_bIsVisible);
				}
			}
			// Now Sets the Light attributes
			// Iterates through each Light of the scene
			for (pugi::xml_node lightNode = sceneNode.child("light");
				lightNode;
				lightNode = lightNode.next_sibling("light")) {
				std::map<std::string, cLight*>::iterator itLight = m_selectedScene->m_mLights.find(lightNode.attribute("id").as_string());
				// Checks if found the Selected Light Obj
				if (itLight == m_selectedScene->m_mLights.end()) {
					DEBUG_PRINT("Tried to find a Mesh to save and got nullptr. Mesh %s not saved.", lightNode.attribute("id").as_string());
					continue;
				}
				// Sets On/Off
				lightNode.attribute("onoff").set_value(itLight->second->m_param2.x);
				// Sets Show Model
				lightNode.attribute("showmodel").set_value(itLight->second->m_showModel);
				// Gets first Mesh Child Node - Position
				pugi::xml_node lightInfoNode = *lightNode.children().begin();
				// Sets Position
				lightInfoNode.attribute("x").set_value(itLight->second->m_position.x);
				lightInfoNode.attribute("y").set_value(itLight->second->m_position.y);
				lightInfoNode.attribute("z").set_value(itLight->second->m_position.z);
				lightInfoNode.attribute("w").set_value(itLight->second->m_position.w);
				lightInfoNode = lightInfoNode.next_sibling();
				// Sets Diffuse
				lightInfoNode.attribute("x").set_value(itLight->second->m_diffuse.x);
				lightInfoNode.attribute("y").set_value(itLight->second->m_diffuse.y);
				lightInfoNode.attribute("z").set_value(itLight->second->m_diffuse.z);
				lightInfoNode.attribute("w").set_value(itLight->second->m_diffuse.w);
				lightInfoNode = lightInfoNode.next_sibling();
				// Sets Specular
				lightInfoNode.attribute("r").set_value(itLight->second->m_specular.r);
				lightInfoNode.attribute("g").set_value(itLight->second->m_specular.g);
				lightInfoNode.attribute("b").set_value(itLight->second->m_specular.b);
				lightInfoNode.attribute("a").set_value(itLight->second->m_specular.a);
				lightInfoNode = lightInfoNode.next_sibling();
				// Sets Attenuation
				lightInfoNode.attribute("x").set_value(itLight->second->m_attenuation.x);
				lightInfoNode.attribute("y").set_value(itLight->second->m_attenuation.y);
				lightInfoNode.attribute("z").set_value(itLight->second->m_attenuation.z);
				lightInfoNode.attribute("w").set_value(itLight->second->m_attenuation.w);
				lightInfoNode = lightInfoNode.next_sibling();
				// Sets Direction
				lightInfoNode.attribute("x").set_value(itLight->second->m_direction.x);
				lightInfoNode.attribute("y").set_value(itLight->second->m_direction.y);
				lightInfoNode.attribute("z").set_value(itLight->second->m_direction.z);
				lightInfoNode.attribute("w").set_value(itLight->second->m_direction.w);
				lightInfoNode = lightInfoNode.next_sibling();
				// Sets param1
				lightInfoNode.attribute("x").set_value(itLight->second->m_param1.x);
				lightInfoNode.attribute("y").set_value(itLight->second->m_param1.y);
				lightInfoNode.attribute("z").set_value(itLight->second->m_param1.z);
				lightInfoNode.attribute("w").set_value(itLight->second->m_param1.w);
				lightInfoNode = lightInfoNode.next_sibling();
				// Sets param2
				lightInfoNode.attribute("x").set_value(itLight->second->m_param2.x);
				lightInfoNode.attribute("y").set_value(itLight->second->m_param2.y);
				lightInfoNode.attribute("z").set_value(itLight->second->m_param2.z);
				lightInfoNode.attribute("w").set_value(itLight->second->m_param2.w);
			}
		}
	}
	graphicsLibrary.save_file(PROJECT_SAVE_FILE);
	return true;
}

void cProjectManager::DrawObject(cMeshObject* pCurrentMeshObject, GLuint shaderID, GLint mModel_location, GLint mModelInverseTransform_location, glm::mat4x4 parentModel) {
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	glm::mat4x4 matModel = parentModel;
	// Apply Position Transformation
	glm::mat4 matTranslation = glm::translate(glm::mat4(1.0f), pCurrentMeshObject->m_position);
	// Apply Rotation Transformation
	glm::mat4 matRoationZ = glm::rotate(glm::mat4(1.0f), pCurrentMeshObject->m_rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 matRoationY = glm::rotate(glm::mat4(1.0f), pCurrentMeshObject->m_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 matRoationX = glm::rotate(glm::mat4(1.0f), pCurrentMeshObject->m_rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	// Scale the object
	float uniformScale = pCurrentMeshObject->m_scale;
	glm::mat4 matScale = glm::scale(glm::mat4(1.0f), glm::vec3(uniformScale, uniformScale, uniformScale));
	// Applying all these transformations to the Model
	matModel = matModel * matTranslation;
	matModel = matModel * matRoationX;
	matModel = matModel * matRoationY;
	matModel = matModel * matRoationZ;
	matModel = matModel * matScale;

	// Pass all the matrices to the Shader
	glUniformMatrix4fv(mModel_location, 1, GL_FALSE, glm::value_ptr(matModel));

	// Inverse transpose of a 4x4 matrix removes the right column and lower row
	// Leaving only the rotation (the upper left 3x3 matrix values)
	glm::mat4 mModelInverseTransform = glm::inverse(glm::transpose(matModel));
	glUniformMatrix4fv(mModelInverseTransform_location, 1, GL_FALSE, glm::value_ptr(mModelInverseTransform));

	// Wireframe Check
	if (pCurrentMeshObject->m_isWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Pass Colours to the Shader
	GLint RGBA_Colour_ULocID = glGetUniformLocation(shaderID, "RGBA_Colour");
	glUniform4f(RGBA_Colour_ULocID, pCurrentMeshObject->m_RGBA_colour.r,
									pCurrentMeshObject->m_RGBA_colour.g,
									pCurrentMeshObject->m_RGBA_colour.b,
									pCurrentMeshObject->m_RGBA_colour.w);
	// Pass the UseRGB boolean to the Shader
	GLint bUseRGBA_Colour_ULocID = glGetUniformLocation(shaderID, "bUseRGBA_Colour");
	if (pCurrentMeshObject->m_bUse_RGBA_colour)
		glUniform1f(bUseRGBA_Colour_ULocID, (GLfloat)GL_TRUE);
	else
		glUniform1f(bUseRGBA_Colour_ULocID, (GLfloat)GL_FALSE);

	// Pass DoNotLight boolean to the Shader	
	GLint bDoNotLight_Colour_ULocID = glGetUniformLocation(shaderID, "bDoNotLight");
	if (pCurrentMeshObject->m_bDoNotLight)
		glUniform1f(bDoNotLight_Colour_ULocID, (GLfloat)GL_TRUE);
	else
		glUniform1f(bDoNotLight_Colour_ULocID, (GLfloat)GL_FALSE);

	// Pass the Model we want to draw
	glBindVertexArray(pCurrentMeshObject->m_parentModel->VAO_ID);
	glDrawElements(GL_TRIANGLES,
		pCurrentMeshObject->m_parentModel->numberOfIndices,
		GL_UNSIGNED_INT,
		(void*)0);
	glBindVertexArray(0);

	// Display Bounding Box
	if (pCurrentMeshObject->m_displayBoundingBox) {
		// Cube 1x1x1, centered on origin
		GLfloat vertices[] = {
		  -0.5, -0.5, -0.5, 1.0,
		   0.5, -0.5, -0.5, 1.0,
		   0.5,  0.5, -0.5, 1.0,
		  -0.5,  0.5, -0.5, 1.0,
		  -0.5, -0.5,  0.5, 1.0,
		   0.5, -0.5,  0.5, 1.0,
		   0.5,  0.5,  0.5, 1.0,
		  -0.5,  0.5,  0.5, 1.0,
		};
		GLuint vbo_vertices;
		glGenBuffers(1, &vbo_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLushort elements[] = {
			0, 1, 2, 3,
			4, 5, 6, 7,
			0, 4, 1, 5, 2, 6, 3, 7
		};
		GLuint ibo_elements;
		glGenBuffers(1, &ibo_elements);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		cModel* parentModel = pCurrentMeshObject->m_parentModel;
		glm::vec3 size = glm::vec3(parentModel->max_x - parentModel->min_x,
								   parentModel->max_y - parentModel->min_y,
								   parentModel->max_z - parentModel->min_z);
		glm::vec3 center = glm::vec3((parentModel->min_x + parentModel->max_x) / 2,
									 (parentModel->min_y + parentModel->max_y) / 2,
									 (parentModel->min_z + parentModel->max_z) / 2);
		glm::mat4 transform = glm::translate(glm::mat4(1), center) * glm::scale(glm::mat4(1), size);

		// Use Colour vCol
		glUniform1f(bUseRGBA_Colour_ULocID, (GLfloat)GL_TRUE);
		if (this->m_selectedScene->m_name == "5.Patterns MidTerm" &&
			this->m_GameLoopState == RUNNING) {
			/*size_t pos = pCurrentMeshObject->m_meshName.find(" ");
			std::string token;
			int robotID;
			iRobot* theRobot;
			if (pos != std::string::npos) {
				token = pCurrentMeshObject->m_meshName.substr(0, pos);
				if (token == "Robot") {
					robotID = std::stoi(pCurrentMeshObject->m_meshName.substr(pos, pos + 2));
					theRobot = g_robotFactory->getRobot(robotID - 1);
					if (theRobot->getWeaponName() == "Laser") {
						glUniform4f(RGBA_Colour_ULocID, 0.0f, 1.0f, 0.0f, 1.0f);
					} else if (theRobot->getWeaponName() == "Bomb") {
						glUniform4f(RGBA_Colour_ULocID, 1.0f, 0.0f, 0.0f, 1.0f);
					} else if (theRobot->getWeaponName() == "Bullet") {
						glUniform4f(RGBA_Colour_ULocID, 0.0f, 0.0f, 1.0f, 1.0f);
					}
				}
			}*/
		} else {
			// Set White BoundingBox
			glUniform4f(RGBA_Colour_ULocID, 1.0f, 1.0f, 1.0f, 1.0f);
		}
		// Do Not Light the BB
		glUniform1f(bDoNotLight_Colour_ULocID, (GLfloat)GL_TRUE);

		/* Apply object's transformation matrix */
		glm::mat4 m = matModel * transform;
		glUniformMatrix4fv(mModel_location, 1, GL_FALSE, glm::value_ptr(m));

		GLint attribute_v_coord = glGetAttribLocation(this->m_VAOManager->m_shaderID, "vPos");
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glEnableVertexAttribArray(attribute_v_coord);
		glVertexAttribPointer(
			attribute_v_coord,  // attribute
			4,                  // number of elements per vertex, here (x,y,z,w)
			GL_FLOAT,           // the type of each element
			GL_FALSE,           // take our values as-is
			0,                  // no extra data between each position
			0                   // offset of first element
		);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
		glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
		glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
		glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glDisableVertexAttribArray(attribute_v_coord);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDeleteBuffers(1, &vbo_vertices);
		glDeleteBuffers(1, &ibo_elements);

		glLineWidth(2);

	}

	// Draw all the children
	for (std::vector< cMeshObject* >::iterator itCurrentMesh = pCurrentMeshObject->vecChildMeshes.begin();
		itCurrentMesh != pCurrentMeshObject->vecChildMeshes.end();
		itCurrentMesh++) {
		cMeshObject* pCurrentCHILDMeshObject = *itCurrentMesh;        // * is the iterator access thing

	   // All the drawing code has been moved to the DrawObject function
		DrawObject(pCurrentCHILDMeshObject,
				   shaderID, 
				   //pTextureManager,
				   mModel_location, 
				   mModelInverseTransform_location, 
				   matModel);

	}
}

bool cProjectManager::LoadSaveFile() {
	DEBUG_PRINT("cProjectManager::LoadSaveFile()\n");
	// Create a document object
	pugi::xml_document graphicsLibrary;
	// Load the XML file into the object
	pugi::xml_parse_result result = graphicsLibrary.load_file(PROJECT_SAVE_FILE);
	if (!result) {
		std::cout << "ProjectManager error: Failed to load file named #" << PROJECT_SAVE_FILE << std::endl;
		return false;
	}
	DEBUG_PRINT("Successfully loaded file named #%s\n", PROJECT_SAVE_FILE);

	// Gets all nodes of sound inside the soundlibrary
	pugi::xml_node scenes = graphicsLibrary.child("graphicsLibrary");
	//cScene* scene;

	DEBUG_PRINT("<graphicsLibrary>\n");
	// Iterates through each node
	for (pugi::xml_node sceneNode = scenes.child("scene"); 
						sceneNode; 
						sceneNode = sceneNode.next_sibling("scene")) {
		DEBUG_PRINT("	<scene> Title: %s\n",sceneNode.attribute("title").value());
		// Adds the unloaded scene to the map
		m_mScenes.try_emplace(sceneNode.attribute("title").value(), nullptr);
	}

	return true;
}
