#pragma once
#include <glm/glm.hpp>
#include <glm/vec4.hpp>
#include <string>

class cLight {
public:
	std::string m_friendlyName;
	glm::vec4 m_position;
	glm::vec4 m_diffuse;
	bool m_useRGB;
	bool m_showModel;
	glm::vec4 m_specular;		// rgb = highlight colour, w = power
	glm::vec4 m_attenuation;	// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	glm::vec4 m_direction;		// Spot, directional lights
	/// <summary>
	/// x = lightType, y = inner angle, z = outer angle, w = TBD
	/// 0 = pointlight
	/// 1 = spot light
	/// 2 = directional light
	/// </summary>
	glm::vec4 m_param1;
	glm::vec4 m_param2;		// x = 0 for off, 1 for on

	// Also store the uniform locations from the shader here
	int position_UniformLocation;
	int diffuse_UniformLocation;
	int specular_UniformLocation;
	int atten_UniformLocation;
	int direction_UniformLocation;
	int param1_UniformLocation;
	int param2_UniformLocation;

	cLight();
	~cLight();

	void setConstantAttenuation(float newConstAtten);
	void setLinearAttenuation(float newLinearAtten);
	void setQuadraticAttenuation(float newQuadAtten);

	enum eLightType {
		POINT_LIGHT,
		SPOT_LIGHT,
		DIRECTIONAL_LIGHT
	};

	void TurnOn();
	void TurnOff();
	bool isOn();
	void SetToWhite();
	void SetColour(float r, float g, float b);
	void SetLightType(eLightType newLightType);
	void MakeItAPointLight();
	void MakeItASpotLight(glm::vec3 direction, float innerAngle, float outerAngle);
};

