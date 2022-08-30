/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Affine.cpp
Purpose: My math file whic contains Affine, Matrix, Point, Vector
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS300_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 9/22/2021
End Header --------------------------------------------------------*/

#ifndef CS250_AFFINE_H
#define CS250_AFFINE_H

#include <cmath>
#include <cassert>


struct Hcoord {
  float x, y, z, w;
  explicit Hcoord(float X=0, float Y=0, float Z=0, float W=0);
  float& operator[](int i) { return *(&x+i); }
  float operator[](int i) const { return *(&x+i); }
  static bool _near(float x, float y) { return std::abs(x-y) < 1e-5f; }
};


struct Point : Hcoord {
  explicit Point(float X=0, float Y=0, float Z=0);
  Point(const Hcoord& v) : Hcoord(v) { assert(_near(w,1)); }
};
  

struct Vector : Hcoord {
  explicit Vector(float X=0, float Y=0, float Z=0);
  Vector(const Hcoord& v) : Hcoord(v) { assert(_near(w,0)); }
};


struct Matrix {
  Hcoord row[4];
  Hcoord& operator[](int i) { return row[i]; }
  const Hcoord& operator[](int i) const { return row[i]; }
};


struct Affine : Matrix {
  Affine(void);
  Affine(const Vector& Lx, const Vector& Ly, const Vector& Lz, const Point& D);
  Affine(const Matrix& M) : Matrix(M)                 
      { assert(Hcoord::_near(M[3][0],0) && Hcoord::_near(M[3][1],0)
               && Hcoord::_near(M[3][2],0) && Hcoord::_near(M[3][3],1)); }
};


Hcoord operator+(const Hcoord& u, const Hcoord& v);
Hcoord operator-(const Hcoord& u, const Hcoord& v);
Hcoord operator-(const Hcoord& v);
Hcoord operator*(float r, const Hcoord& v);
Hcoord operator*(const Matrix& A, const Hcoord& v);
Matrix operator*(const Matrix& A, const Matrix& B);
float dot(const Vector& u, const Vector& v);
float abs(const Vector& v);
Vector normalize(const Vector& v);
Vector cross(const Vector& u, const Vector& v);
Affine rotate(float t, const Vector& v);
Affine translate(const Vector& v);
Affine scale(float r);
Affine scale(float rx, float ry, float rz);
Affine inverse(const Affine& A);


#endif

