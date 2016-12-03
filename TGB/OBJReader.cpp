#ifndef __APPLE__
    #include <windows.h>
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include "OBJReader.h"
#include "Material.h"
#include <algorithm>

using namespace std;

OBJReader::OBJReader()
{
}


OBJReader::~OBJReader()
{
}

const int BUFFSIZE = 80;

// http://www.dreamincode.net/forums/topic/95826-stringstream-tutorial/
Mesh* OBJReader::readFromPath(char* path) {
	ifstream file(path);
	if (!file.is_open()) {
		exit(EXIT_FAILURE);
	}
	char buffer[BUFFSIZE];
	stringstream ss;	
	Mesh* mesh = new Mesh();

	string line;
	// Grupo corrente
	Group* g = new Group();
	g->name = "default";
	mesh->allGroups.push_back(g);

	// Seta o valor default do nome do arquivo mtl para ser o nome do arquivo obj com a extensão mtl
	istringstream sPath(path);
	string filename = sPath.str();
	mesh->mtllib = filename.substr(0, filename.find_last_of('.')) + ".mtl";

	while (getline(file, line)) {
		std::transform(line.begin(), line.end(), line.begin(), ::tolower);
		if (line.substr(0, 7) == "mtllib ") {
			istringstream s(line.substr(7));
			mesh->mtllib = s.str();
		} else if (line.substr(0, 2) == "g ") {
			istringstream s(line.substr(2));
			g = new Group();
			g->name = s.str();
			mesh->allGroups.push_back(g);
		} else if (line.substr(0, 7) == "usemtl ") {
			istringstream s(line.substr(7));
			g->materialId = s.str();
		} else if (line.substr(0, 2) == "v ") {
			istringstream s(line.substr(2));
			Vertex* v = new Vertex(); 
			s >> v->v3[0];
			s >> v->v3[1];
			s >> v->v3[2];
			mesh->allVertex.push_back(v);
		}
		else if (line.substr(0, 3) == "vn ") {
			istringstream s(line.substr(3));
			Vertex* v = new Vertex();
			s >> v->v3[0];
			s >> v->v3[1];
			s >> v->v3[2];
			mesh->allNormals.push_back(v);
		}
		else if (line.substr(0, 3) == "vt ") {
			istringstream s(line.substr(3));
			TextureMapping* t = new TextureMapping();
			s >> t->v2[0];
			s >> t->v2[1];
			mesh->allMappings.push_back(t);
		}
		else if (line.substr(0, 2) == "f ") {
			Face* face = new Face();
			g->faces.push_back(face);
			istringstream s(line.substr(2));
			while (s.getline(buffer, 30, ' ')) {
				istringstream ss(buffer);
				int i = 0;
				while (ss.getline(buffer, 30, '/')) {
					// pode não ter o mapping (a == 1) informado
					try
					{
						int a = atoi(buffer) - 1;
						if (i == 0) {
							face->vertex.push_back(a);
						}
						else if (i == 1) {
							face->mappings.push_back(a);
						}
						else if (i == 2) {
							face->normals.push_back(a);
						}
					}
					catch (const std::exception&){}
					i++;
				}
			}
		}
		else if (line[0] == '#') {/*Ignora*/}
		// [x] - #				// comentario
		// [x] - v				// vertices
		// [x] - vn				// normais
		// [x] - vt				// mapeamentos de textura
		// [x] - f				// faces ( 'v1 v2 v...' ou 'v1/t1/n1 v2/t2/n2 ...' ou 'v1//n1 v2//n2 ...' ou 'v1/t1 v2/t2 ...' )
		// [x] - g				// grupo (opcional)
		// [x] - mtllib <file>	// Arquivo de materiais
		// [x] - usemtl <id>	// id do material associado a um grupo
	}
	file.close();
	return mesh;
}

std::map<std::string, Material*> OBJReader::readMtlFrom(const std::string& path) {

	ifstream file(path);
	if (!file.is_open()) {
		exit(EXIT_FAILURE);
	}
	std::stringstream ss;
	map<string, Material*> materialsMap;

	string line;
	Material* m = new Material();
	while (getline(file, line)) {
		std::transform(line.begin(), line.end(), line.begin(), ::tolower);
		if (line.substr(0, 7) == "newmtl ") {
			istringstream s(line.substr(7));
			materialsMap[s.str()] = m = new Material();
		}
		else if (line.substr(0, 3) == "ka ") {
			istringstream s(line.substr(3));
			s >> m->ka[0];
			s >> m->ka[1];
			s >> m->ka[2];
		}
		else if (line.substr(0, 3) == "ks ") {
			istringstream s(line.substr(3));
			s >> m->ks[0];
			s >> m->ks[1];
			s >> m->ks[2];
		}
		else if (line.substr(0, 3) == "kd ") {
			istringstream s(line.substr(3));
			s >> m->kd[0];
			s >> m->kd[1];
			s >> m->kd[2];
		}
		else if (line.substr(0, 3) == "ns ") {
			istringstream s(line.substr(3));
			s >> m->ns;
		}
		else if (line.substr(0, 7) == "map_kd ") {
			istringstream s(line.substr(7));
			m->map_kd = s.str();
			GLuint textureID;
			LoadBMP(m->map_kd, textureID);
			m->textureId = textureID;
		}
	}

	file.close();
	return materialsMap;
}

int OBJReader::LoadBMP(std::string location, GLuint& texture) {
//	unsigned char* datBuff[2] = { nullptr, nullptr }; // Header buffers
//
//	unsigned char* pixels = nullptr; // Pixels
//
//	BITMAPFILEHEADER* bmpHeader = nullptr; // Header
//	BITMAPINFOHEADER* bmpInfo = nullptr; // Info 
//
//										 // The file... We open it with it's constructor
//	std::ifstream file(location, std::ios::binary);
//	if (!file)
//	{
//		std::cout << "Failure to open bitmap file.\n";
//
//		return 1;
//	}
//
//	// Allocate byte memory that will hold the two headers
//	datBuff[0] = new unsigned char[sizeof(BITMAPFILEHEADER)];
//	datBuff[1] = new unsigned char[sizeof(BITMAPINFOHEADER)];
//
//	file.read((char*)datBuff[0], sizeof(BITMAPFILEHEADER));
//	file.read((char*)datBuff[1], sizeof(BITMAPINFOHEADER));
//
//	// Construct the values from the buffers
//	bmpHeader = (BITMAPFILEHEADER*)datBuff[0];
//	bmpInfo = (BITMAPINFOHEADER*)datBuff[1];
//
//	// Check if the file is an actual BMP file
//	if (bmpHeader->bfType != 0x4D42)
//	{
//		std::cout << "File \"" << location << "\" isn't a bitmap file\n";
//		return 2;
//	}
//	// First allocate pixel memory
//	pixels = new unsigned char[bmpInfo->biSizeImage];
//
//	// Go to where image data starts, then read in image data
//	file.seekg(bmpHeader->bfOffBits);
//	file.read((char*)pixels, bmpInfo->biSizeImage);
//	// We're almost done. We have our image loaded, however it's not in the right format.
//	// .bmp files store image data in the BGR format, and we have to convert it to RGB.
//	// Since we have the value in bytes, this shouldn't be to hard to accomplish
//	unsigned char tmpRGB = 0; // Swap buffer
//	for (unsigned long i = 0; i < bmpInfo->biSizeImage; i += 3)
//	{
//		tmpRGB = pixels[i];
//		pixels[i] = pixels[i + 2];
//		pixels[i + 2] = tmpRGB;
//	}
//
//	// Set width and height to the values loaded from the file
//	GLuint w = bmpInfo->biWidth;
//	GLuint h = bmpInfo->biHeight;
//	/*******************GENERATING TEXTURES*******************/
//
//	glGenTextures(1, &texture);             // Generate a texture
//	glBindTexture(GL_TEXTURE_2D, texture); // Bind that texture temporarily
//
//	GLint mode = GL_RGB;                   // Set the mode
//
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//
//	// Create the texture. We get the offsets from the image, then we use it with the image's
//	// pixel data to create it.
//	glTexImage2D(GL_TEXTURE_2D, 0, mode, w, h, 0, mode, GL_UNSIGNED_BYTE, pixels);
//
//	// Unbind the texture
//	//glBindTexture(GL_TEXTURE_2D, NULL);
//
//	// Output a successful message
//	std::cout << "Texture \"" << location << "\" successfully loaded.\n";
//
//	// Delete the two buffers.
//	delete[] datBuff[0];
//	delete[] datBuff[1];
//	delete[] pixels;

	return 0; // Return success code 
}
