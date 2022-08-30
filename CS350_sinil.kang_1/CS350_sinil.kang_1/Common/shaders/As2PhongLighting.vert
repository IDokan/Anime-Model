///* Start Header -------------------------------------------------------
//Copyright (C) FALL2021 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: As2PhongLighting.vertasdfadsf
//Purpose:	This shader implements the vertex-based lighting model. 
//					This is the implementation of the OpenGL compatibility pipeline (version 2.1 or older). 
//					Lighting computations are done in the vertex shader, 
//					and only the final color is interpolated to the fragments. 
//					The only “out?variable from the vertex shader should be the final color of the fragment.
//Language: GLSL
//Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
//Project: sinil.kang_CS300_1
//Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
//Creation date: 10/20/2021
//End Header --------------------------------------------------------*/

#version 410 core
#extension GL_ARB_uniform_buffer_object : enable

uniform mat4 objToWorld;
uniform mat4 worldToNDC;
uniform vec3 CameraPosition;

uniform float ambient;
uniform float diffuse;
uniform float specular;
uniform float zNear;
uniform float zFar;
uniform vec3 intensityEmissive;
uniform vec3 intensityFog;
uniform vec3 attenuationConstants;

uniform int lightSize;

uniform int uvIdentifier;
uniform vec3 center;

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

layout(location = 0) in vec3 objPosition;
layout(location = 1) in vec3 objNormal;
layout(location = 2) in vec2 objUVs;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

out vec3 colorCalculatedPerVertex;

vec3 CalculatePointLight(TheStruct item, vec3 worldPos, vec2 uv)
{
	vec3 vertexNormal = mat3(transpose(inverse(objToWorld))) * objNormal;
	vertexNormal = normalize(vertexNormal);

	vec3 lightVector = item.lightPosition - worldPos;
	float distanceLight = length(lightVector);
	lightVector = normalize(lightVector);
	vec3 view = CameraPosition - worldPos;
	view = normalize(view);
	vec3 reflection = 2*dot(vertexNormal, lightVector)*vertexNormal - lightVector;

	vec3 lightAmbient = ambient * item.intensityAmbient;
	vec3 lightDiffuse = texture(diffuseTexture, uv).rgb * max(dot(lightVector, vertexNormal), 0) * item.intensityDiffuse;
	vec3 lightSpecular = texture(specularTexture, uv).rgb * pow(max(dot(view, reflection), 0), texture(specularTexture,uv).r) * item.intensitySpecular;
	lightSpecular.r = max(lightSpecular.r, 0);
	lightSpecular.g = max(lightSpecular.g, 0);
	lightSpecular.b = max(lightSpecular.b, 0);


	float attenuation = min(1/(attenuationConstants.x + attenuationConstants.y*distanceLight + attenuationConstants.z * distanceLight*distanceLight), 1);
	vec3 intensityLocal = attenuation*(lightAmbient + lightDiffuse + lightSpecular);

	return intensityLocal;
}

vec3 CalculateDirectionalLight(TheStruct item, vec3 worldPos, vec2 uv)
{
	vec3 vertexNormal = mat3(transpose(inverse(objToWorld))) * objNormal;
	vertexNormal = normalize(vertexNormal);

	vec3 lightVector = normalize(-item.lightDirection);
	vec3 view = CameraPosition - worldPos;
	float distanceView = length(view);
	view = normalize(view);
	vec3 reflection = 2*dot(vertexNormal, lightVector)*vertexNormal - lightVector;

	vec3 lightAmbient = ambient * item.intensityAmbient;
	vec3 lightDiffuse = texture(diffuseTexture, uv).rgb * max(dot(lightVector, vertexNormal), 0) * item.intensityDiffuse;
	vec3 lightSpecular = texture(specularTexture, uv).rgb * pow(max(dot(view, reflection), 0), texture(specularTexture, uv).r) * item.intensitySpecular;
	lightSpecular.r = max(lightSpecular.r, 0);
	lightSpecular.g = max(lightSpecular.g, 0);
	lightSpecular.b = max(lightSpecular.b, 0);

	vec3 intensityLocal = (lightAmbient + lightDiffuse + lightSpecular);

	return intensityLocal;
}

vec3 CalculateSpotlightEffect(TheStruct item, vec3 worldPos, float alpha, vec2 uv)
{
	vec3 vertexNormal = mat3(transpose(inverse(objToWorld))) * objNormal;
	vertexNormal = normalize(vertexNormal);

	vec3 lightVector = item.lightPosition - worldPos;
	float distanceLight = length(lightVector);
	lightVector = normalize(lightVector);
	vec3 view = CameraPosition - worldPos;
	view = normalize(view);
	vec3 reflection = 2*dot(vertexNormal, lightVector)*vertexNormal - lightVector;

	vec3 lightAmbient = ambient * item.intensityAmbient;
	vec3 lightDiffuse = texture(diffuseTexture, uv).rgb * max(dot(lightVector, vertexNormal), 0) * item.intensityDiffuse;
	vec3 lightSpecular = texture(specularTexture, uv).rgb * pow(max(dot(view, reflection), 0), pow(texture(specularTexture,uv).r, 2)) * item.intensitySpecular;
	lightSpecular.r = max(lightSpecular.r, 0);
	lightSpecular.g = max(lightSpecular.g, 0);
	lightSpecular.b = max(lightSpecular.b, 0);

	float attenuation = min(1/(attenuationConstants.x + attenuationConstants.y*distanceLight + attenuationConstants.z * distanceLight*distanceLight), 1);
	float spotlightEffect = pow((alpha - cos(item.anglesInnerOuter.y)) / (cos(item.anglesInnerOuter.x) - cos(item.anglesInnerOuter.y)), item.spotFalloff);
	vec3 intensityLocal = attenuation*(lightAmbient + (spotlightEffect * (lightDiffuse + lightSpecular)));

	return intensityLocal;
}

vec3 CalculateSpotLight(TheStruct item, vec3 worldPos, vec2 uv)
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
		return CalculatePointLight(item, worldPos, uv);
	}
	else 
	{
		return CalculateSpotlightEffect(item, worldPos, ld, uv);
	}

	return vec3(0, 0, 0);
}

vec3 CalculateIntensityFinal(TheStruct item, vec3 worldPos, vec2 uv)
{
if(item.lightType >= -5.f && item.lightType <= 5.f)
{
return CalculatePointLight(item, worldPos, uv);
}
else if(item.lightType > 5.f && item.lightType <= 15.f)
{
return CalculateSpotLight(item, worldPos, uv);
}
else
{
return CalculateDirectionalLight(item, worldPos, uv);
}
}


vec2 CalculateUVOnGPU(vec3 vertex)
{
	vec2 uv = vec2(0);
	vec3 modelCentroid = normalize(vertex - center);
	
	if(uvIdentifier < 0)
	{								// Use CPU UVs
		return objUVs;
	}
	else if(uvIdentifier == 1)
	{								// Use Spherical
		float theta = atan(modelCentroid.y, modelCentroid.x);
		theta += 3.14;

		float z = modelCentroid.z;
		float phi = acos(z / length(modelCentroid));

		uv.x = theta / 6.28;
		uv.y = 1f - (phi / 3.14);
	}
	else if(uvIdentifier == 2)
	{								// Use Cylindrical
		float theta = atan(modelCentroid.y, modelCentroid.x);
		theta += 3.14;

		float z = (modelCentroid.z + 1) * 0.5;

		uv.x = theta / 6.28;
		uv.y = z;
	}
	else
	{								// Use Cube mapped
		float x = modelCentroid.x;
		float y = modelCentroid.y;
		float z = modelCentroid.z;

		float absX = abs(x);
		float absY = abs(y);
		float absZ = abs(z);
		
		int isXPositive = x > 0 ? 1 : 0;
		int isYPositive = y > 0 ? 1 : 0;
		int isZPositive = z > 0 ? 1 : 0;

		float maxAxis;
		float uc = 0;
		float vc = 0;
		
		// POSITIVE X
		if(bool(isXPositive) && (absX>=absY) && (absX >= absZ))
		{
			maxAxis = absX;
			uc = -z;
			vc = y;
		}
		// NEGATIVE X
		else if(!bool(isXPositive) && (absX>=absY) && (absX >= absZ))
		{
			maxAxis = absX;
			uc = z;
			vc = y;
		}
		// POSITIVE Y
		else if(bool(isYPositive) && absY>=absX && absY >= absZ)
		{
			maxAxis = absY;
			uc = x;
			vc = -z;
		}
		// NEGATIVE Y
		else if(!bool(isYPositive) && absY>= absX && absY >= absZ)
		{
			maxAxis = absY;
			uc = x;
			vc = z;
		}
		// POSITIVE Z
		else if(bool(isZPositive) && absZ >= absX && absZ >= absY)
		{
			maxAxis = absZ;
			uc = x;
			vc = y;
		}
		else if(!bool(isZPositive) && absZ >= absX && absZ >= absY)
		{
			maxAxis = absZ;
			uc = -x;
			vc = y;
		}

		uv.x = 0.5 * (uc / maxAxis + 1);
		uv.y = 0.5 * (vc / maxAxis + 1);
	}


	return uv;
}





void main()
{
	vec2 uv = CalculateUVOnGPU(objPosition);
	

// Vertex transformation
	vec3 worldPos = vec3(objToWorld * vec4(objPosition, 1.0f));
	gl_Position = worldToNDC * objToWorld * vec4(objPosition, 1.0f);

// Calculate final color	


	colorCalculatedPerVertex = intensityEmissive;
	
	for(int i = 0; i < lightSize; i++)
	{
	colorCalculatedPerVertex = colorCalculatedPerVertex + CalculateIntensityFinal(data.item[i], worldPos, uv);
	}

	
	
	float distanceView = length(CameraPosition - worldPos);
	float s = (zFar - distanceView) / (zFar - zNear);
	colorCalculatedPerVertex = s*colorCalculatedPerVertex + (1-s)*intensityFog;
}