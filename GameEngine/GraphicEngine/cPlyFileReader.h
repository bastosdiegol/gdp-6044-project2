#pragma once
#include <string>

/// <summary>
/// Struct containing all vertices from a ply model file
/// </summary>
struct sVertex_XYZ_N_RGBA_UV {
    float x, y, z;
    float nx, ny, nz;
    float red, green, blue, alpha;
    float texture_u, texture_v;
};

/// <summary>
/// Struct containing all indexes of vertices part of a triangle of the mesh
/// </summary>
struct sTrianglePLY {
    // The 3 vertex index values from the ply file
    unsigned int vertexIndices[3];
};

class cPlyFileReader {
public:
    sVertex_XYZ_N_RGBA_UV* pTheModelArray;
    sTrianglePLY* pTheModelTriangleArray;

    /// <summary>
    /// Stores total number of vertices of a model
    /// </summary>
    unsigned int m_numberOfVertices;
    /// <summary>
    /// Stores total number of triangles of a model
    /// </summary>
    unsigned int m_numberOfTriangles;

    /// <summary>
    /// Default Constructor
    /// </summary>
    cPlyFileReader();
    ~cPlyFileReader();

    /// <summary>
    /// Reads a Mesh File and store its Header, Vertices and Faces on a struct in memory
    /// </summary>
    /// <param name="file_location">Path to the file to be read</param>
    bool loadMeshFromFile(std::string file_location);
};