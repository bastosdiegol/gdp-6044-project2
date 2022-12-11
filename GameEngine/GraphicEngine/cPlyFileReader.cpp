#include "cPlyFileReader.h"

#include <fstream>
#include <sstream>
#include <iostream>

#ifdef _DEBUG
#define DEBUG_LOG_ENABLED
#endif
#ifdef DEBUG_LOG_ENABLED
#define DEBUG_PRINT(x, ...) printf(x, __VA_ARGS__)
#else
#define DEBUG_PRINT(x)
#endif

cPlyFileReader::cPlyFileReader() {
	DEBUG_PRINT("PlyFileReader::PlyFileReader()\n");
	m_numberOfVertices = -1;
	m_numberOfTriangles = -1;
	pTheModelArray = nullptr;
	pTheModelTriangleArray = nullptr;
}

cPlyFileReader::~cPlyFileReader() {
	DEBUG_PRINT("PlyFileReader::~PlyFileReader()\n");
}

bool cPlyFileReader::loadMeshFromFile(std::string file_location) {
	DEBUG_PRINT("PlyFileReader::loadMeshFromFile(%s)\n", file_location.c_str());
	// Loads the file
	std::ifstream theFile(file_location);
	// Checks if the file is open
	if (!theFile.is_open()) {
		std::cout << "File " << file_location << " could not be opened!" << std::endl;
		return false;
	}
	
	std::string theNextToken;
	// Iteration to Find the Total of Vertices
	do {
		theFile >> theNextToken;
	} while (theNextToken != "vertex");

	theFile >> theNextToken;
	// Sets the total of Vertices
	m_numberOfVertices = std::stoul(theNextToken);

	if (m_numberOfVertices <= 0) {
		DEBUG_PRINT("cPlyFireReader error: File %s has %d vertices.\nThis model won't be read.\n", file_location.c_str(), m_numberOfVertices);
		return false;
	}

	// Create new instance of Vertex array for the model
	pTheModelArray = new sVertex_XYZ_N_RGBA_UV[m_numberOfVertices];
	
	// Booleans to check existence of properties
	bool hasPosition = false;
	bool hasNormals  = false;
	bool hasColours  = false;
	bool hasTextures = false;
	// Iteration to Find the Total of Triangles
	// And Checks Properties Integrity
	do {
		theFile >> theNextToken;
		// Checks if has Position X Property
		if (theNextToken == "x")
			hasPosition = true;
		// Checks if has Normal X Property
		if (theNextToken == "nx")
			hasNormals = true;
		// Checks if has Normal X Property
		if (theNextToken == "red")
			hasColours = true;
		// Checks if has Texture U Property
		if (theNextToken == "texture_u")
			hasTextures = true;
	} while (theNextToken != "face");
	theFile >> theNextToken;

	// Checks if the file has vertices
	if (hasPosition == false) {
		DEBUG_PRINT("cPlyFireReader error: File %s has no vertices.\nThis model won't be read.\n", file_location.c_str());
		return false;
	}
	// Checks if the file has normals
	if (hasNormals == false) {
		DEBUG_PRINT("cPlyFireReader warning: File %s has no normals.\nVec3 will be set to 0.0f\n", file_location.c_str());
	}
	// Checks if the file has colours
	if (hasColours == false) {
		DEBUG_PRINT("cPlyFireReader warning: File %s has no colours.\nVec3 will be set to 1.0f\n", file_location.c_str());
	}
	// Checks if the file has textures
	if (hasTextures == false) {
		DEBUG_PRINT("cPlyFireReader warning: File %s has no textures.\nValues will be set to 0.0f\n", file_location.c_str());
	}

	// Sets the total of Faces
	m_numberOfTriangles = std::stoul(theNextToken);
	// Create a new instance of Triangles Array for the model
	pTheModelTriangleArray = new sTrianglePLY[m_numberOfTriangles];

	// Runs the file till reach "end_header" and be ready to read vertices
	do {
		theFile >> theNextToken;
	} while (theNextToken != "end_header");

	DEBUG_PRINT("Loading %s vertices ", file_location.c_str());
	// Iteration to read and store all vertices
	for (unsigned int i = 0; i < m_numberOfVertices; i++) {
		// Stores each property value per Vertice
		theFile >> pTheModelArray[i].x;
		theFile >> pTheModelArray[i].y;
		theFile >> pTheModelArray[i].z;

		if (hasNormals) {
			theFile >> pTheModelArray[i].nx;
			theFile >> pTheModelArray[i].ny;
			theFile >> pTheModelArray[i].nz;
		} else {
			pTheModelArray[i].nx = 0.0f;
			pTheModelArray[i].ny = 0.0f;
			pTheModelArray[i].nz = 0.0f;
		}

		if (hasColours) {
			theFile >> pTheModelArray[i].red;
			if (pTheModelArray[i].red > 1.0f)
				pTheModelArray[i].red /= 255;
			theFile >> pTheModelArray[i].green;
			if (pTheModelArray[i].green > 1.0f)
				pTheModelArray[i].green /= 255;
			theFile >> pTheModelArray[i].blue;
			if (pTheModelArray[i].blue > 1.0f)
				pTheModelArray[i].blue /= 255;
			theFile >> pTheModelArray[i].alpha;
			if (pTheModelArray[i].alpha > 1.0f)
				pTheModelArray[i].alpha /= 255;
		} else {
			pTheModelArray[i].red	= 1.0f;
			pTheModelArray[i].green = 1.0f;
			pTheModelArray[i].blue	= 1.0f;
			pTheModelArray[i].alpha = 1.0f;
		}

		if (hasTextures) {
			theFile >> pTheModelArray[i].texture_u;
			theFile >> pTheModelArray[i].texture_v;
		} else {
			pTheModelArray[i].texture_u = 0.0f;
			pTheModelArray[i].texture_v = 0.0f;
		}
	}
	DEBUG_PRINT("done!\n");


	DEBUG_PRINT("Loading %s triangle faces ", file_location.c_str());
	// Iteration to read and store all vertices
	for (unsigned int i = 0; i < m_numberOfTriangles; i++) {
		// First value of triangles is its ID that has no use
		unsigned int discard = 0;
		theFile >> discard;
		// Stores all vertices for each triangle
		theFile >> pTheModelTriangleArray[i].vertexIndices[0];
		theFile >> pTheModelTriangleArray[i].vertexIndices[1];
		theFile >> pTheModelTriangleArray[i].vertexIndices[2];
	}
	DEBUG_PRINT("done!\n");

	theFile.close();
	return true;
}
