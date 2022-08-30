/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: MyMesh.h
Purpose: Class of MyMesh. It is abstract class because it is parent class of other mesh classes
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS300_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 9/29/2021
End Header --------------------------------------------------------*/
#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Mesh;

class MyMesh
{
public:
	MyMesh();
	virtual ~MyMesh();

	virtual void PrepareDrawing() = 0;
	virtual void Draw(int drawCount) = 0;

	void SetShader(GLuint programID);
	GLuint GetShader();

	void SendUniformFloatMatrix4(const char* uniformName, const float* uniformData);
	void SendUniformFloat3(const char* uniformName, const float* uniformData);
	void SendUniformFloat(const char* uniformName, const float uniformData);
	void SendUniformInt(const char* uniformName, const int uniformData);

protected:
	GLuint VAO;
	GLuint VBO;

	GLuint programIDReference;
};