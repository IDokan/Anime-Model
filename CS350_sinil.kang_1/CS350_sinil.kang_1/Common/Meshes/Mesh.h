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

#include <glm/glm.hpp>

typedef float GLfloat;
typedef unsigned int GLuint;
typedef unsigned char GLboolean;

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

    // Get attribute values buffer
    GLfloat *getVertexBuffer();             // attribute 0
    GLfloat *getVertexNormals();            // attribute 1
    GLfloat *getVertexUVs();                // attribute 2

    GLfloat *getVertexNormalsForDisplay();  // attribute 0
    GLfloat* getFaceNormalsForDisplay();

    unsigned int getVertexBufferSize();
    unsigned int getVertexCount();
    unsigned int getVertexNormalCount();
    unsigned int getFaceNormalCount();

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

private:
    std::vector<glm::vec3>    vertexBuffer;
    std::vector<GLuint>       vertexIndices;
    std::vector<glm::vec2>    vertexUVs;
    std::vector<glm::vec3>    vertexNormals, vertexNormalDisplay, faceNormalDisplay;

    glm::vec3               boundingBox[2];
    GLfloat                 normalLength = 0.f;
};


#endif //SIMPLE_OBJ_SCENE_MESH_H
