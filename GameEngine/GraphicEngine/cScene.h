#pragma once
#include <string>
#include <map>

#include "cMeshObject.h"
#include "cLight.h"

class cScene {
public:
	std::string				 m_name;
	glm::vec3				 m_cameraEye;
	glm::vec3				 m_cameraTarget;
	std::map<std::string, cMeshObject*> m_mMeshes;
	std::map<std::string, cLight*>		m_mLights;

	cScene(std::string name);
	// TODO: Clean the Vector of MeshOjects
	~cScene();
};

