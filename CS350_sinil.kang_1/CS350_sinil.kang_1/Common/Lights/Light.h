/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Light.h
Purpose: Light header code
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS300_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 11/2/2021
End Header --------------------------------------------------------*/


#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class Light
{
public:
	enum class LightType
	{
		Point = 0,
		Spot = 10,
		Directional = 100
	};
public:
	Light();
	
	float* GetTypePointer();
	float* GetAmbientPointer();
	float* GetDiffusePointer();
	float* GetSpecularPointer();
	float* GetPositionPointer();
	float* GetDirectionPointer();
	float* GetAnglesPointer();
	float* GetSpotFalloff();


	void UpdateLightPosition(float angleOfRotate);
	void SetLightTranslation(glm::vec3 translation);
	void SetLightRotationAxis(glm::vec3 rotationAxis, float scaler = 1.f);
	glm::vec3 GetLightTranslation();
	glm::vec3 GetLightRotationAxis();
	float* GetLightRotationScalerReference();
	float* GetInitLightRotationReference();
	glm::vec3 GetLightPosition();

	void SetLightDirection(glm::vec3 direction);
	void SetLightAngles(float inner, float outer);
	void SetLightSpotFalloff(float spotFalloff);

	bool GetShouldStopLight();
	void SetShouldStopLight(bool stop);

	void SetLightColor(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);

	glm::mat4 GetMatrix();
private:
	float type;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	
	glm::vec3 position;		// for point & spot
	glm::vec3 direction;		// for spot & direction
	glm::vec2 angles; // inner & outer angle for spot light
	float spotFalloff;



	glm::vec3 translation;
	glm::vec3 rotationAxis;
	float scaler;

	float initAngle;

	bool shouldStopLight;

	glm::mat4 matrix;
};