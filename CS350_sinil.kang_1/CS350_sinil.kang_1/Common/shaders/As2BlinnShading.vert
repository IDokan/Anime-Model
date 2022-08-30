///* Start Header -------------------------------------------------------
//Copyright (C) FALL2021 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: As2BlinnShading.vert
//Purpose:	The variation to the Phong Shading (shader 2 above) 
//					where we do not have to calculate the reflection vector (expensive part of the computation). 
//					Instead, we use the half-vector between the light vector (L) and view vector (V), 
//					and combine it with the surface normal for the specular computation. 
//					The lighting calculations are done in the fragment shader.
//Language: GLSL
//Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
//Project: sinil.kang_CS300_1
//Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
//Creation date: 11/6/2021
//End Header --------------------------------------------------------*/

#version 410 core
#extension GL_ARB_uniform_buffer_object : enable

uniform mat4 objToWorld;
uniform mat4 worldToNDC;
uniform vec3 CameraPosition;

layout(location = 0) in vec3 objPosition;
layout(location = 1) in vec3 objNormal;
layout(location = 2) in vec2 objUVs;

out vec3 worldPos;
out vec3 vertexNormal;
out vec3 view;
out float distanceView;
out vec2 outUv;


uniform int uvIdentifier;
uniform vec3 center;


vec2 CalculateUVOnGPU(vec3 vertex)
{
	vec2 uv = vec2(0);
	vec3 modelCentroid = normalize(vertex - center);
	
	if(uvIdentifier < -5)
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
		uv.y = 1 - (phi / 3.14);
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
	outUv = CalculateUVOnGPU(objPosition);

// Vertex transformation
	worldPos = vec3(objToWorld * vec4(objPosition, 1.0f));
	gl_Position = worldToNDC * objToWorld * vec4(objPosition, 1.0f);

	
	vertexNormal = mat3(transpose(inverse(objToWorld))) * objNormal;
	vertexNormal = normalize(vertexNormal);

	view = CameraPosition - worldPos;
	distanceView = length(view);
	view = normalize(view);
}