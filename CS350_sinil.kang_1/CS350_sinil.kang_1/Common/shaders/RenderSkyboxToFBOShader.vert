///* Start Header -------------------------------------------------------
//Copyright (C) FALL2021 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: RenderSkyboxToFBOShader.vert
//Purpose: shader skybox for frame buffer object.
//Language: GLSL
//Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
//Project: sinil.kang_CS300_1
//Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
//Creation date: 11/29/2021
//End Header --------------------------------------------------------*/

#version 410 core

uniform mat4 objToWorld;
uniform mat4 worldToNDC;

uniform vec3 modelCentroid;

layout(location = 0) in vec3 objPosition;
layout(location = 1) in vec3 objNormal;

out vec3 fragPos;
out vec3 vertexNormal;

out vec3 modelPosition;

void main()
{
	vertexNormal = mat3(transpose(inverse(objToWorld))) * objNormal;
	fragPos = vec3(objToWorld * vec4(objPosition, 1.0f));
	gl_Position = worldToNDC * objToWorld * vec4(objPosition, 1.0f);

	modelPosition = objPosition - modelCentroid;
}