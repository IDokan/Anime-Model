/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: LightManager.h
Purpose: LightManager header code
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS300_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 11/2/2021
End Header --------------------------------------------------------*/

#include <vector>
#include <../Common/Lights/Light.h>



class LightManager
{
public:
	LightManager();
	void SetLightSize(unsigned size);
	const GLchar** GetLightUniformBlockNames();
	const float* * GetLightUniformBlockData();
	const int GetLightUniformDataSize();
	int* GetCurrentLightSizeReference();
	Light& GetLightReference(unsigned index);
public:
	static constexpr int propertySize = 8;
	static constexpr unsigned MAX_LIGHT = 16;
	static constexpr int lightUniformDataSize = propertySize * MAX_LIGHT;

private:
	std::vector<Light> lights;
	int currentLightSize;
};