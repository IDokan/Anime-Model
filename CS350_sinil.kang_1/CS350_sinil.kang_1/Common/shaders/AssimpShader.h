/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Shader.h
Purpose: Shader codes which compatible with Assimp(Model, AssimpMesh)
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS350_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 2/5/2022
End Header --------------------------------------------------------*/


#include <GL/glew.h>
#include <glm/common.hpp>

#pragma once

class AssimpShader
{
public:
	AssimpShader(GLuint shader);

	GLuint GetShaderHandle() const;
	void Use() const;

	void SendUniformFloatMatrix4(const char* uniformName, const float* uniformData);
	void SendUniformFloat3(const char* uniformName, const float* uniformData);
	void SendUniformFloat(const char* uniformName, const float uniformData);
	void SendUniformInt(const char* uniformName, const int uniformData);

private:
	GLuint shader;
};