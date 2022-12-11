#pragma once
#include <vector>
#include <string>
#include <map>

#include "cLight.h"

class cLightManager {
public:
	cLightManager();
	~cLightManager();

	static const unsigned int NUMBER_OF_LIGHTS_IM_GONNA_USE = 7;

	void LoadLightUniformLocations(unsigned int shaderID, std::map<std::string, cLight*>* p_mapOfLights);

	void CopyLightInformationToShader(unsigned int shaderID, std::map<std::string, cLight*>* p_mapOfLights);

};

