#pragma once
#include <string>
#include <map>

#include "cModel.h"
#include "cPlyFileReader.h"

#define LIGHT_MODEL_PATH "assets/ISO_Shphere.ply"

class cVAOManager {
public:
	GLuint m_shaderID;
	cModel* m_lightModel;
	// Default Constructor
	cVAOManager(GLuint shaderID);
	// Destructor
	~cVAOManager();
	/// <summary>
	/// After Reading from a Ply File
	/// This Methods organize that Data to be ready to send
	/// To the GPU (Shader)
	/// </summary>
	/// <returns></returns>
	cModel* PrepareNewModel(std::string friendlyName,
						 std::string filePath);
	/// <summary>
	/// Loads the light model
	/// </summary>
	/// <returns>Returns pointer to the model of the light</returns>
	cModel* LoadLightModel();

	/// <summary>
	/// Binds now the Model Data to the Shader
	/// </summary>
	/// <param name="fileName">Friendly Name</param>
	/// <param name="drawInfo">Model Vertices n Triangles</param>
	/// <param name="shaderProgramID">Shader ID</param>
	/// <returns></returns>
	bool LoadModelIntoVAO(cModel* modelObj);

	/// <summary>
	/// Checks if a model was previously loaded
	/// </summary>
	/// <param name="name">Name of the model</param>
	/// <returns>The instance of the model</returns>
	cModel* findModel(std::string name);

private:
	cPlyFileReader*					m_plyReader;
	std::map<std::string, cModel*>	m_mapModels;
};

