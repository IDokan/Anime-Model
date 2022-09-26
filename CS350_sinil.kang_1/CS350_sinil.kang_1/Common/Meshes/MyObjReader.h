/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: MyObjReader.h
Purpose: My Obj Reader
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS300_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 9/22/2021
End Header --------------------------------------------------------*/

#include <filesystem>
#include <../Common/Meshes/Mesh.h>

class MyObjReader
{
public:
	MyObjReader();
	virtual ~MyObjReader();

	void ReadObjFile(std::filesystem::path path, Mesh* mesh, bool shouldNormalFlip = false, Mesh::UVType uvType = Mesh::UVType::SPHERICAL_UV);

private:
	char const * DELIMITERS = " \r\n\t";
	
	void ParseLine(char* line, Mesh* mesh);

	void AddVertex(Mesh* mesh);
	void AddIndex(Mesh* mesh);
	void AddVertexNormal(Mesh* mesh);

	void HandleVertexTypes(char secondChar, Mesh* mesh);

	void InitData();

	glm::vec3 maxVertex;
	glm::vec3 minVertex;
};