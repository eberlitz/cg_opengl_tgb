#include <vector>
#include <map>
#include <string>
#include "Mesh.h"
#include "Material.h"
#pragma once



class OBJReader
{
public:
	OBJReader();
	~OBJReader();
	static Mesh* readFromPath(char* path);
	static std::map<std::string, Material*> readMtlFrom(const std::string& path);
	static int LoadBMP(std::string location, GLuint& texture);
};

