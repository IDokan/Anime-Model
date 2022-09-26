/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Projection.cpp
Purpose: Projection calculate codes
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS300_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 9/30/2021
End Header --------------------------------------------------------*/
#include "Projection.h"
#include "Affine.h"
#include "Camera.h"

Affine CameraToWorld(const Camera & cam)
{
	return Affine(cam.Right(), cam.Up(), cam.Back(), cam.Eye());
}

Affine WorldToCamera(const Camera & cam)
{
	return inverse(CameraToWorld(cam));
}

Matrix CameraToNDC(const Camera & cam)
{
	Matrix result;
	Vector viewpoertGeometry = cam.ViewportGeometry();
	float twiceDistance = 2 * viewpoertGeometry.z;
	float near = cam.NearDistance();
	float far = cam.FarDistance();
	result[0][0] = twiceDistance / viewpoertGeometry.x;
	result[1][1] = twiceDistance / viewpoertGeometry.y;
	result[2][2] = (near + far) / (near - far);
	result[2][3] = (2 * near * far) / (near - far);
	result[3][2] = -1.f;
	return result;
}

glm::mat4 GetWorldToNDC(const Camera& cam)
{
	Matrix wTN = (CameraToNDC(cam) * WorldToCamera(cam));
	glm::mat4 worldToNDC;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			worldToNDC[i][j] = wTN[i][j];
		}
	}
	worldToNDC = glm::transpose(worldToNDC);

	return worldToNDC;
}


//Hcoord deviceToWorld(SDL_Window* window, Hcoord clickedPoint, Camera camera)
//{
//	Hcoord viewportGeometry = camera.ViewportGeometry();
//	float nearDistanceRatio = camera.NearDistance() / viewportGeometry.z;
//	viewportGeometry.x = nearDistanceRatio * viewportGeometry.x;
//	viewportGeometry.y = nearDistanceRatio * viewportGeometry.y;
//	int devWidth;
//	int devHeight;
//	SDL_GetWindowSize(window, &devWidth, &devHeight);
//	// Translation is implicitly omitted
//	Affine dev2ndc = (translate(Vector((1.f - devWidth) / devWidth, ((devHeight - 1.f) / devHeight))) * scale(2.f / devWidth, -2.f / devHeight, 0.f));
//	clickedPoint = (scale(viewportGeometry.x / 2.f, viewportGeometry.y / 2.f, 1.f)) * dev2ndc * clickedPoint;
//	clickedPoint.z = -camera.NearDistance();
//
//	// now clicked point is in world space
//	return CameraToWorld(camera) * clickedPoint;
//}