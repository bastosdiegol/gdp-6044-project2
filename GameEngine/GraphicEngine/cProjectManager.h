#pragma once
#include <string>
#include <map>
#include <glad/glad.h>

#include "cScene.h"
#include "cVAOManager.h"
#include "cLightManager.h"

#define PROJECT_SAVE_FILE "graphicLibrary.xml"

enum GameState {
	STARTING_UP,
	NEW_GAME,
	RUNNING,
	SHUTING_DOWN
};

class cProjectManager {	
private:
	std::map<std::string, cScene*>	m_mScenes;			// List of Scenes Available

	/// <summary>
	/// Loads the PROJECT_SAVE_FILE and stores all scenes available to be loaded
	/// </summary>
	/// <returns>TRUE if loading was successful</returns>
	bool LoadSaveFile();
public:
	cScene*						m_selectedScene;	// Selected Scene by the user
	cMeshObject*				m_selectedMesh;		// Selected Mesh with info available to be edited
	cLight*						m_selectedLight;	// Selected Light
	bool						isNewScene;			// True everytime a user changes the scene
	bool						m_isGameLoopEnabled;
	GameState					m_GameLoopState;
	cVAOManager*				m_VAOManager;
	cLightManager*				m_lightManager;
	/// <summary>
	/// Default Constructor - also Loads Save File
	/// </summary>
	cProjectManager(GLuint shaderID);
	// TODO: Clean the Vector of Scenes
	~cProjectManager();

	/// <summary>
	/// Loads a scene contained on the Vector of Scenes
	/// Sets the new scene on m_selectedScene pointer
	/// </summary>
	/// <param name="name">Name of the scene to be loaded</param>
	/// <returns>True if loading was successful</returns>
	bool LoadScene(std::string name);
	/// <summary>
	/// Free Heap Memory Used for Last Loaded Scene
	/// </summary>
	void UnloadScene();
	/// <summary>
	/// Saves the selected scene and all its data to the XML file
	/// </summary>
	/// <returns>True if saving was successful</returns>
	bool SaveSelectedScene();

	/// <summary>
	/// Drawns an object on the Screen
	/// </summary>
	/// <param name="pCurrentMeshObject">Mesh to be drawn</param>
	/// <param name="shaderID"></param>
	/// <param name="mModel_location"></param>
	/// <param name="mModelInverseTransform_location"></param>
	/// <param name="parentModel">Default is identity matrix</param>
	void DrawObject(cMeshObject* pCurrentMeshObject,            
					GLuint shaderID,								// ID for the current shader
					//cBasicTextureManager* pTextureManager,
					//cVAOManager* pVAOManager,
					GLint mModel_location,							// Uniform location of mModel matrix
					GLint mModelInverseTransform_location,
					glm::mat4x4 parentModel = glm::mat4x4(1.0f));	// The "parent's" model matrix

	friend class cProjectUI;
};

