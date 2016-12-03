#include <vector>

#include "MeshGroup.h"

#pragma once
class ShaderMesh
{
public:
	ShaderMesh();
	~ShaderMesh();
	std::vector<MeshGroup*> meshGroups;
};

