/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Model.h
Purpose: To import assimp
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS350_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 2/5/2022
End Header --------------------------------------------------------*/
#pragma once

#include <string>
#include <vector>

class AssimpMesh;

struct AssimpTexture;

struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;
enum aiTextureType;

class AssimpShader;

class Model
{
public:
	Model(const std::string& path);
	void LoadModel(const std::string& path);

	void Draw(const AssimpShader& shader);


	glm::mat4 CalcAdjustBoundingBoxMatrix();
	glm::vec3   GetModelScale();
	glm::vec3   GetModelCentroid();
	glm::vec3 GetCentroidVector(glm::vec3 vVertex);

	GLfloat* GetVertexNormalsForDisplay();
	GLfloat* GetFaceNormalsForDisplay();
	unsigned int GetVertexNormalCount();
	unsigned int GetFaceNormalCount();

	glm::vec2 CalcUVs(glm::vec3 position);
	glm::vec2   CalcCubeMap(glm::vec3 vEntity);

	void Clear();

private:
	std::vector<AssimpTexture> textures_loaded;
	std::vector<AssimpMesh> meshes;
	std::string directory;
	bool gammaCorrection;

	void ProcessNode(aiNode* node, const aiScene* scene);
	AssimpMesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<AssimpTexture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

	glm::vec3 boundingBox[2];

	std::vector<glm::vec3> vertexNormalDisplay, faceNormalDisplay;
	float normalLength;
};