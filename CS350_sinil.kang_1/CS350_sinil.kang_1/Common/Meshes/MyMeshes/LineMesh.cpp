/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: LineMesh.cpp
Purpose: Class of Line Mesh, it makes scene code clean
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS300_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 9/29/2021
End Header --------------------------------------------------------*/
#include "LineMesh.h"
#include <../Common/Meshes/Mesh.h>
LineMesh::LineMesh()
	:MyMesh()
{
}

LineMesh::~LineMesh()
{
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}



bool LineMesh::Init(int vertexCount, GLfloat* vertices)
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * vertexCount, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);

	return true;
}

void LineMesh::PrepareDrawing()
{
	glUseProgram(programIDReference);
	glBindVertexArray(VAO);
}

void LineMesh::Draw(int drawCount)
{
	glDrawArrays(GL_LINES, 0, drawCount);
}
