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
struct Physics;

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
    friend struct Physics;
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

    std::vector<float>& getMass();

    unsigned int getVertexBufferSize();
    unsigned int getVertexCount();
    unsigned int getVertexNormalCount();
    unsigned int getFaceNormalCount();
    unsigned int GetBoneCountForDisplay();
    const std::vector<Bone>& GetSkeleton();
    void SetVertex(const size_t i, const glm::vec3 newVertex);

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
    void GetAnimationTransform(std::vector<Vqs>& transforms, bool getHierachicalResult = true);
    Vqs GetAnimationTransform(unsigned int index);
    float GetAnimationDuration();
    void SetAnimationTimer(float time);
    void UpdateAnimationTimer(float dt, float velocity);
/// Bin parser functions

    bool LoadBinFile(const std::string& path);
    bool ParseBinFile(FileObject* pFile);
    bool ReadMesh(FileObject* pFile);
    void ReadSkeleton(FileObject* pFile);
    void ReadAnimation(FileObject* pFile);
    void ReadVqs(FileObject* pFile, Vqs& vqs);

    // Inverse Kinematics
    void CalculateInverseKinematics(glm::vec3 targetPositionInModelSpace, bool enforced = false, int interpolatedPositionCount = 4);
    void GetInverseKinematicAnimationTransform(std::vector<glm::mat4>& transform);
    void InitManipulator(size_t endEffector);
    glm::vec3 GetTest();
    void GetInit(std::vector<glm::mat4>& data);
    void GetFirst(std::vector<glm::mat4>& data);

private:
    // Models
    std::vector<glm::vec3>    vertexBuffer;
    std::vector<GLuint>       vertexIndices;
    std::vector<glm::vec2>    vertexUVs;
    std::vector<glm::vec3>    vertexNormals, vertexNormalDisplay, faceNormalDisplay;
    std::vector<float> mass;

private:
    // Skeletons
    std::vector<BoneIndex> boneIndices;
    std::vector<glm::vec4> boneWeights;
    std::vector<Bone> skeleton;
    std::vector<glm::vec3> initialBones;

private:
    // Animations
    std::vector<Animation> animations;
    
    // Inverse kinematics
    void MakeHierchical(std::vector<glm::mat4>& frame, const int frameSize);
    glm::vec3 CalculateInverseKinematics(std::vector<glm::mat4>& frame, const int frameSize, const glm::vec3 targetPositionInModelSpace, const glm::vec3 startPosition, bool enforced = false);
    glm::vec3 ExportEulerAngleData(glm::mat4 rotationMatrix);
    void ClampAngles(glm::vec3& eularAngles, size_t manipulatorIndex);
    std::vector<std::vector<glm::mat4>> inverseKinematicMatrices;
    std::vector<size_t> manipulator;
    size_t endEffectorIndex;
    std::vector<std::pair<glm::vec3, glm::vec3>> initEularAngleConstraints;
    std::vector<std::pair<glm::vec3, glm::vec3>> eularAngleConstraints;
    glm::vec3 test;
    int interpolatedPositionCount;

    // boundingBox[0] -> minimum point, boundingBox[1] -> maximum point
    glm::vec3               boundingBox[2];
    GLfloat                 normalLength = 0.f;

    bool binParserMode;

    float animTimer;
    float animCyclePerSecond;
};


#endif //SIMPLE_OBJ_SCENE_MESH_H
