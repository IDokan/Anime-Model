//
// Mainly Created and contributed by pushpak on 10/5/18.
//

/* Start Header -------------------------------------------------------
Sinil made a small changes of this code
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: ObjectMesh.h
Purpose: Class of ObjectMesh. All object in the scene is drawn by this class
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS300_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 9/20/2021
End Header --------------------------------------------------------*/
#include <GL/glew.h>
#include <iostream>
#include <set>
#include "Mesh.h"

#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/transform.hpp>

#include <../Common/Meshes/binFileSources/FileObject.h>

// Initialize the data members in the mesh
void Mesh::initData()
{
	vertexBuffer.clear();
	vertexIndices.clear();
	vertexUVs.clear();
	vertexNormals.clear();
	vertexNormalDisplay.clear();
	faceNormalDisplay.clear();

	normalLength = 0.00f;

	boneIndices.clear();
	boneWeights.clear();

	boundingBox[0].x = FLT_MAX;
	boundingBox[0].y = FLT_MAX;
	boundingBox[0].z = FLT_MAX;
	boundingBox[1].x = -FLT_MAX;
	boundingBox[1].y = -FLT_MAX;
	boundingBox[1].z = -FLT_MAX;

	return;
}

Mesh::Mesh(bool binParserMode)
	: binParserMode(binParserMode), animTimer(0.f), animCyclePerSecond(1.f), endEffectorIndex(0), interpolatedPositionCount(0)
{
}

/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////
GLfloat* Mesh::getVertexBuffer()
{
	return reinterpret_cast<GLfloat*>(vertexBuffer.data());
}

GLfloat* Mesh::getVertexNormals()
{
	return reinterpret_cast<GLfloat*>(vertexNormals.data());
}

GLfloat* Mesh::getVertexUVs()
{
	return reinterpret_cast<GLfloat*>(vertexUVs.data());
}

GLfloat* Mesh::getVertexNormalsForDisplay()
{
	return reinterpret_cast<GLfloat*>(vertexNormalDisplay.data());
}

GLfloat* Mesh::getFaceNormalsForDisplay()
{
	return reinterpret_cast<GLfloat*>(faceNormalDisplay.data());
}

GLfloat* Mesh::GetBonesForDisplay()
{
	return reinterpret_cast<GLfloat*>(initialBones.data());
}

GLubyte* Mesh::GetBoneIDs()
{
	return reinterpret_cast<GLubyte*>(boneIndices.data());
}

GLfloat* Mesh::GetBoneWeights()
{
	return reinterpret_cast<GLfloat*>(boneWeights.data());
}

std::vector<float>& Mesh::getMass()
{
	return mass;
}

GLuint* Mesh::getIndexBuffer()
{
	return vertexIndices.data();
}

////////////////////////////////////
////////////////////////////////////
////////////////////////////////////
unsigned int Mesh::getVertexBufferSize()
{
	return (unsigned int)vertexBuffer.size();
}

unsigned int Mesh::getIndexBufferSize()
{
	return (unsigned int)vertexIndices.size();
}

unsigned int Mesh::getTriangleCount()
{
	return getIndexBufferSize() / 3;
}

unsigned int Mesh::getVertexCount()
{
	return getVertexBufferSize();
}

unsigned int Mesh::getVertexNormalCount()
{
	return static_cast<unsigned int>(vertexNormalDisplay.size());
}

unsigned int Mesh::getFaceNormalCount()
{
	return static_cast<unsigned int>(faceNormalDisplay.size());
}

unsigned int Mesh::GetBoneCountForDisplay()
{
	return static_cast<unsigned int>(initialBones.size());
}

const std::vector<Bone>& Mesh::GetSkeleton()
{
	return skeleton;
}

void Mesh::SetVertex(const size_t i, const glm::vec3 newVertex)
{
	vertexBuffer[i] = newVertex;
}

glm::vec3  Mesh::getModelScale()
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

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

glm::vec3  Mesh::getModelCentroid()
{
	return glm::vec3(boundingBox[0] + boundingBox[1]) * 0.5f;
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

glm::vec3 Mesh::getCentroidVector(glm::vec3 vVertex)
{
	return glm::normalize(vVertex - getModelCentroid());
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

struct compareVec
{
	bool operator() (const glm::vec3& lhs, const glm::vec3& rhs) const
	{
		float V = glm::dot(lhs, rhs);
		bool bRetCode = glm::epsilonNotEqual(V, 1.0f, FLT_EPSILON);

		return bRetCode;
	}
};

/////////////////////////////////////////////////////////
int Mesh::calcVertexNormals(GLboolean bFlipNormals)
{
	int rFlag = -1;

	// vertices and indices must be populated
	if (vertexBuffer.empty() || vertexIndices.empty())
	{
		std::cout << "Cannot calculate vertex normals for empty mesh." << std::endl;
		return rFlag;
	}

	// Pre-built vertex normals
//    if( !vertexNormals.empty() )
//    {
//        std::cout << "Vertex normals specified by the file. Skipping this step." << std::endl;
//        return rFlag;
//    }

	// Initialize vertex normals
	GLuint  numVertices = getVertexCount();
	vertexNormals.resize(numVertices, glm::vec3(0.0f));
	vertexNormalDisplay.resize(static_cast<size_t>(numVertices) * 2U, glm::vec3(0.0f));

	std::vector< std::vector< glm::vec3> >  vNormalSet;
	vNormalSet.resize(numVertices);

	// For every face
	GLuint numIndex = static_cast<GLuint>(vertexIndices.size());
	GLuint numFaces = numIndex / 3;
	std::vector<glm::vec3> faceNormals;
	faceNormals.resize(numFaces, glm::vec3(0.0f));
	for (glm::uint index = 0; index < numIndex; )
	{
		GLuint a = vertexIndices.at(index++);
		GLuint b = vertexIndices.at(index++);
		GLuint c = vertexIndices.at(index++);

		glm::vec3  vA = vertexBuffer[a];
		glm::vec3  vB = vertexBuffer[b];
		glm::vec3  vC = vertexBuffer[c];

		// Edge vectors
		glm::vec3  E1 = vB - vA;
		glm::vec3  E2 = vC - vA;

		glm::vec3  N = glm::normalize(glm::cross(E1, E2));

		if (isnan(N.x) || isnan(N.y) || isnan(N.z))
		{
			continue;
		}

		if (bFlipNormals)
			N = N * -1.0f;

		if ((index / 3) - 1 >= faceNormals.size())
		{
			std::cout << "(index / 3) - 1 >= faceNormals.size()" << std::endl;
		}
		faceNormals[(index / 3) - 1] = (N);

		// For vertex a
		vNormalSet.at(a).push_back(N);
		vNormalSet.at(b).push_back(N);
		vNormalSet.at(c).push_back(N);
	}

	// Now sum up the values per vertex
	for (int index = 0; index < vNormalSet.size(); ++index)
	{
		glm::vec3  vNormal(0.0f);

		//        if( vNormalSet[index].size() <= 0 )
		//            std::cout << "[Normal Set](" << index << ") Size = " << vNormalSet[index].size() << std::endl;

		auto nIt = vNormalSet[index].begin();
		while (nIt != vNormalSet[index].end())
		{
			vNormal += (*nIt);
			++nIt;
		}

		// save vertex normal
		vertexNormals[index] = glm::normalize(vNormal);

		// save normal to display
		glm::vec3  vA = vertexBuffer[index];

		if ((2U * static_cast<size_t>(index)) >= vertexNormalDisplay.size())
		{
			std::cout << "2 * index >= vertexNormalDisplay.size()" << std::endl;
		}
		if ((2U * static_cast<size_t>(index)) + 1 >= vertexNormalDisplay.size())
		{
			std::cout << "2 * index+1 >= vertexNormalDisplay.size()" << std::endl;
		}

		vertexNormalDisplay[2U * static_cast<size_t>(index)] = vA;
		vertexNormalDisplay[(2U * static_cast<size_t>(index)) + 1] = vA + (normalLength * vertexNormals[index]);

	}


	faceNormalDisplay.resize(static_cast<size_t>(numFaces) * 2U, glm::vec3(0.0f));
	for (unsigned int i = 0; i < numFaces; i++)
	{
		glm::vec3 v1 = vertexBuffer[vertexIndices[(static_cast<size_t>(i) * 3U)]];
		glm::vec3 v2 = vertexBuffer[vertexIndices[(static_cast<size_t>(i) * 3U) + 1]];
		glm::vec3 v3 = vertexBuffer[vertexIndices[(static_cast<size_t>(i) * 3U) + 2]];

		glm::vec3 position = (v1 + v2 + v3) / 3.f;

		glm::vec3 fN = faceNormals[i];
		faceNormalDisplay[2U * static_cast<size_t>(i)] = position;
		faceNormalDisplay[(2U * static_cast<size_t>(i)) + 1] = position + (normalLength * fN);
	}

	// success
	rFlag = 0;

	return rFlag;
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

void Mesh::calcVertexNormalsForDisplay(GLboolean /*bFlipNormals*/)
{
	GLuint numVertices = getVertexCount();
	vertexNormalDisplay.resize(static_cast<size_t>(numVertices) * 2U, glm::vec3(0.0f));

	for (int iNormal = 0; iNormal < vertexNormals.size(); ++iNormal)
	{
		glm::vec3 normal = vertexNormals[iNormal] * normalLength;

		vertexNormalDisplay[2U * static_cast<size_t>(iNormal)] = vertexBuffer[iNormal];
		vertexNormalDisplay[(2U * static_cast<size_t>(iNormal)) + 1] = vertexBuffer[iNormal] + normal;
	}
}
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

GLfloat& Mesh::getNormalLength()
{
	return normalLength;
}

void Mesh::setNormalLength(GLfloat nLength)
{
	normalLength = nLength;
}
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

int Mesh::calcUVs(Mesh::UVType uvType)
{
	int rFlag = -1;

	// clear any existing UV
	vertexUVs.clear();

	glm::vec3 delta = getModelScale();

	for (int nVertex = 0; nVertex < vertexBuffer.size(); ++nVertex)
	{
		glm::vec3 V = vertexBuffer[nVertex];
		glm::vec2 uv(0.0f);

		glm::vec3 normVertex = glm::vec3((V.x - boundingBox[0].x) / delta.x,
			(V.y - boundingBox[0].y) / delta.y,
			(V.z - boundingBox[0].z) / delta.z);

		//        normVertex = (2.0f * normVertex) - glm::vec3(1.0f);

		glm::vec3 centroidVec = getCentroidVector(V);

		float theta(0.0f);
		float z(0.0f);
		float phi(0.0f);

		switch (uvType)
		{
		case Mesh::UVType::CYLINDRICAL_UV:
			theta = glm::degrees(static_cast<float>(atan2(centroidVec.y, centroidVec.x)));
			theta += 180.0f;

			z = (centroidVec.z + 1.0f) * 0.5f;

			uv.x = theta / 360.0f;
			uv.y = z;
			break;

		case Mesh::UVType::SPHERICAL_UV:
			theta = glm::degrees(static_cast<float>(glm::atan(centroidVec.y, centroidVec.x)));
			theta += 180.0f;

			z = centroidVec.z;
			phi = glm::degrees(glm::acos(z / centroidVec.length()));

			uv.x = theta / 360.0f;
			uv.y = 1.0f - (phi / 180.0f);
			break;

		case Mesh::UVType::CUBE_MAPPED_UV:
			uv = calcCubeMap(centroidVec);
			break;
		}

		vertexUVs.push_back(uv);
	}

	return rFlag;
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

glm::vec2 Mesh::calcCubeMap(glm::vec3 vEntity)
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

glm::mat4 Mesh::calcAdjustBoundingBoxMatrix()
{
	return glm::scale(2.f / getModelScale()) * glm::translate(-getModelCentroid());
}

void Mesh::GetToBoneFromModel(std::vector<Vqs>& toBoneFromModel)
{
	int skeletonSize = static_cast<int>(skeleton.size());
	toBoneFromModel.resize(skeletonSize);
	for (int i = 0; i < skeletonSize; i++)
	{
		toBoneFromModel[i] = skeleton[i].toBoneFromModel;
	}
}

void Mesh::GetAnimationTransform(std::vector<Vqs>& transforms, bool getHierachicalResult)
{
	Animation animation = animations[0];

	const size_t trackSize = animation.tracks.size();
	transforms.resize(trackSize);
	for (size_t i = 0; i < trackSize; i++)
	{
		const size_t keyFrameSize = animation.tracks[i].keyFrames.size();
		for (size_t j = 0; j < keyFrameSize; j++)
		{
			int transformIndex;
			int nextTransformIndex;
			// If time is correct or that is the last data
			if (animTimer < animation.tracks[i].keyFrames[j].time || j == keyFrameSize - 1)
			{
				// If there is only one keyFrame
				if (keyFrameSize == 1)
				{
					Vqs result = animation.tracks[i].keyFrames[j].toModelFromBone;
					result.q = result.q / (magnitude(result.q));
					// Has parent
					if (skeleton[i].parentID >= 0 && getHierachicalResult)
					{
						transforms[i] = transforms[skeleton[i].parentID] * result;
					}
					else
					{
						transforms[i] = result;
					}
					break;
				}

				// Guarantee that data is in the area
				transformIndex = static_cast<int>((j == 0) ? 0 : (j - 1));
				nextTransformIndex = static_cast<int>(j);


				float t = (animTimer - animation.tracks[i].keyFrames[transformIndex].time) / (animation.tracks[i].keyFrames[nextTransformIndex].time - animation.tracks[i].keyFrames[transformIndex].time);
				Vqs result;
				result.v = (1 - t) * animation.tracks[i].keyFrames[transformIndex].toModelFromBone.v + (t * animation.tracks[i].keyFrames[nextTransformIndex].toModelFromBone.v);

				slerp(animation.tracks[i].keyFrames[transformIndex].toModelFromBone.q, animation.tracks[i].keyFrames[nextTransformIndex].toModelFromBone.q, t, result.q);
				result.q = (result.q) / magnitude(result.q);

				// Has parent
				if (skeleton[i].parentID >= 0 && getHierachicalResult)
				{
					transforms[i] = transforms[skeleton[i].parentID] * result;
				}
				else
				{
					transforms[i] = result;
				}

				break;
			}
		}
	}
}

// Return a transformation to the End Effector.
Vqs Mesh::GetAnimationTransform(unsigned int index)
{
	Animation animation = animations[0];
	Vqs finalResult{};

	while (index != -1)
	{
		const size_t keyFrameSize = animation.tracks[index].keyFrames.size();
		for (size_t j = 0; j < keyFrameSize; j++)
		{
			int transformIndex;
			int nextTransformIndex;
			// If time is correct or that is the last data
			if (animTimer < animation.tracks[index].keyFrames[j].time || j == keyFrameSize - 1)
			{
				// If there is only one keyFrame
				if (keyFrameSize == 1)
				{
					Vqs result = animation.tracks[index].keyFrames[j].toModelFromBone;
					result.q = result.q / (magnitude(result.q));

					finalResult = result * finalResult;

					// Update current index to do get transformations of parent.
					index = skeleton[index].parentID;
					break;
				}

				// Guarantee that data is in the area
				transformIndex = static_cast<int>((j == 0) ? 0 : (j - 1));
				nextTransformIndex = static_cast<int>(j);


				float t = (animTimer - animation.tracks[index].keyFrames[transformIndex].time) / (animation.tracks[index].keyFrames[nextTransformIndex].time - animation.tracks[index].keyFrames[transformIndex].time);
				Vqs result;
				result.v = (1 - t) * animation.tracks[index].keyFrames[transformIndex].toModelFromBone.v + (t * animation.tracks[index].keyFrames[nextTransformIndex].toModelFromBone.v);

				slerp(animation.tracks[index].keyFrames[transformIndex].toModelFromBone.q, animation.tracks[index].keyFrames[nextTransformIndex].toModelFromBone.q, t, result.q);
				result.q = (result.q) / magnitude(result.q);

				finalResult = result * finalResult;
				// Update current index to do get transformations of parent.
				index = skeleton[index].parentID;

				break;
			}
		}
	}

	return finalResult;
}

float Mesh::GetAnimationDuration()
{
	return animations[0].duration;
}

void Mesh::SetAnimationTimer(float time)
{
	animTimer = time;
}

void Mesh::UpdateAnimationTimer(float dt, float velocity)
{
	Animation animation = animations[0];

	constexpr float P = 2.f;
	// Disable velocity features
		// -> It indicates path walking features has been disabled.
	animTimer += dt * velocity / P;
	if (animTimer > animation.duration)
	{
		animTimer -= animation.duration * static_cast<int>(animTimer / animation.duration);
	}
}

bool Mesh::LoadBinFile(const std::string& path)
{
	initData();

	// Bin parser mode activated
	if (binParserMode)
	{
		FileObject* pFile = FileObject::OpenBinaryRead(path.c_str());
		if (!pFile)
		{
			std::cout << "Invalid File Name: " << path << std::endl;
			return false;
		}

		if (ParseBinFile(pFile) == false)
		{
			std::cout << "ERROR::BINPARSER::PARSING_FAILED";
			return false;
		}

		return true;
	}

	return false;
}

bool Mesh::ParseBinFile(FileObject* pFile)
{
	if (binParserMode == false)
	{
		std::cout << "Activate bin parser mode";
		return false;
	}

	unsigned int fileVersion;
	pFile->Read(fileVersion);
	if (fileVersion != 'dpm2')
	{
		std::cout << "File Version Mismatch\n";
		delete pFile;
		return false;
	}

	unsigned int sectionType;
	unsigned int sectionSize;
	pFile->Read(sectionSize);

	while (true)
	{
		pFile->Read(sectionType);
		pFile->Read(sectionSize);

		if (pFile->IsAtEndOfFile())
		{
			break;
		}

		switch (sectionType)
		{
		case 'mesh':
			if (!ReadMesh(pFile))
			{
				delete pFile;
				return false;
			}
			break;
		case 'skel':
			ReadSkeleton(pFile);
			break;
		case 'anim':
			ReadAnimation(pFile);
			break;
		default:
			std::cout << "Unknown Section Type" << std::endl;
			delete pFile;
			return false;
			break;
		}
	}

	return true;
}

bool Mesh::ReadMesh(FileObject* pFile)
{
	if (binParserMode == false)
	{
		std::cout << "Activate bin parser mode" << std::endl;
		return false;
	}

	unsigned int vertexType;
	pFile->Read(vertexType);
	switch (vertexType)
	{
	case 0:				// VertexTypeDefault
		std::cout << "Model has no skeleton";
		return false;
	case 1:				// VertexTypeSkin
		break;
	default:
		std::cout << "Invalid Vertex Type" << std::endl;
		return false;
	}

	unsigned int indexCount;
	pFile->Read(indexCount);
	vertexIndices.resize(indexCount);
	pFile->Read(vertexIndices[0], indexCount);

	unsigned int vertCount;
	pFile->Read(vertCount);
	vertexBuffer.resize(vertCount);
	vertexUVs.resize(vertCount);
	vertexNormals.resize(vertCount);
	boneIndices.resize(vertCount);
	boneWeights.resize(vertCount);


	glm::vec3 minVertex(FLT_MAX);
	glm::vec3 maxVertex(-FLT_MAX);

	for (unsigned int i = 0; i < vertCount; i++)
	{
		glm::vec3& position = vertexBuffer.at(i);
		glm::vec2& uv = vertexUVs.at(i);
		glm::vec3& normal = vertexNormals.at(i);
		BoneIndex& boneIndex = boneIndices.at(i);
		glm::vec4& weights = boneWeights.at(i);

		pFile->Read(position.x, 3);
		pFile->Read(normal.x, 3);
		pFile->Read(uv.x, 2);
		pFile->Read(weights.x, 4);
		pFile->Read(boneIndex.elements[0], 4);

		minVertex.x = std::min(minVertex.x, position.x);
		maxVertex.x = std::max(maxVertex.x, position.x);
		minVertex.y = std::min(minVertex.y, position.y);
		maxVertex.y = std::max(maxVertex.y, position.y);
		minVertex.z = std::min(minVertex.z, position.z);
		maxVertex.z = std::max(maxVertex.z, position.z);
	}

	boundingBox[0] = minVertex;
	boundingBox[1] = maxVertex;

	calcVertexNormalsForDisplay();
	return true;
}

void Mesh::ReadSkeleton(FileObject* pFile)
{
	if (binParserMode == false)
	{
		std::cout << "Activate bin parser mode" << std::endl;
		return;
	}

	unsigned int boneCount;
	pFile->Read(boneCount);
	skeleton.resize(boneCount);
	initialBones.resize(boneCount * 2);

	// loop through bones
	for (unsigned int i = 0; i < boneCount; i++)
	{
		Bone& bone = skeleton[i];
		pFile->Read(bone.name);
		pFile->Read(bone.parentID);
		Vqs toModel;
		Vqs toBone;
		ReadVqs(pFile, bone.toModelFromBone);
		ReadVqs(pFile, bone.toBoneFromModel);

		// @@@@ TODO: figure out appropriate unit bone
		constexpr float BONE_SCALE = 0.5f;
		initialBones[i * 2] = bone.toModelFromBone * glm::vec3(0.f, 0.f, 0.f);
		initialBones[i * 2 + 1] = bone.toModelFromBone * glm::vec3(0.f, BONE_SCALE, 0.f);
	}
}

void Mesh::ReadAnimation(FileObject* pFile)
{
	if (binParserMode == false)
	{
		std::cout << "Activate bin parser mode" << std::endl;
		return;
	}
	animations.push_back(Animation());
	Animation& animation = animations.back();
	pFile->Read(animation.duration);
	unsigned int trackCount;
	pFile->Read(trackCount);
	animation.tracks.resize(trackCount);
	for (unsigned int i = 0; i < trackCount; i++)
	{
		Track& track = animation.tracks[i];
		unsigned int keyFrameCount;
		pFile->Read(keyFrameCount);
		track.keyFrames.resize(keyFrameCount);

		for (unsigned int k = 0; k < keyFrameCount; k++)
		{
			KeyFrame& frame = track.keyFrames[k];
			pFile->Read(frame.time);
			ReadVqs(pFile, frame.toModelFromBone);
		}   // End reading in key frames
	}   // End reading in tracks

}

void Mesh::ReadVqs(FileObject* pFile, Vqs& vqs)
{
	pFile->Read(vqs.v.x, 3);
	pFile->Read(vqs.q.x, 4);
	vqs.s = 1.f;        // Set scale to identity
}

void Mesh::CalculateInverseKinematics(glm::vec3 targetPositionInModelSpace, bool enforced, int interpolatedPositionCount)
{
	eularAngleConstraints = initEularAngleConstraints;
	inverseKinematicMatrices.clear();

	std::vector<Vqs> frameVQS;
	GetAnimationTransform(frameVQS, false);
	const int frameSize = static_cast<int>(frameVQS.size());
	inverseKinematicMatrices.push_back(std::vector<glm::mat4>(static_cast<size_t>(frameSize)));

	for (int i = 0; i < frameSize; i++)
	{
		inverseKinematicMatrices[0][i] = glm::translate(frameVQS[i].v) * ConvertToMatrix4(frameVQS[i].q) * glm::scale(glm::vec3(frameVQS[i].s));
	}
	std::vector<glm::mat4> initFrame = inverseKinematicMatrices[0];
	MakeHierchical(initFrame, frameSize);

	// Interpolate here
	this->interpolatedPositionCount = interpolatedPositionCount;
	std::vector<glm::vec3> interpolatedPositions(interpolatedPositionCount + 1);
	const glm::vec4 currentPositionV4 = initFrame[endEffectorIndex] * glm::vec4(0.f, 0.f, 0.f, 1.f);
	glm::vec3 currentPositionV3(currentPositionV4.x, currentPositionV4.y, currentPositionV4.z);
	//float angleBetween = 1.f;

	interpolatedPositions[0] = currentPositionV3;
	float t = 1.f / interpolatedPositionCount;
	for (int i = 1; i <= interpolatedPositionCount; i++)
	{
		interpolatedPositions[i] = (1.f - (t * i)) * currentPositionV3 + (t * i) * targetPositionInModelSpace;
	}

	// Frame will be hierarchical after copied.
	inverseKinematicMatrices.resize(interpolatedPositionCount + 1);
	for (int i = 1; i <= interpolatedPositionCount; i++)
	{
		glm::vec3 resultPosition;
		// Initialize with non hierarchical animation data
		inverseKinematicMatrices[i] = (inverseKinematicMatrices[i - 1]);
		interpolatedPositions[i] = CalculateInverseKinematics(inverseKinematicMatrices[i], frameSize, interpolatedPositions[i], interpolatedPositions[i - 1], enforced);
	}
	test = interpolatedPositions[interpolatedPositionCount];

	inverseKinematicMatrices[0] = initFrame;
	for (size_t i = 1; i < inverseKinematicMatrices.size(); i++)
	{
		MakeHierchical(inverseKinematicMatrices[i], frameSize);
	}
}

void Mesh::GetInverseKinematicAnimationTransform(std::vector<glm::mat4>& transform)
{
	if (interpolatedPositionCount <= 0)
	{
		// Should Call CalculateInverseKinematics first!
		return;
	}
	constexpr float maxTime = 3.f;
	if (animTimer >= maxTime)
	{
		transform = inverseKinematicMatrices.back();
		return;
	}

	const float stepSize = maxTime / interpolatedPositionCount;
	int transformIndex = static_cast<int>(animTimer / stepSize);
	int nextTransformIndex = transformIndex + 1;

	const size_t frameSize = inverseKinematicMatrices[0].size();
	transform.resize(frameSize);
	for (size_t i = 0; i < frameSize; i++)
	{
		float t = (animTimer - (transformIndex * stepSize)) / stepSize;

		transform[i] = (1 - t) * inverseKinematicMatrices[transformIndex][i] + (t * inverseKinematicMatrices[nextTransformIndex][i]);
	}
}

void Mesh::InitManipulator(size_t endEffector)
{
	manipulator.clear();
	eularAngleConstraints.clear();

	const static float pi = glm::pi<float>();
	manipulator.push_back(6);
	eularAngleConstraints.push_back(std::pair(glm::vec3(0.f, -pi / 8.f, -pi / 8.f), glm::vec3(pi / 2.f, pi / 8.f, pi / 8.f)));
	manipulator.push_back(29);
	eularAngleConstraints.push_back(std::pair(glm::vec3(-pi / 4.f, 0.f, -pi / 4.f), glm::vec3(pi / 8.f, 0.f, pi / 4.f)));
	manipulator.push_back(28);
	eularAngleConstraints.push_back(std::pair(glm::vec3(-pi / 2.f, -pi / 2.f, -pi / 4.f), glm::vec3(2.5f * pi / 8.f, pi / 4.f, 0.f)));
	manipulator.push_back(26);
	eularAngleConstraints.push_back(std::pair(glm::vec3(-pi / 4.f, 0.f, -pi / 8.f), glm::vec3(pi / 4.f, 0.f, pi / 2.f)));
	manipulator.push_back(25);
	eularAngleConstraints.push_back(std::pair(glm::vec3(-pi / 16.f, 0.f, 0.f), glm::vec3(pi / 16.f, 0.f, pi / 4.f)));

	initEularAngleConstraints = eularAngleConstraints;

	endEffectorIndex = 31;
}

glm::vec3 Mesh::GetTest()
{
	return test;
}

void Mesh::GetInit(std::vector<glm::mat4>& data)
{
	data = inverseKinematicMatrices[0];
}

void Mesh::GetFirst(std::vector<glm::mat4>& data)
{
	data = inverseKinematicMatrices[1];
}

void Mesh::MakeHierchical(std::vector<glm::mat4>& frame, const int frameSize)
{
	for (int i = 0; i < frameSize; i++)
	{
		if (int parentID = skeleton[i].parentID;
			parentID >= 0)
		{
			frame[i] = frame[parentID] * frame[i];
		}
	}
}

glm::vec3 Mesh::CalculateInverseKinematics(std::vector<glm::mat4>& frame, const int frameSize, const glm::vec3 targetPositionInModelSpace, const glm::vec3 startPosition, bool enforced)
{
	std::vector<glm::mat4> hierchicalFrame(frame);
	MakeHierchical(hierchicalFrame, frameSize);

	constexpr float targetAccuracyThreshold = 0.05f;
	constexpr float movementDetectionThreshold = 0.05f;
	const size_t manipulatorSize = manipulator.size();

	// let asume the current position is the first keyframes of the first animation.
	glm::vec3 previousEndEffectorPosition;
	glm::vec3 endEffectorPosition(startPosition);
	bool isEEpointedTarget = false;
	float movement = 0.f;
	int i = 0;
	do
	{
		i++;
		previousEndEffectorPosition = endEffectorPosition;

		for (size_t manipulatorIndex = 0; manipulatorIndex < manipulatorSize; manipulatorIndex++)
		{
			const glm::vec4 joint = hierchicalFrame[manipulator[manipulatorIndex]] * glm::vec4(0.f, 0.f, 0.f, 1.f);
			const glm::vec3 targetVector = glm::normalize(glm::vec3(targetPositionInModelSpace.x - joint.x, targetPositionInModelSpace.y - joint.y, targetPositionInModelSpace.z - joint.z));
			const glm::vec3 endEffectorVector = glm::normalize(glm::vec3(endEffectorPosition.x - joint.x, endEffectorPosition.y - joint.y, endEffectorPosition.z - joint.z));

			// Get rotation variables
			float angleBetweenTargetAndEE = acosf(glm::dot(endEffectorVector, targetVector));
			glm::vec3 rotationAxis = glm::cross(endEffectorVector, targetVector);


			// Rotate lk by R_vk(ak) hierarchically
			if (angleBetweenTargetAndEE > std::numeric_limits<float>::epsilon() || angleBetweenTargetAndEE < -std::numeric_limits<float>::epsilon())
			{
				glm::mat4 rotationMatrix = glm::rotate(angleBetweenTargetAndEE, rotationAxis);

				if (enforced)
				{
					glm::vec3 eularAngles = ExportEulerAngleData(rotationMatrix);
					ClampAngles(eularAngles, manipulatorIndex);
					rotationMatrix = glm::rotate(eularAngles.x, glm::vec3(1.f, 0.f, 0.f)) * glm::rotate(eularAngles.y, glm::vec3(0.f, 1.f, 0.f)) * glm::rotate(eularAngles.z, glm::vec3(0.f, 0.f, 1.f));
				}

				frame[manipulator[manipulatorIndex]] = frame[manipulator[manipulatorIndex]] * rotationMatrix;
			}
			hierchicalFrame = frame;
			MakeHierchical(hierchicalFrame, frameSize);

			const glm::vec4 tmp = hierchicalFrame[endEffectorIndex] * glm::vec4(0.f, 0.f, 0.f, 1.f);
			endEffectorPosition.x = tmp.x;
			endEffectorPosition.y = tmp.y;
			endEffectorPosition.z = tmp.z;

			const glm::vec3 newEndEffectorVector = glm::normalize(glm::vec3(endEffectorPosition.x - joint.x, endEffectorPosition.y - joint.y, endEffectorPosition.z - joint.z));
			// If current EE indicates target,
			if (glm::length(endEffectorPosition - targetPositionInModelSpace) < targetAccuracyThreshold)
			{
				isEEpointedTarget = true;
				break;
			}
		}
		movement = glm::length(endEffectorPosition - previousEndEffectorPosition);
	} while (movement > movementDetectionThreshold && !isEEpointedTarget && i <= 800);

	return endEffectorPosition;
}

glm::vec3 Mesh::ExportEulerAngleData(glm::mat4 rotationMatrix)
{
	const static float pi = glm::pi<float>();
	float xAngle, yAngle, zAngle;
	float sinY = rotationMatrix[2][0];
	if (sinY  < 1.f && sinY  > -1.f)
	{
		float yCandidate0 = -asinf(sinY);
		float yCandidate1 = pi - yCandidate0;

		// Select positive angle
		if (abs(yCandidate0) > abs(yCandidate1))
		{
			yAngle = yCandidate0;
		}
		else
		{
			yAngle = yCandidate1;
		}

		const float cosY0 = cosf(yAngle);
		xAngle = atan2f(rotationMatrix[2][1] / cosY0, rotationMatrix[2][2] / cosY0);

		zAngle = atan2f(rotationMatrix[1][0] / cosY0, rotationMatrix[0][0] / cosY0);

	}
	else
	{
		zAngle = 0.f;

		if (sinY <= -1.f)
		{
			yAngle = pi / 2.f;
			xAngle = zAngle + atan2f(rotationMatrix[0][1], rotationMatrix[0][2]);
		}
		else
		{
			yAngle = -pi / 2.f;
			xAngle = -zAngle + atan2f(-rotationMatrix[0][1], -rotationMatrix[0][2]);
		}
	}
	return glm::vec3(xAngle, yAngle, zAngle);
}

void Mesh::ClampAngles(glm::vec3& eularAngles, size_t manipulatorIndex)
{
	// Clamp angles
	for (int i = 0; i < 3; i++)
	{
		if (eularAngles[i] < eularAngleConstraints[manipulatorIndex].first[i])
		{
			eularAngles[i] = eularAngleConstraints[manipulatorIndex].first[i];
		}
		else if (eularAngles[i] > eularAngleConstraints[manipulatorIndex].second[i])
		{
			eularAngles[i] = eularAngleConstraints[manipulatorIndex].second[i];
		}
	}

	// Update constaints
	for (int i = 0; i < 3; i++)
	{
		eularAngleConstraints[manipulatorIndex].first[i] -= eularAngles[i];
		eularAngleConstraints[manipulatorIndex].second[i] -= eularAngles[i];
	}
}
