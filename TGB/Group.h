#include <string>
#include "Face.h"

#pragma once
class Group
{
public:
	Group();
	~Group();
	std::string name;
	std::string materialId;
	std::vector<Face*> faces;
};

