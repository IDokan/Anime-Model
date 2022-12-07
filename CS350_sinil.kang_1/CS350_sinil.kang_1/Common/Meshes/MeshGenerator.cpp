/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: MeshGenerator.cpp
Purpose: MeshGenerator procedually generates Sphere and Orbit
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS300_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 9/28/2021
End Header --------------------------------------------------------*/
#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/glm.hpp>
#include <../Common/Meshes/MeshGenerator.h>
#include <../Common/Meshes/Mesh.h>

void MeshGenerator::GenerateSphereMesh(Mesh& mesh, const float radius, const int sectionCount)
{
	// Clear mesh and add first top vertex
	mesh.vertexBuffer.clear();
	mesh.vertexBuffer.push_back(glm::vec3{ 0.f, 0.f, radius });

	const float stepPhi = static_cast<float>(M_PI) / sectionCount;
	const float M_2PI = static_cast<float>(M_PI) * 2.f;
	const float stepTheta = M_2PI / sectionCount;
	for (float phi = static_cast<float>(M_PI_2) - stepPhi; phi > -M_PI_2; phi -= stepPhi)
	{
		for (float theta = 0; theta < M_2PI; theta += stepTheta)
		{
			glm::vec3 point;
			point.x = radius * cos(phi) * cos(theta);
			point.y = radius * cos(phi) * sin(theta);
			point.z = radius * sin(phi);
			mesh.vertexBuffer.push_back(point);
		}
	}

	// Add last vertex
	mesh.vertexBuffer.push_back(glm::vec3{ 0.f, 0.f, -radius });

	AddIndices(mesh, sectionCount);

	glm::vec3 minVertex = glm::vec3(-radius);
	glm::vec3 maxVertex = glm::vec3(radius);
	mesh.boundingBox[0] = minVertex;
	mesh.boundingBox[1] = maxVertex;

	glm::vec3 box = maxVertex - minVertex;
	box = box / 30.f;
	mesh.setNormalLength(glm::length(box));
	mesh.calcVertexNormals();
	mesh.calcUVs(Mesh::UVType::CYLINDRICAL_UV);
}

void MeshGenerator::GenerateOrbitMesh(Mesh& mesh, const float radius, const int sectionCount)
{
	mesh.vertexBuffer.clear();

	const float M_2PI = static_cast<float>(M_PI) * 2.f;
	for (int i = 0; i < sectionCount; i++)
	{
		float currentX = radius * cos(M_2PI / sectionCount * i);
		float currentZ = radius * sin(M_2PI / sectionCount * i);
		mesh.vertexBuffer.push_back(glm::vec3(currentX, 0.f, currentZ));

		float nextX = radius * cos(M_2PI / sectionCount * (i + 1));
		float nextZ = radius * sin(M_2PI / sectionCount * (i + 1));
		mesh.vertexBuffer.push_back(glm::vec3(nextX, 0.f, nextZ));
	}
}

void MeshGenerator::GenerateCubeMesh(Mesh& mesh, const float length, const unsigned int sectionCount)
{
	mesh.vertexBuffer.clear();

	float edgeLength = length / sectionCount;

	glm::vec3 vertex = glm::vec3(-length / 2.f);
	const glm::vec3 startPosition = vertex;

	for (unsigned int depthIndex = 0; depthIndex <= sectionCount; depthIndex++)
	{
		for (unsigned int heightIndex = 0; heightIndex <= sectionCount; heightIndex++)
		{
			// X-axis vertices
			for (unsigned int widthIndex = 0; widthIndex <= sectionCount; widthIndex++)
			{
				mesh.vertexBuffer.push_back(vertex);
				vertex.x += edgeLength;
			}
			vertex.x = startPosition.x;
			vertex.y += edgeLength;
		}

		vertex.y = startPosition.y;
		vertex.z += edgeLength;
	}

	mesh.vertexNormals = mesh.vertexBuffer;

	const unsigned int vertexCount = sectionCount + 1;
	const unsigned int vertexCountSquare = vertexCount * vertexCount;
	// Bottom face
	for (unsigned int i = 0; i < sectionCount; i++)
	{
		for (unsigned int j = 0; j < sectionCount; j++)
		{
			mesh.vertexIndices.push_back(i + j * vertexCount);
			mesh.vertexIndices.push_back((i + 1) + j * vertexCount);
			mesh.vertexIndices.push_back(i + (j + 1) * vertexCount);

			mesh.vertexIndices.push_back((i + 1) + j * vertexCount);
			mesh.vertexIndices.push_back((i + 1) + (j + 1) * vertexCount);
			mesh.vertexIndices.push_back(i + (j + 1) * vertexCount);
		}
	}

	// Up face
	for (unsigned int i = 0; i < sectionCount; i++)
	{
		for (unsigned int j = 0; j < sectionCount; j++)
		{
			mesh.vertexIndices.push_back((i + 1) + j * vertexCount + sectionCount * vertexCountSquare);
			mesh.vertexIndices.push_back(i + j * vertexCount + sectionCount * vertexCountSquare);
			mesh.vertexIndices.push_back(i + (j + 1) * vertexCount + sectionCount * vertexCountSquare);

			mesh.vertexIndices.push_back((i + 1) + (j + 1) * vertexCount + sectionCount * vertexCountSquare);
			mesh.vertexIndices.push_back((i + 1) + j * vertexCount + sectionCount * vertexCountSquare);
			mesh.vertexIndices.push_back(i + (j + 1) * vertexCount + sectionCount * vertexCountSquare);
		}
	}

	// right face
	for (unsigned int z = 0; z < sectionCount; z++)
	{
		for (unsigned int j = 0; j < sectionCount; j++)
		{
			mesh.vertexIndices.push_back(sectionCount + j * vertexCount + z * vertexCountSquare);
			mesh.vertexIndices.push_back(sectionCount + j * vertexCount + (z + 1) * vertexCountSquare);
			mesh.vertexIndices.push_back(sectionCount + (j + 1) * vertexCount + (z + 1) * vertexCountSquare);

			mesh.vertexIndices.push_back(sectionCount + (j + 1) * vertexCount + (z + 1) * vertexCountSquare);
			mesh.vertexIndices.push_back(sectionCount + (j + 1) * vertexCount + z * vertexCountSquare);
			mesh.vertexIndices.push_back(sectionCount + j * vertexCount + z * vertexCountSquare);
		}
	}

	// left face
	for (unsigned int z = 0; z < sectionCount; z++)
	{
		for (unsigned int j = 0; j < sectionCount; j++)
		{
			mesh.vertexIndices.push_back(j * vertexCount + z * vertexCountSquare);
			mesh.vertexIndices.push_back((j + 1) * vertexCount + z * vertexCountSquare);
			mesh.vertexIndices.push_back((j + 1) * vertexCount + (z + 1) * vertexCountSquare);

			mesh.vertexIndices.push_back(j * vertexCount + z * vertexCountSquare);
			mesh.vertexIndices.push_back((j + 1) * vertexCount + (z + 1) * vertexCountSquare);
			mesh.vertexIndices.push_back(j * vertexCount + (z + 1) * vertexCountSquare);
		}
	}

	// front face
	for (unsigned int z = 0; z < sectionCount; z++)
	{
		for (unsigned int i = 0; i < sectionCount; i++)
		{
			mesh.vertexIndices.push_back((i + 1) + sectionCount * vertexCount + z * vertexCountSquare);
			mesh.vertexIndices.push_back((i + 1) + sectionCount * vertexCount + (z + 1) * vertexCountSquare);
			mesh.vertexIndices.push_back(i + sectionCount * vertexCount + (z + 1) * vertexCountSquare);

			mesh.vertexIndices.push_back(i + sectionCount * vertexCount + (z + 1) * vertexCountSquare);
			mesh.vertexIndices.push_back(i + sectionCount * vertexCount + z * vertexCountSquare);
			mesh.vertexIndices.push_back((i + 1) + sectionCount * vertexCount + z * vertexCountSquare);
		}
	}

	// back face
	for (unsigned int z = 0; z < sectionCount; z++)
	{
		for (unsigned int i = 0; i < sectionCount; i++)
		{
			mesh.vertexIndices.push_back((i + 1) + z * vertexCountSquare);
			mesh.vertexIndices.push_back(i + z * vertexCountSquare);
			mesh.vertexIndices.push_back((i + 1) + (z + 1) * vertexCountSquare);

			mesh.vertexIndices.push_back((i + 1) + (z + 1) * vertexCountSquare);
			mesh.vertexIndices.push_back(i + z * vertexCountSquare);
			mesh.vertexIndices.push_back(i + (z + 1) * vertexCountSquare);
		}
	}
}

void MeshGenerator::AddIndices(Mesh& mesh, const int sectionCount)
{
	const int beginVertexIndex = 0;
	const int endVertexIndex = static_cast<int>(mesh.vertexBuffer.size()) - 1;
	const int previousIndexOfEndVertex = endVertexIndex - 1;
	// Begin faces
	for (int i = 1; i <= sectionCount; ++i)
	{
		mesh.vertexIndices.push_back(beginVertexIndex);
		mesh.vertexIndices.push_back(i);
		const int lastIndex = (i + 1 > sectionCount) ? (i + 1 - sectionCount) : (i + 1);
		mesh.vertexIndices.push_back(lastIndex);
	}

	for (int floor = 1; floor <= endVertexIndex - (2 * sectionCount); floor += sectionCount)
	{
		for (int i = 0; i < sectionCount; ++i)
		{
			// First part of Middle faces
			const int firstIndexOfLeftFace = i + floor;
			mesh.vertexIndices.push_back(firstIndexOfLeftFace);
			mesh.vertexIndices.push_back(firstIndexOfLeftFace + sectionCount);
			const int lastIndex = (firstIndexOfLeftFace + 1 >= floor + sectionCount) ? (firstIndexOfLeftFace + 1 - sectionCount) : (firstIndexOfLeftFace + 1);
			mesh.vertexIndices.push_back(lastIndex);


			// Second part of Middle faces
			const int firstIndexOfRightFace = firstIndexOfLeftFace + sectionCount;
			mesh.vertexIndices.push_back(firstIndexOfRightFace);
			const int secondIndex = (firstIndexOfRightFace + 1 >= floor + (2 * sectionCount)) ? (firstIndexOfRightFace + 1 - sectionCount) : (firstIndexOfRightFace + 1);
			mesh.vertexIndices.push_back(secondIndex);
			mesh.vertexIndices.push_back(lastIndex);
		}
	}


	// End faces
	for (int i = sectionCount * (sectionCount - 2) + 1; i <= (previousIndexOfEndVertex); ++i)
	{
		mesh.vertexIndices.push_back(i);
		mesh.vertexIndices.push_back(endVertexIndex);
		const int lastIndex = (i + 1 > previousIndexOfEndVertex) ? (i + 1 - sectionCount) : (i + 1);
		mesh.vertexIndices.push_back(lastIndex);
	}
}
