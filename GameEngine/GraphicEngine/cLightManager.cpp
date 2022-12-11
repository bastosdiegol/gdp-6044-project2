#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <sstream>

#include "cLightManager.h"

#ifdef _DEBUG
#define DEBUG_LOG_ENABLED
#endif
#ifdef DEBUG_LOG_ENABLED
#define DEBUG_PRINT(x, ...) printf(x, __VA_ARGS__)
#else
#define DEBUG_PRINT(x)
#endif

cLightManager::cLightManager() {
	DEBUG_PRINT("cLightManager::cLightManager()\n");
}

cLightManager::~cLightManager() {
	DEBUG_PRINT("cLightManager::~cLightManager()\n");
}

void cLightManager::LoadLightUniformLocations(unsigned int shaderID, std::map<std::string, cLight*>* p_mapOfLights) {
	DEBUG_PRINT("cLightManager::LoadLightUniformLocations(%d, mapOfLights[%d])\n", shaderID, p_mapOfLights->size());
	std::map<std::string, cLight*>::iterator itLight = p_mapOfLights->begin();
	if(itLight != p_mapOfLights->end()){
		itLight->second->position_UniformLocation	= glGetUniformLocation(shaderID, "theLights[0].position");
		itLight->second->diffuse_UniformLocation	= glGetUniformLocation(shaderID, "theLights[0].diffuse");
		itLight->second->specular_UniformLocation	= glGetUniformLocation(shaderID, "theLights[0].specular");
		itLight->second->atten_UniformLocation		= glGetUniformLocation(shaderID, "theLights[0].atten");
		itLight->second->direction_UniformLocation	= glGetUniformLocation(shaderID, "theLights[0].direction");
		itLight->second->param1_UniformLocation		= glGetUniformLocation(shaderID, "theLights[0].param1");
		itLight->second->param2_UniformLocation		= glGetUniformLocation(shaderID, "theLights[0].param2");
	} else {
		return;
	}
	itLight++;
	if(itLight != p_mapOfLights->end()){
		itLight->second->position_UniformLocation	= glGetUniformLocation(shaderID, "theLights[1].position");
		itLight->second->diffuse_UniformLocation	= glGetUniformLocation(shaderID, "theLights[1].diffuse");
		itLight->second->specular_UniformLocation	= glGetUniformLocation(shaderID, "theLights[1].specular");
		itLight->second->atten_UniformLocation		= glGetUniformLocation(shaderID, "theLights[1].atten");
		itLight->second->direction_UniformLocation	= glGetUniformLocation(shaderID, "theLights[1].direction");
		itLight->second->param1_UniformLocation		= glGetUniformLocation(shaderID, "theLights[1].param1");
		itLight->second->param2_UniformLocation		= glGetUniformLocation(shaderID, "theLights[1].param2");
	} else {
		return;
	}
	itLight++;
	if(itLight != p_mapOfLights->end()){
		itLight->second->position_UniformLocation	= glGetUniformLocation(shaderID, "theLights[2].position");
		itLight->second->diffuse_UniformLocation	= glGetUniformLocation(shaderID, "theLights[2].diffuse");
		itLight->second->specular_UniformLocation	= glGetUniformLocation(shaderID, "theLights[2].specular");
		itLight->second->atten_UniformLocation		= glGetUniformLocation(shaderID, "theLights[2].atten");
		itLight->second->direction_UniformLocation	= glGetUniformLocation(shaderID, "theLights[2].direction");
		itLight->second->param1_UniformLocation		= glGetUniformLocation(shaderID, "theLights[2].param1");
		itLight->second->param2_UniformLocation		= glGetUniformLocation(shaderID, "theLights[2].param2");
	} else {
		return;
	}
	itLight++;
	if(itLight != p_mapOfLights->end()){
		itLight->second->position_UniformLocation	= glGetUniformLocation(shaderID, "theLights[3].position");
		itLight->second->diffuse_UniformLocation	= glGetUniformLocation(shaderID, "theLights[3].diffuse");
		itLight->second->specular_UniformLocation	= glGetUniformLocation(shaderID, "theLights[3].specular");
		itLight->second->atten_UniformLocation		= glGetUniformLocation(shaderID, "theLights[3].atten");
		itLight->second->direction_UniformLocation	= glGetUniformLocation(shaderID, "theLights[3].direction");
		itLight->second->param1_UniformLocation		= glGetUniformLocation(shaderID, "theLights[3].param1");
		itLight->second->param2_UniformLocation		= glGetUniformLocation(shaderID, "theLights[3].param2");
	} else {
		return;
	}
	itLight++;
	if(itLight != p_mapOfLights->end()){
		itLight->second->position_UniformLocation	= glGetUniformLocation(shaderID, "theLights[4].position");
		itLight->second->diffuse_UniformLocation	= glGetUniformLocation(shaderID, "theLights[4].diffuse");
		itLight->second->specular_UniformLocation	= glGetUniformLocation(shaderID, "theLights[4].specular");
		itLight->second->atten_UniformLocation		= glGetUniformLocation(shaderID, "theLights[4].atten");
		itLight->second->direction_UniformLocation	= glGetUniformLocation(shaderID, "theLights[4].direction");
		itLight->second->param1_UniformLocation		= glGetUniformLocation(shaderID, "theLights[4].param1");
		itLight->second->param2_UniformLocation		= glGetUniformLocation(shaderID, "theLights[4].param2");
	} else {
		return;
	}
	itLight++;
	if(itLight != p_mapOfLights->end()){
		itLight->second->position_UniformLocation	= glGetUniformLocation(shaderID, "theLights[5].position");
		itLight->second->diffuse_UniformLocation	= glGetUniformLocation(shaderID, "theLights[5].diffuse");
		itLight->second->specular_UniformLocation	= glGetUniformLocation(shaderID, "theLights[5].specular");
		itLight->second->atten_UniformLocation		= glGetUniformLocation(shaderID, "theLights[5].atten");
		itLight->second->direction_UniformLocation	= glGetUniformLocation(shaderID, "theLights[5].direction");
		itLight->second->param1_UniformLocation		= glGetUniformLocation(shaderID, "theLights[5].param1");
		itLight->second->param2_UniformLocation		= glGetUniformLocation(shaderID, "theLights[5].param2");
	} else {
		return;
	}
	itLight++;
	if(itLight != p_mapOfLights->end()){
		itLight->second->position_UniformLocation	= glGetUniformLocation(shaderID, "theLights[6].position");
		itLight->second->diffuse_UniformLocation	= glGetUniformLocation(shaderID, "theLights[6].diffuse");
		itLight->second->specular_UniformLocation	= glGetUniformLocation(shaderID, "theLights[6].specular");
		itLight->second->atten_UniformLocation		= glGetUniformLocation(shaderID, "theLights[6].atten");
		itLight->second->direction_UniformLocation	= glGetUniformLocation(shaderID, "theLights[6].direction");
		itLight->second->param1_UniformLocation		= glGetUniformLocation(shaderID, "theLights[6].param1");
		itLight->second->param2_UniformLocation		= glGetUniformLocation(shaderID, "theLights[6].param2");
	} else {
		return;
	}
}

void cLightManager::CopyLightInformationToShader(unsigned int shaderID, std::map<std::string, cLight*>* p_mapOfLights) {
	//DEBUG_PRINT("cLightManager::CopyLightInformationToShader(%d, mapOfLights[%d])\n", shaderID, p_mapOfLights->size());
	std::map<std::string, cLight*>::iterator itLight;
	for (itLight = p_mapOfLights->begin(); itLight != p_mapOfLights->end(); itLight++) {
		glUniform4f(itLight->second->position_UniformLocation,
					itLight->second->m_position.x,
					itLight->second->m_position.y,
					itLight->second->m_position.z,
					itLight->second->m_position.w);

		glUniform4f(itLight->second->diffuse_UniformLocation,
					itLight->second->m_diffuse.x,
					itLight->second->m_diffuse.y,
					itLight->second->m_diffuse.z,
					itLight->second->m_diffuse.w);

		glUniform4f(itLight->second->specular_UniformLocation,
					itLight->second->m_specular.x,
					itLight->second->m_specular.y,
					itLight->second->m_specular.z,
					itLight->second->m_specular.w);

		glUniform4f(itLight->second->atten_UniformLocation,
					itLight->second->m_attenuation.x,
					itLight->second->m_attenuation.y,
					itLight->second->m_attenuation.z,
					itLight->second->m_attenuation.w);

		glUniform4f(itLight->second->direction_UniformLocation,
					itLight->second->m_direction.x,
					itLight->second->m_direction.y,
					itLight->second->m_direction.z,
					itLight->second->m_direction.w);

		glUniform4f(itLight->second->param1_UniformLocation,
					itLight->second->m_param1.x,
					itLight->second->m_param1.y,
					itLight->second->m_param1.z,
					itLight->second->m_param1.w);

		glUniform4f(itLight->second->param2_UniformLocation,
					itLight->second->m_param2.x,
					itLight->second->m_param2.y,
					itLight->second->m_param2.z,
					itLight->second->m_param2.w);
	}
}


