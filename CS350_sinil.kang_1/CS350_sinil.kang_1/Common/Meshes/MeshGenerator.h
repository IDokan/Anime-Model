/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: MeshGenerator.h
Purpose: MeshGenerator procedually generates Sphere and Orbit
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS300_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 9/28/2021
End Header --------------------------------------------------------*/
#pragma once

class Mesh;
class MeshGenerator
{
public:
	static void GenerateSphereMesh(Mesh& mesh, const float radius, const int sectionCount);

	// Generate orbit on XY-plane
	static void GenerateOrbitMesh(Mesh& mesh, const float radius, const int sectionCount);
	static void GenerateCubeMesh(Mesh& mesh, const float length, const int sectionCount);
private:
	static void AddIndices(Mesh& mesh, const int sectionCount);
};