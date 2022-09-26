/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Light.cpp
Purpose: Light source code
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS300_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 11/2/2021
End Header --------------------------------------------------------*/

#include <../Common/Lights/Light.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/detail/type_mat4x4.hpp>

Light::Light()
	: type(0.f), ambient(1.f, 1.f, 1.f), diffuse(1.f, 1.f, 1.f), specular(1.f, 1.f, 1.f), position(0.f, 0.f, 0.f), direction(0.f, 0.f, 0.f), angles(0.f, 0.f), spotFalloff(0.f),
	translation(glm::vec3(0.f, 0.f, 0.f)), rotationAxis(glm::vec3(0.f, 1.f, 0.f)), scaler(1.f), initAngle(0.f), shouldStopLight(false)
{
}

float* Light::GetTypePointer()
{
	return &type;
}

float* Light::GetAmbientPointer()
{
	return reinterpret_cast<float*>(&ambient);
}

float* Light::GetDiffusePointer()
{
	return reinterpret_cast<float*>(&diffuse);
}

float* Light::GetSpecularPointer()
{
	return reinterpret_cast<float*>(&specular);
}

float* Light::GetPositionPointer()
{
	return reinterpret_cast<float*>(&position);
}

float* Light::GetDirectionPointer()
{
	return reinterpret_cast<float*>(&direction);
}

float* Light::GetAnglesPointer()
{
	return reinterpret_cast<float*>(&angles);
}

float* Light::GetSpotFalloff()
{
	return reinterpret_cast<float*>(&spotFalloff);
}



void Light::UpdateLightPosition(float angleOfRotate)
{
	if (shouldStopLight == true)
	{
		return;
	}

	matrix = glm::rotate(initAngle, rotationAxis) * glm::rotate(angleOfRotate * scaler, rotationAxis) * glm::translate(translation);
	glm::vec4 p = matrix * glm::vec4(0.f, 0.f, 0.f, 1.f);
	position = glm::vec3(p.x, p.y, p.z);
}

void Light::SetLightTranslation(glm::vec3 _translation)
{
	translation = _translation;
}

void Light::SetLightRotationAxis(glm::vec3 _rotationAxis, float _scaler)
{
	rotationAxis = _rotationAxis;
	scaler = _scaler;
}

glm::vec3 Light::GetLightTranslation()
{
	return translation;
}

glm::vec3 Light::GetLightRotationAxis()
{
	return rotationAxis;
}

float* Light::GetLightRotationScalerReference()
{
	return &scaler;
}

float* Light::GetInitLightRotationReference()
{
	return &initAngle;
}

glm::vec3 Light::GetLightPosition()
{
	return position;
}

void Light::SetLightDirection(glm::vec3 _direction)
{
	direction = _direction;
}

void Light::SetLightAngles(float inner, float outer)
{
	angles.x = inner;
	angles.y = outer;
}

void Light::SetLightSpotFalloff(float _spotFalloff)
{
	spotFalloff = _spotFalloff;
}

bool Light::GetShouldStopLight()
{
	return shouldStopLight;
}

void Light::SetShouldStopLight(bool stop)
{
	shouldStopLight = stop;
}

void Light::SetLightColor(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular)
{
	ambient = _ambient;
	diffuse = _diffuse;
	specular = _specular;
}

glm::mat4 Light::GetMatrix()
{
	return matrix;
}
