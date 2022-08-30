///* Start Header -------------------------------------------------------
//Copyright (C) FALL2021 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: As3EnvironmentalMapping.frag
//Purpose: shader for assignment3 environmental mapping.
//Language: GLSL
//Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
//Project: sinil.kang_CS300_1
//Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
//Creation date: 11/30/2021
//End Header --------------------------------------------------------*/

#version 410 core


uniform mat4 objToWorld;
uniform mat4 worldToNDC;

uniform vec3 CameraPosition;

uniform float transmissionCoefficient;
uniform float chromaticAberration;

layout(location = 0) in vec3 objPosition;
layout(location = 1) in vec3 objNormal;

out vec3 fragPos;
out vec3 vertexNormal;
out vec3 view;
out float distanceView;
out vec3 reflection;
out vec3 refractionR;
out vec3 refractionG;
out vec3 refractionB;
out float ratio;

vec3 CalculateRefractVector(float eta)
{
	vec3 result;
	float transmitRefraction = sqrt(1 - eta * eta * (1 - pow(dot(vertexNormal, view), 2)));
	if(transmitRefraction < 0)
	{
		result = vec3(0);
	}
	else
	{
		result = eta * view - (eta * dot(vertexNormal, view) + sqrt(transmitRefraction)) * vertexNormal;
	}
	return result;
}

void main()
{


	vertexNormal = mat3(transpose(inverse(objToWorld))) * objNormal;
	vertexNormal = normalize(vertexNormal);

	fragPos = vec3(objToWorld * vec4(objPosition, 1.0f));
	gl_Position = worldToNDC * objToWorld * vec4(objPosition, 1.0f);

	view = (CameraPosition - fragPos);
	distanceView = length(view);
	view = normalize(view);

	reflection = 2*vertexNormal*dot(vertexNormal, view) - view;
	
	refractionR = CalculateRefractVector(transmissionCoefficient - chromaticAberration);
	refractionG = CalculateRefractVector(transmissionCoefficient);
	refractionB = CalculateRefractVector(transmissionCoefficient + chromaticAberration);

	
	float FresnelPower = 5.0;
	float F = ((1.0 - transmissionCoefficient) * (1.0 - transmissionCoefficient)) / ((1.0 + transmissionCoefficient) * (1.0 + transmissionCoefficient));
	ratio = F + (1.0 - F) * pow((1.0 - dot(view, vertexNormal)), FresnelPower);
	ratio = clamp(ratio, 0, 1);
}