/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Texture.cpp
Purpose: Class which manage texture
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS300_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 11/5/2021
End Header --------------------------------------------------------*/

#include <../Common/Textures/Texture.h>
#include <../Common/ppms/ppmReader.h>

Texture::Texture()
	: width(0), height(0), pixels(nullptr), textureNum(-1), textureHandle(0)
{
}

Texture::~Texture()
{
}

void Texture::SetupTexture(const char* path, int _textureNum, enum class TextureType tType)
{
	textureNum = _textureNum;

	switch (tType)
	{
	case Texture::TextureType::BMP:
		pixels = readBMP(path, width, height);
		break;
	case Texture::TextureType::PPM:
		pixels = readImage(path, width, height);
		break;
	default:
		break;
	}

	glGenTextures(1, &textureHandle);
	
	glActiveTexture(GL_TEXTURE0 + textureNum);
	glBindTexture(GL_TEXTURE_2D, textureHandle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, pixels);

	delete[] pixels;
}

void Texture::SetupTexture(int width, int height, int _textureNum)
{
	textureNum = _textureNum;

	glGenTextures(1, &textureHandle);

	glActiveTexture(GL_TEXTURE0 + textureNum);
	glBindTexture(GL_TEXTURE_2D, textureHandle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, 0);
}

void Texture::UpdateTexture(GLuint programID, const GLchar* name)
{
	if (textureNum < 0 )
	{
		// ERROR!!
		return;
	}

	glUseProgram(programID);
	
	// Textures
	glActiveTexture(GL_TEXTURE0 + textureNum);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
	
	// Uniform sampler for texture unit 0
	GLint texSamplerLoc = glGetUniformLocation(programID, name);
	glUniform1i(texSamplerLoc, textureNum);
}

GLuint Texture::GetHandle() const
{
	return textureHandle;
}

void Texture::Clear()
{
	glDeleteTextures(1, &textureHandle);
}
