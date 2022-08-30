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

uniform sampler2D front;
uniform sampler2D left;
uniform sampler2D right;
uniform sampler2D back;
uniform sampler2D top;
uniform sampler2D bottom;

in vec3 fragPos;
in vec3 vertexNormal;

in vec3 view;
in float distanceView;
in vec3 reflection;
in vec3 refractionR;
in vec3 refractionG;
in vec3 refractionB;
in float ratio;

uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float ns;
uniform float highlightTightness;
uniform float zNear;
uniform float zFar;
uniform vec3 intensityEmissive;
uniform vec3 intensityFog;
uniform vec3 attenuationConstants;

uniform float transmissionCoefficient;

uniform float mappingType;
uniform float envPhongScaler;

uniform int lightSize;

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

out vec3 color;

//////////// Lighting calculation starts from right here.

vec3 CalculatePointLight(TheStruct item)
{
	vec3 lightVector = item.lightPosition - fragPos;
	float distanceLight = length(lightVector);
	lightVector = normalize(lightVector);
	vec3 lightReflection = 2*dot(vertexNormal, lightVector)*vertexNormal - lightVector;

	vec3 lightAmbient = ambient * item.intensityAmbient;
	vec3 lightDiffuse = diffuse * max(dot(vertexNormal, lightVector), 0) * item.intensityDiffuse;
	vec3 lightSpecular = specular * pow(max(dot(lightReflection , view), 0), ns) * item.intensitySpecular;
	lightSpecular.r = max(lightSpecular.r, 0);
	lightSpecular.g = max(lightSpecular.g, 0);
	lightSpecular.b = max(lightSpecular.b, 0);

	float attenuation = min(1/(attenuationConstants.x + attenuationConstants.y*distanceLight + attenuationConstants.z * distanceLight*distanceLight), 1);
	vec3 intensityLocal = attenuation*(lightAmbient + lightDiffuse + lightSpecular);

	return intensityLocal;
}

vec3 CalculateSpotlightEffect(TheStruct item, float alpha)
{
	vec3 lightVector = item.lightPosition - fragPos;
	float distanceLight = length(lightVector);
	lightVector = normalize(lightVector);
	vec3 lightReflection  = 2*dot(vertexNormal, lightVector)*vertexNormal - lightVector;

	vec3 lightAmbient = ambient * item.intensityAmbient;
	vec3 lightDiffuse = diffuse * max(dot(lightVector, vertexNormal), 0) * item.intensityDiffuse;
	vec3 lightSpecular = specular * pow(max(dot(view, lightReflection ), 0), ns) * item.intensitySpecular;
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
	vec3 toVertex = normalize(fragPos - item.lightPosition);
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
	vec3 lightReflection  = 2*dot(vertexNormal, lightVector)*vertexNormal - lightVector;

	vec3 lightAmbient = ambient * item.intensityAmbient;
	vec3 lightDiffuse = diffuse * max(dot(lightVector, vertexNormal), 0) * item.intensityDiffuse;
	vec3 lightSpecular = specular * pow(max(dot(view, lightReflection ), 0), ns) * item.intensitySpecular;
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
/////////////////////////

vec3 CalculateSkyboxCubemap(vec3 directionVector)
{
	float x = directionVector.x;
	float y = directionVector.y;
	float z = directionVector.z;

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

vec3 CalculateEnvironmentMappingColor()
{
	// // Only reflection
	vec3 reflectColor = CalculateSkyboxCubemap(reflection);
	if(mappingType >= -0.5 && mappingType < 0.5)
	{
		return reflectColor;
	}

	// Only Refraction
	vec3 refractColor;
	refractColor.r = CalculateSkyboxCubemap(refractionR).r;
	refractColor.g = CalculateSkyboxCubemap(refractionG).g;
	refractColor.b = CalculateSkyboxCubemap(refractionB).b;
	if(mappingType >=0.5 && mappingType < 1.5)
	{
		return refractColor;
	}
	
	if(refractColor.r < 0.1 &&
	refractColor.g < 0.1 &&
	refractColor.b < 0.1)
	{
		discard;
	}

	return mix(refractColor, reflectColor, ratio);
}

vec3 CalculatePhongShadingColor()
{
	vec3 result;
	result = intensityEmissive;
	
	for(int i = 0; i < lightSize; i++)
	{
		result = result + CalculateIntensityFinal(data.item[i]);
	}



	float s = (zFar - distanceView) / (zFar - zNear);
	result = s*result + (1-s)*intensityFog;	
	return result;
}

void main()
{
	vec3 envColor = CalculateEnvironmentMappingColor();

	if(envColor.r < 0.1 &&
	envColor.g < 0.1 &&
	envColor.b < 0.1)
	{
	discard;
	}

	vec3 phongColor = CalculatePhongShadingColor();

	color = mix(envColor, phongColor, envPhongScaler);
}