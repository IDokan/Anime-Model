///* Start Header -------------------------------------------------------
//Copyright (C) FALL2021 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: As2PhongShading.frag
//Purpose:	 This is the implementation of the Phong model 
//					 where the lighting computations are implemented in the fragment shader.
//Language: GLSL
//Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
//Project: sinil.kang_CS300_1
//Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
//Creation date: 10/20/2021
//End Header --------------------------------------------------------*/

#version 410 core

out vec3 color;

in vec3 worldPos;
in vec3 vertexNormal;
in vec3 view;
in float distanceView;
in vec2 outUv;


uniform float ambient;
uniform float diffuse;
uniform float specular;
uniform float ns;
uniform float zNear;
uniform float zFar;
uniform vec3 intensityEmissive;
uniform vec3 intensityFog;
uniform vec3 attenuationConstants;

uniform int lightSize;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

struct TheStruct
{
	vec3 lightPosition;
	vec3 intensityAmbient;
	vec3 intensityDiffuse;
	vec3 intensitySpecular;
	float lightType;
	vec3 lightDirection;
	vec2 anglesInnerOuter;
	float spotFalloff;
};

layout(std140) uniform Block
{
	TheStruct item[99];
} data;

vec3 CalculatePointLight(TheStruct item)
{
	vec3 lightVector = item.lightPosition - worldPos;
	float distanceLight = length(lightVector);
	lightVector = normalize(lightVector);
	vec3 reflection = 2*dot(vertexNormal, lightVector)*vertexNormal - lightVector;

	vec3 lightAmbient = ambient * item.intensityAmbient;
	vec3 lightDiffuse = texture(diffuseTexture, outUv).rgb * max(dot(vertexNormal, lightVector), 0) * item.intensityDiffuse;
	vec3 lightSpecular = texture(specularTexture,outUv).rgb* pow(max(dot(reflection, view), 0), ns) * item.intensitySpecular;
	lightSpecular.r = max(lightSpecular.r, 0);
	lightSpecular.g = max(lightSpecular.g, 0);
	lightSpecular.b = max(lightSpecular.b, 0);

	float attenuation = min(1/(attenuationConstants.x + attenuationConstants.y*distanceLight + attenuationConstants.z * distanceLight*distanceLight), 1);
	vec3 intensityLocal = attenuation*(lightAmbient + lightDiffuse + lightSpecular);

	return intensityLocal;
}

vec3 CalculateSpotlightEffect(TheStruct item, float alpha)
{
	vec3 lightVector = item.lightPosition - worldPos;
	float distanceLight = length(lightVector);
	lightVector = normalize(lightVector);
	vec3 reflection = 2*dot(vertexNormal, lightVector)*vertexNormal - lightVector;

	vec3 lightAmbient = ambient * item.intensityAmbient;
	vec3 lightDiffuse = texture(diffuseTexture, outUv).rgb * max(dot(lightVector, vertexNormal), 0) * item.intensityDiffuse;
	vec3 lightSpecular = texture(specularTexture,outUv).rgb* pow(max(dot(view, reflection), 0), ns) * item.intensitySpecular;
	lightSpecular.r = max(lightSpecular.r, 0);
	lightSpecular.g = max(lightSpecular.g, 0);
	lightSpecular.b = max(lightSpecular.b, 0);

	float attenuation = min(1/(attenuationConstants.x + attenuationConstants.y*distanceLight + attenuationConstants.z * distanceLight*distanceLight), 1);
	float spotlightEffect = pow((alpha - cos(item.anglesInnerOuter.y)) / (cos(item.anglesInnerOuter.x) - cos(item.anglesInnerOuter.y)), item.spotFalloff);
	vec3 intensityLocal = attenuation*(lightAmbient + (spotlightEffect * (lightDiffuse + lightSpecular)));

	return intensityLocal;
}

vec3 CalculateSpotLight(TheStruct item)
{
	vec3 direction = normalize(item.lightDirection);
	vec3 toVertex = normalize(worldPos - item.lightPosition);
	float ld = dot(direction, toVertex);

	if(ld < cos(item.anglesInnerOuter.y))
	{
		return vec3(0, 0, 0);
	}
	else if(ld > cos(item.anglesInnerOuter.x))
	{
		return CalculatePointLight(item);
	}
	else 
	{
		return CalculateSpotlightEffect(item, ld);
	}

	return vec3(0, 0, 0);
}

vec3 CalculateDirectionalLight(TheStruct item)
{
	vec3 lightVector = -item.lightDirection;
	lightVector = normalize(lightVector);
	vec3 reflection = 2*dot(vertexNormal, lightVector)*vertexNormal - lightVector;

	vec3 lightAmbient = ambient * item.intensityAmbient;
	vec3 lightDiffuse = texture(diffuseTexture, outUv).rgb * max(dot(lightVector, vertexNormal), 0) * item.intensityDiffuse;
	vec3 lightSpecular = texture(specularTexture,outUv).rgb* pow(max(dot(view, reflection), 0), ns) * item.intensitySpecular;
	lightSpecular.r = max(lightSpecular.r, 0);
	lightSpecular.g = max(lightSpecular.g, 0);
	lightSpecular.b = max(lightSpecular.b, 0);

	vec3 intensityLocal = (lightAmbient + lightDiffuse + lightSpecular);

	return intensityLocal;
}

vec3 CalculateIntensityFinal(TheStruct item)
{
	if(item.lightType >= -5.f && item.lightType <= 5.f)
	{
		return CalculatePointLight(item);
	}
	else if(item.lightType > 5.f && item.lightType <= 15.f)
	{
		return CalculateSpotLight(item);
	}
	else
	{
		return CalculateDirectionalLight(item);
	}
}

void main()
{
color = intensityEmissive;

for(int i = 0; i < lightSize; i++)
{
color = color + CalculateIntensityFinal(data.item[i]);
}
color = color;



	float s = (zFar - distanceView) / (zFar - zNear);
	color = s*color + (1-s)*intensityFog;
}