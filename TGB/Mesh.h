#include <vector>
#include <string>
#include "Vertex.h"
#include "TextureMapping.h"
#include "Group.h"
#pragma once

class Mesh
{
public:
	Mesh();
	~Mesh();
	std::string mtllib;
	std::vector<Vertex*> allVertex;
	std::vector<Vertex*> allNormals;
	std::vector<TextureMapping*> allMappings;
	std::vector<Group*> allGroups;
};

