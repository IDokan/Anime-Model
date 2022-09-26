/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: TextureManager.h
Purpose: Class which manage textures
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS300_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 11/5/2021
End Header --------------------------------------------------------*/
#pragma once

#include <map>
#include <string>
#include <../Common/Textures/Texture.h>

class TextureManager
{
public:
	TextureManager();
	void AddTexture(const char* path, std::string textureName, enum class Texture::TextureType t);

	void AddTexture(int width, int height, std::string textureName);

	void ActivateTexture(GLuint programID, std::string activateName);

	void ActivateTexture(GLuint programID, std::string textureName, std::string ActivateName);

	GLuint GetTextureHandle(std::string textureName);
	
	void Clear();
private:
	int textureSize;
	std::map<std::string, Texture> textures;
};