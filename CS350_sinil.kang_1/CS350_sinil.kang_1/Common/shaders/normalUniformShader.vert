///* Start Header -------------------------------------------------------
//Copyright (C) FALL2021 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: normalDisplayShader.frag
//Purpose: shader for vertex and face normals.
//Language: GLSL
//Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
//Project: sinil.kang_CS300_1
//Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
//Creation date: 9/22/2021
//End Header --------------------------------------------------------*/

#version 410 core
layout (location = 0) in vec3 position;

uniform mat4 worldToNDC;
uniform mat4 objToWorld;

struct TheStruct
{
	mat4 toModelFromBone;
};

layout(std140) uniform Block
{
	TheStruct item[99];
} data;

void main()
{
	int id = gl_VertexID / 2;
	vec4 tp = data.item[id].toModelFromBone * vec4(position, 1.f);
	
	gl_Position = worldToNDC * objToWorld * tp;
}