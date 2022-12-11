#pragma once
#include <string>

// The vertex structure 
//	that will be passed to the GPU (Shader)
struct sVertex {
	float x, y, z;
	float r, g, b;
	float nx, ny, nz;
};

class cModel {
public:
	std::string meshName;

	unsigned int VAO_ID;

	// Vertices Data
	unsigned int VertexBufferID;
	unsigned int VertexBuffer_Start_Index;
	unsigned int numberOfVertices;

	// Triangle Indexes Data
	unsigned int IndexBufferID;
	unsigned int IndexBuffer_Start_Index;
	unsigned int numberOfIndices;
	unsigned int numberOfTriangles;

	// Vertices Structure
	sVertex* pVertices;
	unsigned int* pIndices;

	float min_x, max_x;
	float min_y, max_y;
	float min_z, max_z;


	cModel();
	~cModel();
};

