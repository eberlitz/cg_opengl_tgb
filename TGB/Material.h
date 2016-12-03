#include <vector>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#pragma once

class Material
{
public:
	Material();
	~Material();

	float ka[3];
	float ks[3];
	float kd[3];
	float ns;
	GLuint textureId;
	std::string map_kd;
};

