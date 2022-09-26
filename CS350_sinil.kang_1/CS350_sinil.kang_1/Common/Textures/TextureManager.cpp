/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: TextureManager.cpp
Purpose: Class which manage textures
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS300_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 11/5/2021
End Header --------------------------------------------------------*/
#include <../Common/Textures/TextureManager.h>

TextureManager::TextureManager()
	: textureSize(0)
{
	textures.clear();
}

void TextureManager::AddTexture(const char* path, std::string textureName, enum class Texture::TextureType t)
{
	std::pair<std::string, Texture> newTex;
	newTex.first = textureName;
	newTex.second.SetupTexture(path, textureSize++, t);
	textures.insert(newTex);
}

void TextureManager::AddTexture(int width, int height, std::string textureName)
{
	std::pair<std::string, Texture> newTex;
	newTex.first = textureName;
	newTex.second.SetupTexture(width, height, textureSize++);
	textures.insert(newTex);
}

void TextureManager::ActivateTexture(GLuint programID, std::string activateName)
{
	ActivateTexture(programID, activateName, activateName);
}

void TextureManager::ActivateTexture(GLuint programID, std::string textureName, std::string activateName)
{
	textures.find(textureName)->second.UpdateTexture(programID, activateName.c_str());
}

GLuint TextureManager::GetTextureHandle(std::string textureName)
{
	return textures.find(textureName)->second.GetHandle();
}

void TextureManager::Clear()
{
	for (auto t = textures.begin(); t != textures.end(); t++)
	{
		t->second.Clear();
	}
}
