#include "cLight.h"

#ifdef _DEBUG
#define DEBUG_LOG_ENABLED
#endif
#ifdef DEBUG_LOG_ENABLED
#define DEBUG_PRINT(x, ...) printf(x, __VA_ARGS__)
#else
#define DEBUG_PRINT(x)
#endif

cLight::cLight() {
	DEBUG_PRINT("cLight::cLight()\n");
	this->m_position	= glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	this->m_diffuse		= glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	this->m_specular	= glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	this->m_attenuation = glm::vec4(0.01f, 0.01f, 0.0f, 1.0f);
	this->m_direction	= glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	this->m_param1		= glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	this->m_param2		= glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	this->m_useRGB		= false;
	this->m_showModel	= true;
}

cLight::~cLight() {
	DEBUG_PRINT("cLight::~cLight()\n");
}

void cLight::setConstantAttenuation(float newConstAtten) {
	this->m_attenuation.x = newConstAtten;
	return;
}
void cLight::setLinearAttenuation(float newLinearAtten) {
	this->m_attenuation.y = newLinearAtten;
	return;
}

void cLight::setQuadraticAttenuation(float newQuadAtten) {
	this->m_attenuation.z = newQuadAtten;
	return;
}

void cLight::TurnOn() {
	DEBUG_PRINT("cLight::TurnOn()\n");
	this->m_param2.x = 1;
	return;
}

void cLight::TurnOff() {
	DEBUG_PRINT("cLight::TurnOff()\n");
	this->m_param2.x = 0;
	return;
}

bool cLight::isOn() {
	if (this->m_param2.x == 1)
		return true;
	else
		return false;
}

void cLight::SetToWhite() {
	DEBUG_PRINT("cLight::SetToWhite()\n");
	this->m_specular.r = 1.0f;
	this->m_specular.g = 1.0f;
	this->m_specular.b = 1.0f;
	this->m_specular.a = 1.0f;
}

void cLight::SetColour(float r, float g, float b) {
	DEBUG_PRINT("cLight::SetColour(%f, %f, %f)\n", r,g,b);
	this->m_specular.r = r;
	this->m_specular.g = g;
	this->m_specular.b = b;
	this->m_specular.a = 1.0f;
}

void cLight::SetLightType(eLightType newLightType) {
	DEBUG_PRINT("cLight::SetLightType(%d)\n", newLightType);
	switch (newLightType) {
	case cLight::POINT_LIGHT:
		this->m_param1.x = 0;
		break;
	case cLight::SPOT_LIGHT:
		this->m_param1.x = 1;
		break;
	case cLight::DIRECTIONAL_LIGHT:
		this->m_param1.x = 2;
		break;
	}
}
