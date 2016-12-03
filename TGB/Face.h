#include <vector>

#include "Vertex.h"
#include "TextureMapping.h"

#pragma once

class Face
{
public:
	Face();
	~Face();
	std::vector<int> vertex;
	std::vector<int> normals;
	std::vector<int> mappings;
};

