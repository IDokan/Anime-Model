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

#include <iostream>
#include "MyMesh.h"
#include <../Common/Meshes/Mesh.h>
#include <glm/matrix.hpp>
MyMesh::MyMesh()
	: VAO(0), VBO(0), programIDReference(0), uniformBlockBuffer(0), uniformBlockIndex(0)
{
}

MyMesh::~MyMesh()
{
	glDeleteBuffers(1, &uniformBlockBuffer);
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

bool MyMesh::SendUniformBlockFloatVQS(const char* blockName, const GLsizei blockPropertyCount, const char* const* blockPropertyNames, const glm::mat3* blockPropertyDataMatrix, const glm::vec3* blockPropertyDataVector, const float* blockPropertyDataScaler)
{
	glUseProgram(programIDReference);
	glBindVertexArray(VAO);

	uniformBlockIndex = glGetUniformBlockIndex(programIDReference, blockName);
	if (uniformBlockIndex == GL_INVALID_INDEX)
	{
		std::cout << blockName << " is not applied, uniformBlockIndex == GL_INVALID_INDEX" << std::endl;
		return false;
	}

	GLint uniformBlockSize = 0;
	glGetActiveUniformBlockiv(programIDReference, uniformBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &uniformBlockSize);

	GLuint* blockPropertyIndices = new GLuint[blockPropertyCount];
	glGetUniformIndices(programIDReference, blockPropertyCount, blockPropertyNames, blockPropertyIndices);

	if (blockPropertyIndices[0] == GL_INVALID_INDEX)
	{
		std::cout << blockName << " is not applied, blockPropertyIndices[0] == GL_INVALID_INDEX" << std::endl;
		return false;
	}

	GLint* offsets = new GLint[blockPropertyCount];
	glGetActiveUniformsiv(programIDReference, blockPropertyCount, blockPropertyIndices, GL_UNIFORM_OFFSET, offsets);

	char* buffer = new char[uniformBlockSize];
	for (int i = 0; i < blockPropertyCount; i+=3)
	{
		// if the destination write uniform data is valid,
		if (buffer + offsets[i] < &buffer[uniformBlockSize])
		{
			// Currently memcpy overwrites data. when src and dst size is a sizeof(float), it's okay, but it is broken when the size of over then sizeof(float4).
			// The main problem is how to decide sizes???
			const int dataIndex = static_cast<int>(i / 3);
			memcpy_s(buffer + offsets[i], sizeof(float) * 9, &blockPropertyDataMatrix[dataIndex], sizeof(float) * 9);
			memcpy_s(buffer + offsets[i + 1], sizeof(float) * 3, &blockPropertyDataVector[dataIndex], sizeof(float) * 3);
			memcpy_s(buffer + offsets[i + 2], sizeof(float), &blockPropertyDataScaler[dataIndex], sizeof(float));
		}
	}

	if (uniformBlockBuffer == 0)
	{
		glGenBuffers(1, &uniformBlockBuffer);
		glBindBuffer(GL_UNIFORM_BUFFER, uniformBlockBuffer);
	}

	glBufferData(GL_UNIFORM_BUFFER, uniformBlockSize, buffer, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, uniformBlockIndex, uniformBlockBuffer);

	delete[] buffer;

	return true;
}

bool MyMesh::SendUniformBlockMatrix4(const char* blockName, const GLsizei blockPropertyCount, const char* const* blockPropertyNames, const glm::mat4* blockPropertyData)
{
	glUseProgram(programIDReference);
	glBindVertexArray(VAO);

	uniformBlockIndex = glGetUniformBlockIndex(programIDReference, blockName);
	if (uniformBlockIndex == GL_INVALID_INDEX)
	{
		std::cout << blockName << " is not applied, uniformBlockIndex == GL_INVALID_INDEX" << std::endl;
		return false;
	}

	GLint uniformBlockSize = 0;
	glGetActiveUniformBlockiv(programIDReference, uniformBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &uniformBlockSize);

	GLuint* blockPropertyIndices = new GLuint[blockPropertyCount];
	glGetUniformIndices(programIDReference, blockPropertyCount, blockPropertyNames, blockPropertyIndices);

	if (blockPropertyIndices[0] == GL_INVALID_INDEX)
	{
		std::cout << blockName << " is not applied, blockPropertyIndices[0] == GL_INVALID_INDEX" << std::endl;
		return false;
	}

	GLint* offsets = new GLint[blockPropertyCount];
	glGetActiveUniformsiv(programIDReference, blockPropertyCount, blockPropertyIndices, GL_UNIFORM_OFFSET, offsets);
	GLenum result = glGetError();

	char* buffer = new char[uniformBlockSize];
	for (int i = 0; i < blockPropertyCount; i ++)
	{
		// if the destination write uniform data is valid,
		if (buffer + offsets[i] < &buffer[uniformBlockSize])
		{
			memcpy_s(buffer + offsets[i], sizeof(float) * 16, &blockPropertyData[i], sizeof(float) * 16);
		}
	}

	if (uniformBlockBuffer == 0)
	{
		glGenBuffers(1, &uniformBlockBuffer);
		glBindBuffer(GL_UNIFORM_BUFFER, uniformBlockBuffer);
	}

	glBufferData(GL_UNIFORM_BUFFER, uniformBlockSize, buffer, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, uniformBlockIndex, uniformBlockBuffer);

	delete[] buffer;

	return true;
}


bool MyMesh::SendUniformBlockVector3s(const GLchar* blockName, const GLsizei blockPropertyCount, const GLchar* const* blockPropertyNames, const float** blockPropertyData)
{	// Step 1.	Design layout of the uniform Blocks

	glUseProgram(programIDReference);
	glBindVertexArray(VAO);
	// Step 2.	First get the block index
	uniformBlockIndex = glGetUniformBlockIndex(programIDReference, blockName);
	if (uniformBlockIndex == GL_INVALID_INDEX)
	{
		std::cout << blockName << " is not applied, uniformBlockIndex == GL_INVALID_INDEX" << std::endl;
		return false;
	}

	// Step 3.	Allocate the block, get block indices and offsets
	GLint uniformBlockSize = 0;
	glGetActiveUniformBlockiv(programIDReference, uniformBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &uniformBlockSize);

	GLuint* blockPropertyIndices = new GLuint[blockPropertyCount];
	glGetUniformIndices(programIDReference, blockPropertyCount, blockPropertyNames, blockPropertyIndices);

	if (blockPropertyIndices[0] == GL_INVALID_INDEX)
	{
		std::cout << blockName << " is not applied, blockPropertyIndices[0] == GL_INVALID_INDEX" << std::endl;
		return false;
	}

	GLint* offsets = new GLint[blockPropertyCount];
	glGetActiveUniformsiv(programIDReference, blockPropertyCount, blockPropertyIndices, GL_UNIFORM_OFFSET, offsets);

	// Step 4.	Copy data into the buffer from CPU memory
	char* buffer = new char[uniformBlockSize];
	for (int i = 0; i < blockPropertyCount; ++i)
	{
		if (buffer + offsets[i] < &buffer[uniformBlockSize])
		{
			memcpy_s(buffer + offsets[i], sizeof(float) * 3, blockPropertyData[i], sizeof(float) * 3);
		}
	}

	// Step 5.	Create OpenGL buffer to manage this uniform block
	if (uniformBlockBuffer == 0)
	{
		glGenBuffers(1, &uniformBlockBuffer);
		glBindBuffer(GL_UNIFORM_BUFFER, uniformBlockBuffer);
	}
	glBufferData(GL_UNIFORM_BUFFER, uniformBlockSize, buffer, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, uniformBlockIndex, uniformBlockBuffer);

	delete[] buffer;

	return true;
}

bool MyMesh::SendUniformBlockVector3s(const GLchar* blockName, const GLsizei blockPropertyCount, const GLchar* const* blockPropertyNames, const glm::vec3* blockPropertyData)
{	// Step 1.	Design layout of the uniform Blocks

	glUseProgram(programIDReference);
	glBindVertexArray(VAO);
	// Step 2.	First get the block index
	uniformBlockIndex = glGetUniformBlockIndex(programIDReference, blockName);
	if (uniformBlockIndex == GL_INVALID_INDEX)
	{
		std::cout << blockName << " is not applied, uniformBlockIndex == GL_INVALID_INDEX" << std::endl;
		return false;
	}

	// Step 3.	Allocate the block, get block indices and offsets
	GLint uniformBlockSize = 0;
	glGetActiveUniformBlockiv(programIDReference, uniformBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &uniformBlockSize);

	GLuint* blockPropertyIndices = new GLuint[blockPropertyCount];
	glGetUniformIndices(programIDReference, blockPropertyCount, blockPropertyNames, blockPropertyIndices);

	if (blockPropertyIndices[0] == GL_INVALID_INDEX)
	{
		// A string in uniformNames is not the name of an active uniform
		std::cout << blockName << " is not applied, blockPropertyIndices[0] == GL_INVALID_INDEX" << std::endl;
		return false;
	}

	GLint* offsets = new GLint[blockPropertyCount];
	glGetActiveUniformsiv(programIDReference, blockPropertyCount, blockPropertyIndices, GL_UNIFORM_OFFSET, offsets);

	// Step 4.	Copy data into the buffer from CPU memory
	char* buffer = new char[uniformBlockSize];
	for (int i = 0; i < blockPropertyCount; ++i)
	{
		if (buffer + offsets[i] < &buffer[uniformBlockSize])
		{
			memcpy_s(buffer + offsets[i], sizeof(float) * 3, &blockPropertyData[i], sizeof(float) * 3);
		}
	}

	// Step 5.	Create OpenGL buffer to manage this uniform block
	if (uniformBlockBuffer == 0)
	{
		glGenBuffers(1, &uniformBlockBuffer);
		glBindBuffer(GL_UNIFORM_BUFFER, uniformBlockBuffer);
	}
	glBufferData(GL_UNIFORM_BUFFER, uniformBlockSize, buffer, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, uniformBlockIndex, uniformBlockBuffer);

	delete[] buffer;

	return true;
}