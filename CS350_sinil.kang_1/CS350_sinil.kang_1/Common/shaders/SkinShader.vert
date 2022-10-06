///* Start Header -------------------------------------------------------
//Copyright (C) FALL2021 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: As1DiffuseShader.vert
//Purpose: Ambient and diffuse shader for Assignment1
//Language: GLSL
//Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
//Project: sinil.kang_CS300_1
//Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
//Creation date: 9/22/2021
//End Header --------------------------------------------------------*/

#version 410 core

uniform mat4 objToWorld;
uniform mat4 worldToNDC;
uniform vec3 camera;

layout(location = 0) in vec3 objPosition;
layout(location = 1) in vec3 objNormal;
layout(location = 2) in vec2 objUV;
layout(location = 3) in ivec4 boneIndex;
layout(location = 4) in vec4 weights;

struct TheStruct
{
	mat4 toModelFromBone;
};

layout(std140) uniform Block
{
	TheStruct item[99];
} data;

out vec3 fragPos;
out vec3 vertexNormal;
out vec3 viewVector;

void main()
{
	mat4 boneTransform = data.item[boneIndex[0]].toModelFromBone * weights[0];
	boneTransform += data.item[boneIndex[1]].toModelFromBone * weights[1];
	boneTransform += data.item[boneIndex[2]].toModelFromBone * weights[2];
	boneTransform += data.item[boneIndex[3]].toModelFromBone * weights[3];


	vertexNormal = normalize(mat3(transpose(inverse(objToWorld))) * objNormal);
	fragPos = vec3(objToWorld * boneTransform * vec4(objPosition, 1.0f));
	viewVector = normalize(camera - fragPos);


	gl_Position = worldToNDC * objToWorld * boneTransform * vec4(objPosition, 1.0f);
}