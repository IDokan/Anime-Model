/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: AssimpMesh.h
Purpose: To import assimp mesh
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS350_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 2/5/2022
End Header --------------------------------------------------------*/

#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/common.hpp>

static constexpr unsigned int MAX_BONE_INFLUENCE = 4;

class AssimpShader;

struct AssimpVertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;

		//bone indexes which will influence this vertex
		int m_BoneIDs[MAX_BONE_INFLUENCE];
		//weights from each bone
		float m_Weights[MAX_BONE_INFLUENCE];
};

struct AssimpTexture 
{
	unsigned int id;
	std::string type;
	std::string path;
};

class AssimpMesh
{
public:
	std::vector<AssimpVertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<AssimpTexture> textures;
	unsigned int VAO;

	AssimpMesh(std::vector<AssimpVertex> vertices, std::vector<unsigned int> indices, std::vector<AssimpTexture> textures);
	void Draw(const AssimpShader& shader);

private:
	unsigned int VBO, EBO;
	void SetupMesh();
};