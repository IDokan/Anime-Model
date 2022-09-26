/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Camera.cpp
Purpose: Camera source codes
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS300_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 9/30/2021
End Header --------------------------------------------------------*/
#include <cmath>
#include "Camera.h"

Camera::Camera(void)
	:eye(), right(1.f, 0.f, 0.f), up(0.f, 1.f, 0.f), back(0.f, 0.f, 1.f), width(1.f), height(1.f), distance(2.f), m_near(0.1f), m_far(10.f)
{
}

Camera::Camera(const Point& E, const Vector& look, const Vector& vp, float fov, float aspect, float _near, float _far)
	: eye(E), m_near(_near), m_far(_far)
{
	back = -normalize(look);
	right = normalize(cross(vp, back));
	up = cross(back, right);

	float tanOfFov = tanf(fov / 2.f);
	width = 1.f;
	distance = width / (2.f * tanOfFov);
	height = width / aspect;
}

Point Camera::Eye(void) const
{
	return eye;
}

Vector Camera::Right(void) const
{
	return right;
}

Vector Camera::Up(void) const
{
	return up;
}

Vector Camera::Back(void) const
{
	return back;
}

Vector Camera::ViewportGeometry(void) const
{
	return Vector(width, height, distance);
}

float Camera::NearDistance(void) const
{
	return m_near;
}

float Camera::FarDistance(void) const
{
	return m_far;
}

Camera& Camera::Zoom(float factor)
{
	width *= factor;
	height *= factor;
	return *this;
}

Camera& Camera::Forward(float distance_increment)
{
	distance += distance_increment;
	eye = eye + distance_increment * (-back);
	return *this;
}

Camera& Camera::Yaw(float angle)
{
	Affine Rotation = rotate(angle, up);
	right = Rotation * right;
	back = Rotation * back;

	return *this;
}

Camera& Camera::Pitch(float angle)
{
	Affine Rotation = rotate(angle, right);
	up = Rotation * up;
	back = Rotation * back;

	return *this;
}

Camera& Camera::Roll(float angle)
{
	Affine Rotation = rotate(angle, back);
	up = Rotation * up;
	right = Rotation * right;

	return *this;
}
