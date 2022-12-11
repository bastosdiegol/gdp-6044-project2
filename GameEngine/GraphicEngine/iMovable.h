#pragma once
#include <glm/vec3.hpp>
#include "Vector3.h"

class iMovable {
public:
	virtual ~iMovable() {};

	virtual glm::vec3	getGLMPosition() = 0;
	virtual Vector3		getVECPosition() = 0;
	virtual void		setPosition(glm::vec3 pos) = 0;
	virtual void		setPosition(Vector3 pos)   = 0;
};