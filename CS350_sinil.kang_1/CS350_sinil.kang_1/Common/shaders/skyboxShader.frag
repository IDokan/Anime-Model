///* Start Header -------------------------------------------------------
//Copyright (C) FALL2021 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: skyboxShader.frag
//Purpose: shader for skybox.
//Language: GLSL
//Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
//Project: sinil.kang_CS300_1
//Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
//Creation date: 11/24/2021
//End Header --------------------------------------------------------*/

#version 410 core

uniform sampler2D front;
uniform sampler2D left;
uniform sampler2D right;
uniform sampler2D back;
uniform sampler2D top;
uniform sampler2D bottom;

in vec3 fragPos;
in vec3 vertexNormal;
in vec3 modelPosition;

out vec3 color;

vec3 CalculateSkyColor()
{
	float x = modelPosition.x;
	float y = modelPosition.y;
	float z = modelPosition.z;

	float absX = abs(x);
	float absY = abs(y);
	float absZ = abs(z);
	
	bool isXPositive = (x>0) ? true : false;
	bool isYPositive = (y>0) ? true : false;
	bool isZPositive = (z>0) ? true : false;

	float maxAxis = 0, uc = 0, vc = 0;
	vec2 uv = vec2(0, 0);

	if(isXPositive && absX >= absY && absX >= absZ)
	{
        // u (0 to 1) goes from +z to -z
        // v (0 to 1) goes from -y to +y
        maxAxis = absX;
        uc = -z;
        vc = y;

    // Convert range from -1 to 1 to 0 to 1
    uv.s = 0.5f * (uc / maxAxis + 1.0f);
    uv.t = 0.5f * (vc / maxAxis + 1.0f);

		
		return texture(right, uv).rgb;
	}
	if(!isXPositive && absX >= absY && absX >= absZ)
	{
        // u (0 to 1) goes from -z to +z
        // v (0 to 1) goes from -y to +y
        maxAxis = absX;
        uc = z;
        vc = y;

    // Convert range from -1 to 1 to 0 to 1
    uv.s = 0.5f * (uc / maxAxis + 1.0f);
    uv.t = 0.5f * (vc / maxAxis + 1.0f);


		return texture(left, uv).rgb;
	}
	if(isYPositive && absY >= absX && absY >= absZ)
	{
        // u (0 to 1) goes from -x to +x
        // v (0 to 1) goes from +z to -z
        maxAxis = absY;
        uc = x;
        vc = -z;

    // Convert range from -1 to 1 to 0 to 1
    uv.s = 0.5f * (uc / maxAxis + 1.0f);
    uv.t = 0.5f * (vc / maxAxis + 1.0f);


		return texture(top, uv).rgb;
	}
	if(!isYPositive && absY >= absX && absY >= absZ)
	{
        // u (0 to 1) goes from -x to +x
        // v (0 to 1) goes from -z to +z
        maxAxis = absY;
        uc = x;
        vc = z;

    // Convert range from -1 to 1 to 0 to 1
    uv.s = 0.5f * (uc / maxAxis + 1.0f);
    uv.t = 0.5f * (vc / maxAxis + 1.0f);


		return texture(bottom, uv).rgb;
	}
	if(isZPositive && absZ >= absX && absZ >= absY)
	{
        // u (0 to 1) goes from -x to +x
        // v (0 to 1) goes from -y to +y
        maxAxis = absZ;
        uc = x;
        vc = y;

    // Convert range from -1 to 1 to 0 to 1
    uv.s = 0.5f * (uc / maxAxis + 1.0f);
    uv.t = 0.5f * (vc / maxAxis + 1.0f);


		return texture(front, uv).rgb;
	}
	if(!isZPositive && absZ >= absX && absZ >= absY)
	{
        // u (0 to 1) goes from +x to -x
        // v (0 to 1) goes from -y to +y
        maxAxis = absZ;
        uc = -x;
        vc = y;

    // Convert range from -1 to 1 to 0 to 1
    uv.s = 0.5f * (uc / maxAxis + 1.0f);
    uv.t = 0.5f * (vc / maxAxis + 1.0f);


		return texture(back, uv).rgb;
	}
}

void main()
{
	color = CalculateSkyColor();
}