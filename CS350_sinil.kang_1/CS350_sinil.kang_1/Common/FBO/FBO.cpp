/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: FBO.cpp
Purpose: Class which manage Frame buffer object
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS300_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 11/28/2021
End Header --------------------------------------------------------*/

#include <iostream>
#include <../Common/FBO/FBO.h>
#include <../Common/Textures/TextureManager.h>


FBO::FBO()
	: frameBufferHandle(0), depthRenderBuffer(0), clearColor(glm::vec3(0.0f)), textureManager(nullptr), customBufferMode(false)
{
}

void FBO::Initialize(TextureManager* tManager, std::string firstTextureName, glm::vec3 color)
{
	clearColor = color;

	textureManager = tManager;

	if (textureManager == nullptr)
	{
		std::cout << "Please pass valid textureManager" << std::endl; 
	}

	// Get handle to the FB Object
	glGenFramebuffers(1, &frameBufferHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferHandle);

	// Create a texture
	textureManager->AddTexture(windowWidth, windowHeight, firstTextureName);

	// Add Depth buffer
	glGenRenderbuffers(1, &depthRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
	
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);

	// Now add the "render target"
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 
		textureManager->GetTextureHandle(firstTextureName), 0);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer is incomplete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::InitializeCustomBuffer(TextureManager* tManager, std::vector<std::string> textureNames)
{
	customBufferMode = true;

	textureManager = tManager;

	if (textureManager == nullptr)
	{
		std::cout << "Please pass valid textureManager" << std::endl;
	}

	// Get handle to the FB Object
	glGenFramebuffers(1, &frameBufferHandle);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferHandle);

	const size_t namesSize = textureNames.size();
	if (namesSize < 0)
	{
		std::cout << "Please pass valid textureNames" << std::endl;
		return;
	}
	for (int i = 0; i < namesSize; i++)
	{
		const std::string& textureName = textureNames.at(i);
		// Create a texture
		textureManager->AddTexture(windowWidth, windowHeight, textureName);

		// Now add the "render target"
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D,
			textureManager->GetTextureHandle(textureName), 0);
	}

	// Add Depth buffer
	glGenRenderbuffers(1, &depthRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);

	GLenum* drawBuffers = new GLenum[namesSize];
	for (int i = 0; i < namesSize; i++)
	{
		drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
	}
	glDrawBuffers(static_cast<GLsizei>(namesSize), drawBuffers);
	delete[] drawBuffers;
	

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer is incomplete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::AddTexture(std::string newTextureName)
{
	if (customBufferMode == true)
	{
		std::cout << "FBO::AddTexture : CustomBufferMode is on! Called incompatible function" << std::endl;
		return;
	}

	textureManager->AddTexture(windowWidth, windowHeight, newTextureName);
}

void FBO::ApplyFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferHandle);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer is incomplete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return;
	}

	glViewport(0, 0, windowWidth, windowHeight);
	glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
}

void FBO::BindTexture(std::string textureName)
{
	if (customBufferMode == true)
	{
		std::cout << "FBO::BindTexture : CustomBufferMode is on! Called incompatible function" << std::endl;
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferHandle);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
		textureManager->GetTextureHandle(textureName), 0);

	glViewport(0, 0, windowWidth, windowHeight);
	glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer is incomplete!" << std::endl;
	}
}

void FBO::RestoreDefaultFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDrawBuffer(GL_BACK_LEFT);
}

void FBO::Clear()
{
	glDeleteRenderbuffers(1, &depthRenderBuffer);
	glDeleteFramebuffers(1, &frameBufferHandle);
}

void FBO::UseTexture(GLuint programID, const GLchar* name)
{
	textureManager->ActivateTexture(programID, name);
}

void FBO::SetClearColor(glm::vec3 color)
{
	clearColor = color;
}

void FBO::CopyDepthInfo()
{
	// bind the gbuffers for read
	glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferHandle);

	// write to default framebuffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	// copy over depth info
	glBlitFramebuffer(
		0, 0, windowWidth, windowHeight,		// source region
		0, 0, windowWidth, windowHeight,		// destination region
		GL_DEPTH_BUFFER_BIT,					// field to copy
		GL_NEAREST									// filtering mechanism
	);

	// set the default framebuffer for draw
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
