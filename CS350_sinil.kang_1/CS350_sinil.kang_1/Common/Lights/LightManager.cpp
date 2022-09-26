
/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: LightManager.cpp
Purpose: LightManager source code
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS300_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 11/2/2021
End Header --------------------------------------------------------*/

#include <GL/glew.h>
#include "LightManager.h"

LightManager::LightManager()
{
	currentLightSize = 1;
	lights.resize(MAX_LIGHT);
}

void LightManager::SetLightSize(unsigned size)
{
	if (size > MAX_LIGHT)
	{
		currentLightSize = MAX_LIGHT;
		return;
	}
	else if (size < 0)
	{
		currentLightSize = 0;
		return;
	}

	currentLightSize = size;
}

const GLchar** LightManager::GetLightUniformBlockNames()
{
	constexpr int blockNameSize = propertySize * MAX_LIGHT;
	static const GLchar* blockNames[blockNameSize] = {
	   "Block.item[0].lightPosition",							// vec3
	   "Block.item[0].intensityAmbient",					// vec3
	   "Block.item[0].intensityDiffuse",					// vec3
	   "Block.item[0].intensitySpecular",					// vec3
		"Block.item[0].lightType",
		"Block.item[0].lightDirection",
		"Block.item[0].anglesInnerOuter",
		"Block.item[0].spotFalloff",

	   "Block.item[1].lightPosition",							// vec3
	   "Block.item[1].intensityAmbient",					// vec3
	   "Block.item[1].intensityDiffuse",					// vec3
	   "Block.item[1].intensitySpecular",					// vec3
		"Block.item[1].lightType",
		"Block.item[1].lightDirection",
		"Block.item[1].anglesInnerOuter",
		"Block.item[1].spotFalloff",

	   "Block.item[2].lightPosition",							// vec3
	   "Block.item[2].intensityAmbient",					// vec3
	   "Block.item[2].intensityDiffuse",					// vec3
	   "Block.item[2].intensitySpecular",					// vec3
		"Block.item[2].lightType",
		"Block.item[2].lightDirection",
		"Block.item[2].anglesInnerOuter",
		"Block.item[2].spotFalloff",

	   "Block.item[3].lightPosition",							// vec3
	   "Block.item[3].intensityAmbient",					// vec3
	   "Block.item[3].intensityDiffuse",					// vec3
	   "Block.item[3].intensitySpecular",					// vec3
		"Block.item[3].lightType",
		"Block.item[3].lightDirection",
		"Block.item[3].anglesInnerOuter",
		"Block.item[3].spotFalloff",

	   "Block.item[4].lightPosition",							// vec3
	   "Block.item[4].intensityAmbient",					// vec3
	   "Block.item[4].intensityDiffuse",					// vec3
	   "Block.item[4].intensitySpecular",					// vec3
		"Block.item[4].lightType",
		"Block.item[4].lightDirection",
		"Block.item[4].anglesInnerOuter",
		"Block.item[4].spotFalloff",

	   "Block.item[5].lightPosition",							// vec3
	   "Block.item[5].intensityAmbient",					// vec3
	   "Block.item[5].intensityDiffuse",					// vec3
	   "Block.item[5].intensitySpecular",					// vec3
		"Block.item[5].lightType",
		"Block.item[5].lightDirection",
		"Block.item[5].anglesInnerOuter",
		"Block.item[5].spotFalloff",

	   "Block.item[6].lightPosition",							// vec3
	   "Block.item[6].intensityAmbient",					// vec3
	   "Block.item[6].intensityDiffuse",					// vec3
	   "Block.item[6].intensitySpecular",					// vec3
		"Block.item[6].lightType",
		"Block.item[6].lightDirection",
		"Block.item[6].anglesInnerOuter",
		"Block.item[6].spotFalloff",

	   "Block.item[7].lightPosition",							// vec3
	   "Block.item[7].intensityAmbient",					// vec3
	   "Block.item[7].intensityDiffuse",					// vec3
	   "Block.item[7].intensitySpecular",					// vec3
		"Block.item[7].lightType",
		"Block.item[7].lightDirection",
		"Block.item[7].anglesInnerOuter",
		"Block.item[7].spotFalloff",

	   "Block.item[8].lightPosition",							// vec3
	   "Block.item[8].intensityAmbient",					// vec3
	   "Block.item[8].intensityDiffuse",					// vec3
	   "Block.item[8].intensitySpecular",					// vec3
		"Block.item[8].lightType",
		"Block.item[8].lightDirection",
		"Block.item[8].anglesInnerOuter",
		"Block.item[8].spotFalloff",

	   "Block.item[9].lightPosition",							// vec3
	   "Block.item[9].intensityAmbient",					// vec3
	   "Block.item[9].intensityDiffuse",					// vec3
	   "Block.item[9].intensitySpecular",					// vec3
		"Block.item[9].lightType",
		"Block.item[9].lightDirection",
		"Block.item[9].anglesInnerOuter",
		"Block.item[9].spotFalloff",

	   "Block.item[10].lightPosition",							// vec3
	   "Block.item[10].intensityAmbient",					// vec3
	   "Block.item[10].intensityDiffuse",					// vec3
	   "Block.item[10].intensitySpecular",					// vec3
		"Block.item[10].lightType",
		"Block.item[10].lightDirection",
		"Block.item[10].anglesInnerOuter",
		"Block.item[10].spotFalloff",

	   "Block.item[11].lightPosition",							// vec3
	   "Block.item[11].intensityAmbient",					// vec3
	   "Block.item[11].intensityDiffuse",					// vec3
	   "Block.item[11].intensitySpecular",					// vec3
		   "Block.item[11].lightType",
		   "Block.item[11].lightDirection",
		   "Block.item[11].anglesInnerOuter",
		   "Block.item[11].spotFalloff",

	   "Block.item[12].lightPosition",							// vec3
	   "Block.item[12].intensityAmbient",					// vec3
	   "Block.item[12].intensityDiffuse",					// vec3
	   "Block.item[12].intensitySpecular",					// vec3
		   "Block.item[12].lightType",
		   "Block.item[12].lightDirection",
		   "Block.item[12].anglesInnerOuter",
		   "Block.item[12].spotFalloff",

	   "Block.item[13].lightPosition",							// vec3
	   "Block.item[13].intensityAmbient",					// vec3
	   "Block.item[13].intensityDiffuse",					// vec3
	   "Block.item[13].intensitySpecular",					// vec3
		   "Block.item[13].lightType",
		   "Block.item[13].lightDirection",
		   "Block.item[13].anglesInnerOuter",
		   "Block.item[13].spotFalloff",

	   "Block.item[14].lightPosition",							// vec3
	   "Block.item[14].intensityAmbient",					// vec3
	   "Block.item[14].intensityDiffuse",					// vec3
	   "Block.item[14].intensitySpecular",					// vec3
		   "Block.item[14].lightType",
		   "Block.item[14].lightDirection",
		   "Block.item[14].anglesInnerOuter",
		   "Block.item[14].spotFalloff",

	   "Block.item[15].lightPosition",							// vec3
	   "Block.item[15].intensityAmbient",					// vec3
	   "Block.item[15].intensityDiffuse",					// vec3
	   "Block.item[15].intensitySpecular",					// vec3
		   "Block.item[15].lightType",
		   "Block.item[15].lightDirection",
		   "Block.item[15].anglesInnerOuter",
		   "Block.item[15].spotFalloff",
	};
	return blockNames;
}

const float** LightManager::GetLightUniformBlockData()
{
	static float* data[lightUniformDataSize] = {};
	for (int i = 0; i < MAX_LIGHT; i++)
	{
		data[i * propertySize] = lights[i].GetPositionPointer();
		data[i * propertySize + 1] = lights[i].GetAmbientPointer();
		data[i * propertySize + 2] = lights[i].GetDiffusePointer();
		data[i * propertySize + 3] = lights[i].GetSpecularPointer();
		data[i * propertySize + 4] = lights[i].GetTypePointer();		// type
		data[i * propertySize + 5] = lights[i].GetDirectionPointer();			// direction
		data[i * propertySize + 6] = lights[i].GetAnglesPointer();		// angles
		data[i * propertySize + 7] = lights[i].GetSpotFalloff();		// spot falloff
	}
	return const_cast<const float**>(reinterpret_cast<float**>(data));
}

const int LightManager::GetLightUniformDataSize()
{
	return lightUniformDataSize;
}

int* LightManager::GetCurrentLightSizeReference()
{
	return &currentLightSize;
}

Light& LightManager::GetLightReference(unsigned index)
{
	return lights.at(index);
}
