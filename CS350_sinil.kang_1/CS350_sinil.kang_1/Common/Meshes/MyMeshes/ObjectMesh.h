/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: ObjectMesh.h
Purpose: Class of ObjectMesh. All object in the scene is drawn by this class
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS300_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 9/29/2021
End Header --------------------------------------------------------*/
#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <../Common/Meshes/MyMeshes/MyMesh.h>

class ObjectMesh : public MyMesh
{
public:
	ObjectMesh();
	virtual ~ObjectMesh();

	bool Init(int vertexCount, GLfloat* vertices, GLfloat* normals, GLfloat* uvs,
		int indexCount, GLuint* indices);
	bool SendUniformBlockVector3s(const GLchar* blockName, const GLsizei blockPropertyCount, const GLchar* const* blockPropertyNames, const float** blockPropertyData);
	bool SendUniformBlockVector3s(const GLchar* blockName, const GLsizei blockPropertyCount, const GLchar* const* blockPropertyNames, const glm::vec3* blockPropertyData);
	void PrepareDrawing();
	void Draw(int indexCount);

private:
	GLuint normalBuffer;
	GLuint uvBuffer;
	GLuint indexBuffer;
	GLuint  uniformBlockBuffer;
	GLuint uniformBlockIndex;
};