/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: MyObjReader.cpp
Purpose: My Obj Reader
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS300_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 9/22/2021
End Header --------------------------------------------------------*/
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include <../Common/Meshes/MyObjReader.h>

MyObjReader::MyObjReader()
	:maxVertex(glm::vec3(-FLT_MAX)), minVertex(glm::vec3(FLT_MAX))
{
}

MyObjReader::~MyObjReader()
{
}

void MyObjReader::ReadObjFile(std::filesystem::path path, Mesh* mesh, bool shouldNormalFlip, Mesh::UVType uvType)
{
	InitData();

	std::ifstream objFile;
	objFile.open(path);

	if (objFile.bad() || objFile.eof() || objFile.fail())
	{
		// File is not opened normally.
		std::cout << "File is not opened normally\n" << "File path is " << path << std::endl;
		return;
	}

	constexpr unsigned int readBufferSize = 256;
	while (!objFile.eof())
	{
		char buffer[readBufferSize] = "";
		objFile.getline(buffer, readBufferSize, '\n');
		ParseLine(buffer, mesh);
	}

	mesh->boundingBox[0] = minVertex;
	mesh->boundingBox[1] = maxVertex;

	glm::vec3 box = maxVertex - minVertex;
	box = box / 30.f;
	mesh->setNormalLength(glm::length(box));
	mesh->calcUVs(uvType);
	mesh->calcVertexNormals(shouldNormalFlip);
}

void MyObjReader::ParseLine(char* line, Mesh* mesh)
{

	char* token = strtok(line, DELIMITERS);
	// Empty line will be skipped..
	if (token == NULL)
	{
		return;
	}

	// First token should be a single alphabet
	switch (token[0])
	{
	case 'v':
		HandleVertexTypes(token[1], mesh);
		break;
	case 'f':
		AddIndex(mesh);
		break;
	case 'm':
	case 'u':
		// mtllib / usemtl -> Pointer to material data (*.mtl file)
		break;
	case 'o':
		// Object specifier
		break;
	case 'g':
		// Group specifier, useful to store an object in its logical parts.
		break;
	default:
		return;
	}
}

void MyObjReader::AddVertex(Mesh* mesh)
{
	glm::vec3 vertex;
	char* token = strtok(NULL, DELIMITERS);
	vertex.x = static_cast<float>(atof(token));
	token = strtok(NULL, DELIMITERS);
	vertex.y = static_cast<float>(atof(token));
	token = strtok(NULL, DELIMITERS);
	vertex.z = static_cast<float>(atof(token));

	// Update max & min vertex for bounding box
	maxVertex.x = std::max(maxVertex.x, vertex.x);
	maxVertex.y = std::max(maxVertex.y, vertex.y);
	maxVertex.z = std::max(maxVertex.z, vertex.z);

	minVertex.x = std::min(minVertex.x, vertex.x);
	minVertex.y = std::min(minVertex.y, vertex.y);
	minVertex.z = std::min(minVertex.z, vertex.z);

	mesh->vertexBuffer.emplace_back(vertex);
}

void MyObjReader::AddIndex(Mesh* mesh)
{
	// Substitute by 1 because human readable obj file starts from 1 while computer starts from 0
	char* token = strtok(NULL, DELIMITERS);
	GLuint firstIndex = atoi(token) - 1;
	mesh->vertexIndices.emplace_back(firstIndex);
	token = strtok(NULL, DELIMITERS);
	GLuint secondIndex = atoi(token) - 1;
	mesh->vertexIndices.emplace_back(secondIndex);
	token = strtok(NULL, DELIMITERS);
	GLuint thirdIndex = atoi(token) - 1;
	mesh->vertexIndices.emplace_back(thirdIndex);


	// When complex index came up,
	token = strtok(nullptr, DELIMITERS);
	while (token != nullptr)
	{
		secondIndex = thirdIndex;
		thirdIndex = atoi(token) - 1;

		mesh->vertexIndices.emplace_back(firstIndex);
		mesh->vertexIndices.emplace_back(secondIndex);
		mesh->vertexIndices.emplace_back(thirdIndex);

		token = strtok(nullptr, DELIMITERS);
	}
}

void MyObjReader::AddVertexNormal(Mesh* mesh)
{
	glm::vec3 normal;
	char* token = strtok(NULL, DELIMITERS);
	normal.x = static_cast<float>(atof(token));
	token = strtok(NULL, DELIMITERS);
	normal.y = static_cast<float>(atof(token));
	token = strtok(NULL, DELIMITERS);
	normal.z = static_cast<float>(atof(token));

	mesh->vertexNormals.push_back(normal);
}

void MyObjReader::HandleVertexTypes(char secondChar, Mesh* mesh)
{
	switch (secondChar)
	{
	case '\0':
		AddVertex(mesh);
		break;
	case 't':
		// Texture coordinates(2D)
		break;
	case 'n':
		AddVertexNormal(mesh);
		break;
	default:
		break;
	}
}

void MyObjReader::InitData()
{
	maxVertex = (glm::vec3(-FLT_MAX));
	minVertex = (glm::vec3(FLT_MAX));
}
