//
// Created by pushpak on 10/5/18.
//

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
Creation date: 9/20/2021
End Header --------------------------------------------------------*/

#ifndef SIMPLE_OBJ_SCENE_MESH_H
#define SIMPLE_OBJ_SCENE_MESH_H

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "../Common/Meshes/binFileSources/binFileStructs.h"

typedef float GLfloat;
typedef unsigned int GLuint;
typedef unsigned char GLboolean;
class FileObject;

class Mesh
{
public:
    // calculate texture coordinates
    enum class UVType {
        SPHERICAL_UV= 1,
        CYLINDRICAL_UV,
        CUBE_MAPPED_UV
    };
public:
    friend class OBJReader;
    friend class MyObjReader;
    friend class MeshGenerator;

    Mesh(bool binParserMode = false);

    // Get attribute values buffer
    GLfloat *getVertexBuffer();             // attribute 0
    GLfloat *getVertexNormals();            // attribute 1
    GLfloat *getVertexUVs();                // attribute 2

    GLfloat *getVertexNormalsForDisplay();  // attribute 0
    GLfloat* getFaceNormalsForDisplay();
    GLfloat* GetBonesForDisplay();

    GLubyte* GetBoneIDs();
    GLfloat* GetBoneWeights();

    unsigned int getVertexBufferSize();
    unsigned int getVertexCount();
    unsigned int getVertexNormalCount();
    unsigned int getFaceNormalCount();
    unsigned int GetBoneCountForDisplay();
    const std::vector<Bone>& GetSkeleton();

    // Get vertex index buffer
    GLuint *getIndexBuffer();
    unsigned int getIndexBufferSize();
    unsigned int getTriangleCount();

    glm::vec3   getModelScale();
    glm::vec3   getModelCentroid();
    glm::vec3   getCentroidVector( glm::vec3 vVertex );


    GLfloat  &  getNormalLength();
    void setNormalLength( GLfloat nLength );

    // initialize the data members
    void initData();

    // calculate vertex normals, display normals, and display face normals
    int calcVertexNormals(GLboolean bFlipNormals = false);

    // calculate the "display" normals
    void calcVertexNormalsForDisplay(GLboolean bFlipNormals = false);

    int         calcUVs( Mesh::UVType uvType = Mesh::UVType::CYLINDRICAL_UV);
    glm::vec2   calcCubeMap( glm::vec3 vEntity );

    glm::mat4 calcAdjustBoundingBoxMatrix();

    void GetToBoneFromModel(std::vector<Vqs>& toBoneFromModel);
    void GetAnimationTransform(float& time, std::vector<Vqs>& transforms, bool isSkeleton);
    float GetAnimationDuration();
/// Bin parser functions

    bool LoadBinFile(const std::string& path);
    bool ParseBinFile(FileObject* pFile);
    bool ReadMesh(FileObject* pFile);
    void ReadSkeleton(FileObject* pFile);
    void ReadAnimation(FileObject* pFile);
    void ReadVqs(FileObject* pFile, Vqs& vqs);

private:
    // Models
    std::vector<glm::vec3>    vertexBuffer;
    std::vector<GLuint>       vertexIndices;
    std::vector<glm::vec2>    vertexUVs;
    std::vector<glm::vec3>    vertexNormals, vertexNormalDisplay, faceNormalDisplay;

private:
    // Skeletons
    std::vector<BoneIndex> boneIndices;
    std::vector<glm::vec4> boneWeights;
    std::vector<Bone> skeleton;
    std::vector<glm::vec3> initialBones;

private:
    // Animations
    std::vector<Animation> animations;
    

    // boundingBox[0] -> minimum point, boundingBox[1] -> maximum point
    glm::vec3               boundingBox[2];
    GLfloat                 normalLength = 0.f;

    bool binParserMode;
};


#endif //SIMPLE_OBJ_SCENE_MESH_H
