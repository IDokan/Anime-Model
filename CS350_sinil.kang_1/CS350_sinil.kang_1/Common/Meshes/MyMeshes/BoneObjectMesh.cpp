/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: BoneObjectMesh.cpp
Purpose: Class of ObjectMesh. All object in the scene is drawn by this class
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS460_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 10/2/2022
End Header --------------------------------------------------------*/

#include "BoneObjectMesh.h"
#include <iostream>

BoneObjectMesh::BoneObjectMesh()
	:MyMesh(), normalBuffer(0), uvBuffer(0), indexBuffer(0), boneIndexBuffer(0), boneWeightBuffer(0)
{
}

BoneObjectMesh::~BoneObjectMesh()
{
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &normalBuffer);
	glDeleteBuffers(1, &uvBuffer);
	glDeleteBuffers(1, &indexBuffer);
	glDeleteBuffers(1, &boneIndexBuffer);
	glDeleteBuffers(1, &boneWeightBuffer);

	glDeleteVertexArrays(1, &VAO);
}

bool BoneObjectMesh::Init(int vertexCount, GLfloat* vertices, GLfloat* normals, GLfloat* uvs, GLubyte* boneIndices, GLfloat* weights, int indexCount, GLuint* indices)
{
	if (VAO == 0)
	{
		glGenVertexArrays(1, &VAO);
	}
	else
	{
		glDeleteVertexArrays(1, &VAO);
		glGenVertexArrays(1, &VAO);
	}
	glBindVertexArray(VAO);

	if (VBO == 0)
	{
		glGenBuffers(1, &VBO);
	}
	else
	{
		glDeleteBuffers(1, &VBO);
		glGenBuffers(1, &VBO);
	}
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * vertexCount, vertices, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);

	if (normalBuffer == 0)
	{
		glGenBuffers(1, &normalBuffer);
	}
	else
	{
		glDeleteBuffers(1, &normalBuffer);
		glGenBuffers(1, &normalBuffer);
	}
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * vertexCount, normals, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);

	if (uvBuffer == 0)
	{
		glGenBuffers(1, &uvBuffer);
	}
	else
	{
		glDeleteBuffers(1, &uvBuffer);
		glGenBuffers(1, &uvBuffer);
	}
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * vertexCount, uvs, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, nullptr);

	if (boneIndexBuffer == 0)
	{
		glGenBuffers(1, &boneIndexBuffer);
	}
	else
	{
		glDeleteBuffers(1, &boneIndexBuffer);
		glGenBuffers(1, &boneIndexBuffer);
	}
	glBindBuffer(GL_ARRAY_BUFFER, boneIndexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLubyte) * 4 * vertexCount, boneIndices, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 4, GL_UNSIGNED_BYTE, sizeof(GLubyte) * 4, nullptr);

	if (boneWeightBuffer == 0)
	{
		glGenBuffers(1, &boneWeightBuffer);
	}
	else
	{
		glDeleteBuffers(1, &boneWeightBuffer);
		glGenBuffers(1, &boneWeightBuffer);
	}
	glBindBuffer(GL_ARRAY_BUFFER, boneWeightBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * vertexCount, weights, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, nullptr);

	if (indexBuffer == 0)
	{
		glGenBuffers(1, &indexBuffer);
	}
	else
	{
		glDeleteBuffers(1, &indexBuffer);
		glGenBuffers(1, &indexBuffer);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_DYNAMIC_DRAW);

	return true;
}

void BoneObjectMesh::PrepareDrawing()
{
	glUseProgram(programIDReference);

	glBindVertexArray(VAO);
	if (uniformBlockBuffer != 0)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, uniformBlockBuffer);
	}

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
}

void BoneObjectMesh::Draw(int indexCount)
{
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
}
