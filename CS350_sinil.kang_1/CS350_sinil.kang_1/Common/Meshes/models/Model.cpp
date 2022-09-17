/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Model.cpp
Purpose: To import assimp
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS350_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 2/5/2022
End Header --------------------------------------------------------*/
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/common.hpp>
#include <glm/gtx/transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <../Common/Meshes/models/AssimpMesh.h>
#include <../Common/Meshes/models/Model.h>
#include <../Common/Meshes/Mesh.h>
#include <../Common/shaders/AssimpShader.h>

#include <../Common/Meshes/binFileSources/FileObject.h>


Model::Model(const std::string& path)
	:normalLength(1.f)
{
	LoadModel(path);
}

void Model::Draw(const AssimpShader& shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw(shader);
	}
}

glm::mat4 Model::CalcAdjustBoundingBoxMatrix()
{
	return glm::scale(2.f / GetModelScale()) * glm::translate(-GetModelCentroid());
}

glm::vec3 Model::GetModelScale()
{
	glm::vec3 scale = boundingBox[1] - boundingBox[0];

	if (scale.x == 0.0)
		scale.x = 1.0;

	if (scale.y == 0.0)
		scale.y = 1.0;

	if (scale.z == 0.0)
		scale.z = 1.0;

	return scale;
}

glm::vec3 Model::GetModelCentroid()
{
	return glm::vec3(boundingBox[0] + boundingBox[1]) * 0.5f;
}

glm::vec3 Model::GetCentroidVector(glm::vec3 vVertex)
{
	return glm::normalize(vVertex - GetModelCentroid());
}

GLfloat* Model::GetVertexNormalsForDisplay()
{
	return reinterpret_cast<GLfloat*>(vertexNormalDisplay.data());
}

GLfloat* Model::GetFaceNormalsForDisplay()
{
	return reinterpret_cast<GLfloat*>(faceNormalDisplay.data());
}

unsigned int Model::GetVertexNormalCount()
{
	return static_cast<unsigned int>(vertexNormalDisplay.size());
}

unsigned int Model::GetFaceNormalCount()
{
	return static_cast<unsigned int>(faceNormalDisplay.size());
}

glm::vec2 Model::CalcUVs(glm::vec3 position)
{
	glm::vec3 centroidVec = GetCentroidVector(position);
	return CalcCubeMap(centroidVec);

	float theta = 0.f;
	float z = 0.f;
	float phi = 0.f;
	glm::vec2 uv;

	theta = glm::degrees(static_cast<float>(atan2(centroidVec.y, centroidVec.x)));
	theta += 180.0f;

	z = (centroidVec.z + 1.0f) * 0.5f;

	uv.x = theta / 360.0f;
	uv.y = z;

	return uv;
}

glm::vec2 Model::CalcCubeMap(glm::vec3 vEntity)
{
	float x = vEntity.x;
	float y = vEntity.y;
	float z = vEntity.z;

	float absX = abs(x);
	float absY = abs(y);
	float absZ = abs(z);

	int isXPositive = x > 0 ? 1 : 0;
	int isYPositive = y > 0 ? 1 : 0;
	int isZPositive = z > 0 ? 1 : 0;

	float maxAxis, uc, vc;
	glm::vec2 uv = glm::vec2(0.0);
	uc = 0.f;
	vc = 0.f;

	// POSITIVE X
	if (bool(isXPositive) && (absX >= absY) && (absX >= absZ))
	{
		// u (0 to 1) goes from +z to -z
		// v (0 to 1) goes from -y to +y
		maxAxis = absX;
		uc = -z;
		vc = y;
	}

	// NEGATIVE X
	else if (!bool(isXPositive) && absX >= absY && absX >= absZ)
	{
		// u (0 to 1) goes from -z to +z
		// v (0 to 1) goes from -y to +y
		maxAxis = absX;
		uc = z;
		vc = y;
	}

	// POSITIVE Y
	else if (bool(isYPositive) && absY >= absX && absY >= absZ)
	{
		// u (0 to 1) goes from -x to +x
		// v (0 to 1) goes from +z to -z
		maxAxis = absY;
		uc = x;
		vc = -z;
	}

	// NEGATIVE Y
	else if (!bool(isYPositive) && absY >= absX && absY >= absZ)
	{
		// u (0 to 1) goes from -x to +x
		// v (0 to 1) goes from -z to +z
		maxAxis = absY;
		uc = x;
		vc = z;
	}

	// POSITIVE Z
	else if (bool(isZPositive) && absZ >= absX && absZ >= absY)
	{
		// u (0 to 1) goes from -x to +x
		// v (0 to 1) goes from -y to +y
		maxAxis = absZ;
		uc = x;
		vc = y;
	}

	// NEGATIVE Z
	else if (!bool(isZPositive) && absZ >= absX && absZ >= absY)
	{
		// u (0 to 1) goes from +x to -x
		// v (0 to 1) goes from -y to +y
		maxAxis = absZ;
		uc = -x;
		vc = y;
	}

	// Convert range from -1 to 1 to 0 to 1
	uv.s = 0.5f * (uc / maxAxis + 1.0f);
	uv.t = 0.5f * (vc / maxAxis + 1.0f);

	return uv;
}

void Model::Clear()
{
	boundingBox[0] = glm::vec3(INFINITY);
	boundingBox[1] = glm::vec3(-INFINITY);

	vertexNormalDisplay.clear();
	faceNormalDisplay.clear();

	textures_loaded.clear();
	meshes.clear();
}

void Model::LoadModel(const std::string& path)
{
	Clear();

	// Default is assimp importer
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices | aiProcess_GenUVCoords);
	
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));

	ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	// Process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(mesh, scene));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

AssimpMesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<AssimpVertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<AssimpTexture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		AssimpVertex vertex;
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		boundingBox[0].x = std::min(boundingBox[0].x, vector.x);
		boundingBox[1].x = std::max(boundingBox[1].x, vector.x);
		vector.y = mesh->mVertices[i].y;
		boundingBox[0].y = std::min(boundingBox[0].y, vector.y);
		boundingBox[1].y = std::max(boundingBox[1].y, vector.y);
		vector.z = mesh->mVertices[i].z;
		boundingBox[0].z = std::min(boundingBox[0].z, vector.z);
		boundingBox[1].z = std::max(boundingBox[1].z, vector.z);
		vertex.Position = vector;
		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertexNormalDisplay.push_back(vertex.Position);
			vertexNormalDisplay.push_back(vector);
			vertex.Normal = vector;
		}
		if (mesh->mTextureCoords[0])		// does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;

			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;

			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
		}
		else
		{
			vertex.TexCoords = glm::vec2(-99.f);
		}

		vertices.push_back(vertex);
	}

	glm::vec3 modelScale = GetModelScale();
	normalLength = glm::length(modelScale) / 100.f;
	const int normalSize = static_cast<const int>(vertexNormalDisplay.size());
	for (int i = 0; i < normalSize; i += 2)
	{
		vertexNormalDisplay[i + 1] = vertexNormalDisplay[i] + (vertexNormalDisplay[i + 1] * normalLength);
	}
	const int verticesSize = static_cast<const int>(vertices.size());
	for (int i = 0; i < verticesSize; i++)
	{
		glm::vec2 tex = vertices[i].TexCoords;
		if (tex.x < -50.f && tex.y < -50.f)
		{
			vertices[i].TexCoords = CalcUVs(vertices[i].Position);
		}
	}

	// Process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}

		// Calculate face normal
		unsigned int index0 = face.mIndices[0];
		unsigned int index1 = face.mIndices[1];
		unsigned int index2 = face.mIndices[2];

		glm::vec3 position0 = vertices[index0].Position;
		glm::vec3 position1 = vertices[index1].Position;
		glm::vec3 position2 = vertices[index2].Position;

		glm::vec3 E1 = position1 - position0;
		glm::vec3 E2 = position2 - position0;

		glm::vec3 normal = glm::normalize(glm::cross(E1, E2));

		if (isnan(normal.x) || isnan(normal.y) || isnan(normal.z))
		{
			continue;
		}
		glm::vec3 position = (position0 + position1 + position2) / 3.f;
		faceNormalDisplay.push_back(position);
		faceNormalDisplay.push_back(position + (normal * normalLength));
	}
	// Process Material
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	std::vector<AssimpTexture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

	std::vector<AssimpTexture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

	std::vector<AssimpTexture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

	std::vector<AssimpTexture> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	return AssimpMesh(vertices, indices, textures);
}

std::vector<AssimpTexture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<AssimpTexture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		// check if texture was loaded before and if so, continue to next iteration: 
			// skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;	// a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{		// if texture hasn't been loaded already, load it
			AssimpTexture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			// store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
			textures_loaded.push_back(texture);
		}
	}
	return textures;
}

unsigned int Model::TextureFromFile(const char* path, const std::string& directory, bool gamma)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
		{
			format = GL_RED;
		}
		else if (nrComponents == 3)
		{
			format = GL_RGB;
		}
		else if (nrComponents == 4)
		{
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}