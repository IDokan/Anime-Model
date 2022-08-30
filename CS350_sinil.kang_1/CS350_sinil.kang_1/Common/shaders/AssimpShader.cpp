/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Shader.cpp
Purpose: Shader codes which compatible with Assimp(Model, AssimpMesh)
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS350_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 2/5/2022
End Header --------------------------------------------------------*/

#include "AssimpShader.h"

AssimpShader::AssimpShader(GLuint shader)
	: shader(shader)
{
}

GLuint AssimpShader::GetShaderHandle() const
{
	return shader;
}

void AssimpShader::Use() const
{
	glUseProgram(shader);
}

void AssimpShader::SendUniformFloatMatrix4(const char* uniformName, const float* uniformData)
{
	GLint loc = glGetUniformLocation(shader, uniformName);
	glUniformMatrix4fv(loc, 1, GL_FALSE, uniformData);
}

void AssimpShader::SendUniformFloat3(const char* uniformName, const float* uniformData)
{
	GLint loc = glGetUniformLocation(shader, uniformName);
	glUniform3fv(loc, 1, uniformData);
}

void AssimpShader::SendUniformFloat(const char* uniformName, const float uniformData)
{
	GLint loc = glGetUniformLocation(shader, uniformName);
	glUniform1f(loc, uniformData);
}

void AssimpShader::SendUniformInt(const char* uniformName, const int uniformData)
{
	GLint loc = glGetUniformLocation(shader, uniformName);
	glUniform1i(loc, uniformData);
}
