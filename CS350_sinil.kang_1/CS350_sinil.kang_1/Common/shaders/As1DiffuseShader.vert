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

out vec3 fragPos;
out vec3 vertexNormal;
out vec3 viewVector;

void main()
{
	vertexNormal = normalize(mat3(transpose(inverse(objToWorld))) * objNormal);
	fragPos = vec3(objToWorld * vec4(objPosition, 1.0f));
	viewVector = normalize(camera - fragPos);
	gl_Position = worldToNDC * objToWorld * vec4(objPosition, 1.0f);
}