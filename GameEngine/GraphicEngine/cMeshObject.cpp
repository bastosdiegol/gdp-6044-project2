#include "cMeshObject.h"

#ifdef _DEBUG
#define DEBUG_LOG_ENABLED
#endif
#ifdef DEBUG_LOG_ENABLED
#define DEBUG_PRINT(x, ...) printf(x, __VA_ARGS__)
#else
#define DEBUG_PRINT(x)
#endif


//static 
unsigned int cMeshObject::nextID = cMeshObject::STARTING_ID;

cMeshObject::cMeshObject(cModel* parent) {
	DEBUG_PRINT("cMeshObject::cMeshObject(%s)\n", parent->meshName.c_str());
	this->m_parentModel = parent;
	this->m_position = glm::vec3(0.0f);
	this->m_rotation = glm::vec3(0.0f);
	this->m_scale = 1.0f;
	this->m_bUse_RGBA_colour = false;
	this->m_RGBA_colour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	this->m_isWireframe = false;
	this->m_bDoNotLight = false;
	this->m_bIsVisible = true;
	this->m_myID = cMeshObject::nextID;
	cMeshObject::nextID += 1;
}

cMeshObject::~cMeshObject() {
	DEBUG_PRINT("cMeshObject::~cMeshObject()\n");
}

glm::vec3 cMeshObject::getGLMPosition() {
	return m_position;
}

Vector3 cMeshObject::getVECPosition() {
	return Vector3(m_position);
}

void cMeshObject::setPosition(glm::vec3 pos) {
	m_position = pos;
}

void cMeshObject::setPosition(Vector3 pos) {
	m_position = pos.GetGLM();
}
