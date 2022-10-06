/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: BoneObjectMesh.h
Purpose: Class of ObjectMesh. All object in the scene is drawn by this class
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS460_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 10/2/2022
End Header --------------------------------------------------------*/
#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <../Common/Meshes/MyMeshes/MyMesh.h>

class BoneObjectMesh : public MyMesh
{
public:
	BoneObjectMesh();
	virtual ~BoneObjectMesh();

	bool Init(int vertexCount, GLfloat* vertices, GLfloat* normals, GLfloat* uvs, GLubyte* boneIndices, GLfloat* weights,
		int indexCount, GLuint* indices);
	void PrepareDrawing();
	void Draw(int indexCount);

private:
	GLuint normalBuffer;
	GLuint uvBuffer;
	GLuint indexBuffer;
	GLuint boneIndexBuffer;
	GLuint boneWeightBuffer;
};