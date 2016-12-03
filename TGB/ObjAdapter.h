#pragma once
#include "Mesh.h"
#include "ShaderMesh.h"
#include "Material.h"
class ObjAdapter
{
public:
	ObjAdapter();
	~ObjAdapter();
	static ShaderMesh* adapt(Mesh* objMesh);
};

