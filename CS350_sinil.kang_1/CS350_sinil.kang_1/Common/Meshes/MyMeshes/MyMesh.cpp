/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: MyMesh.cpp
Purpose: Class of MyMesh. It is abstract class because it is parent class of other mesh classes
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS300_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 9/29/2021
End Header --------------------------------------------------------*/

#include "MyMesh.h"
#include <../Common/Meshes/Mesh.h>
MyMesh::MyMesh()
	: VAO(0), VBO(0), programIDReference(0)
{
}

MyMesh::~MyMesh()
{
}

void MyMesh::SetShader(GLuint programID)
{
	programIDReference = programID;
}

GLuint MyMesh::GetShader()
{
	return programIDReference;
}

void MyMesh::SendUniformFloatMatrix4(const char* uniformName, const float* uniformData)
{
	GLint loc = glGetUniformLocation(programIDReference, uniformName);
	glUniformMatrix4fv(loc, 1, GL_FALSE, uniformData);
}

void MyMesh::SendUniformFloat3(const char* uniformName, const float* uniformData)
{
	GLint loc = glGetUniformLocation(programIDReference, uniformName);
	glUniform3fv(loc, 1, uniformData);
}

void MyMesh::SendUniformFloat(const char* uniformName, const float uniformData)
{
	GLint loc = glGetUniformLocation(programIDReference, uniformName);
	glUniform1f(loc, uniformData);
}

void MyMesh::SendUniformInt(const char* uniformName, const int uniformData)
{
	GLint loc = glGetUniformLocation(programIDReference, uniformName);
	glUniform1i(loc, uniformData);
}
