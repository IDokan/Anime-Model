/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Camera.h
Purpose: Camera header codes
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS300_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 9/30/2021
End Header --------------------------------------------------------*/

#ifndef CS250_CAMERA_H
#define CS250_CAMERA_H

#include "Affine.h"


class Camera {
  public:
    Camera(void);
    Camera(const Point& E, const Vector& look, const Vector& vp,
           float fov, float aspect, float _near, float _far);
    Point Eye(void) const;
    Vector Right(void) const;
    Vector Up(void) const;
    Vector Back(void) const;
    Vector ViewportGeometry(void) const;
    float NearDistance(void) const;
    float FarDistance(void) const;
    Camera& Zoom(float factor);
    Camera& Forward(float distance_increment);
    Camera& Yaw(float angle);
    Camera& Pitch(float angle);
    Camera& Roll(float angle);
  private:
    Point eye;
    Vector right, up, back;
    float width, height, distance,
          m_near, m_far;
};


#endif

