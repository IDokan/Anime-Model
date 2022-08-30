/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Affine.h
Purpose: My math file whic contains Affine, Matrix, Point, Vector
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS300_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 9/22/2021
End Header --------------------------------------------------------*/
#include "Affine.h"

namespace Helper
{
	void Swap(float& a, float& b)
	{
		float tmp = a;
		a = b;
		b = tmp;
	}

	Affine Scale(float scaler, const Matrix& matrix)
	{
		Affine result;
		for (int row = 0; row < 3; row++)
		{
			for (int col = 0; col < 3; col++)
			{
				result[row][col] = matrix[row][col] * scaler;
			}
		}
		return result;
	}

	Affine OuterProductMatrix(const Vector& v)
	{
		// Based on 7th slide of https://distance.sg.digipen.edu/file.php/3767/notes/01_affine.pdf
		Affine result;
		for (int row = 0; row < 3; row++)
		{
			for (int col = 0; col < 3; col++)
			{
				result[row][col] = v[row] * v[col];
			}
		}
		return result;
	}

	Affine CrossProductMatrix(const Vector& v)
	{
		// Based on 7th slide of https://distance.sg.digipen.edu/file.php/3767/notes/01_affine.pdf
		Affine result;
		result[0][1] = -v.z;
		result[0][2] = v.y;
		result[1][0] = v.z;
		result[1][2] = -v.x;
		result[2][0] = -v.y;
		result[2][1] = v.x;
		return result;
	}

	Affine Addition(const Matrix& matrix1, const Matrix& matrix2)
	{
		Affine result;
		for (int row = 0; row < 3; row++)
		{
			for (int col = 0; col < 3; col++)
			{
				result[row][col] = matrix1[row][col] + matrix2[row][col];
			}
		}
		return result;
	}

	Affine GetInverseOfTranslationMatrix(const Affine& matrix)
	{
		Affine result = scale(1.f);
		// Calculating Inverse translation is easy
		for (int row = 0; row < 3; row++)
		{
			result[row][3] = -matrix[row][3];
		}

		return result;
	}

	Affine GetInverseOfLinearMatrix(const Affine& matrix)
	{
		// In order to get an inverse matrix of linear part, we need det(Matrix) and adj(Matrix).
		// Calculate det(A)
		const float determinant = (matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[2][1] * matrix[1][2]))
			- (matrix[0][1] * (matrix[1][0] * matrix[2][2] - matrix[2][0] * matrix[1][2]))
			+ (matrix[0][2] * (matrix[1][0] * matrix[2][1] - matrix[2][0] * matrix[1][1]));

		Affine inverseLinear;
		for (int row = 0; row < 3; row++)
		{
			for (int col = 0; col < 3; col++)
			{
				inverseLinear[row][col] = matrix[row][col];
			}
		}

		// Transpose
		Helper::Swap(inverseLinear[1][0], inverseLinear[0][1]);
		Helper::Swap(inverseLinear[2][0], inverseLinear[0][2]);
		Helper::Swap(inverseLinear[2][1], inverseLinear[1][2]);

		// Find an adj(A)
		Affine result;
		result[0][0] = inverseLinear[1][1] * inverseLinear[2][2] - inverseLinear[2][1] * inverseLinear[1][2];
		result[0][1] = inverseLinear[2][0] * inverseLinear[1][2] - inverseLinear[1][0] * inverseLinear[2][2];
		result[0][2] = inverseLinear[1][0] * inverseLinear[2][1] - inverseLinear[2][0] * inverseLinear[1][1];
		result[1][0] = inverseLinear[2][1] * inverseLinear[0][2] - inverseLinear[0][1] * inverseLinear[2][2];
		result[1][1] = inverseLinear[0][0] * inverseLinear[2][2] - inverseLinear[2][0] * inverseLinear[0][2];
		result[1][2] = inverseLinear[2][0] * inverseLinear[0][1] - inverseLinear[0][0] * inverseLinear[2][1];
		result[2][0] = inverseLinear[0][1] * inverseLinear[1][2] - inverseLinear[1][1] * inverseLinear[0][2];
		result[2][1] = inverseLinear[1][0] * inverseLinear[0][2] - inverseLinear[0][0] * inverseLinear[1][2];
		result[2][2] = inverseLinear[0][0] * inverseLinear[1][1] - inverseLinear[1][0] * inverseLinear[0][1];

		// Calculate 1 / det(A) * Adj(A)
		result = Helper::Scale(1 / determinant, result);

		return result;
	}
}

Hcoord::Hcoord(float X, float Y, float Z, float W)
	: x(X), y(Y), z(Z), w(W)
{
}

Point::Point(float X, float Y, float Z)
// W is initialized 1 because last component of point in Homogeneous is 1
	:Hcoord(X, Y, Z, 1.f)
{
}

Vector::Vector(float X, float Y, float Z)
// W is initialized 0 because last component of vector in Homogeneous is 0
	:Hcoord(X, Y, Z, 0.f)
{
}

Affine::Affine(void)
{
	// The last component of the matrix for uniform scaling by 0 with respect to the origin would be 1
	row[3].w = 1.f;
}

Affine::Affine(const Vector& Lx, const Vector& Ly, const Vector& Lz, const Point& D)
{
	row[0] = Hcoord(Lx.x, Ly.x, Lz.x, D.x);
	row[1] = Hcoord(Lx.y, Ly.y, Lz.y, D.y);
	row[2] = Hcoord(Lx.z, Ly.z, Lz.z, D.z);
	row[3] = Hcoord(Lx.w, Ly.w, Lz.w, D.w);
}

Hcoord operator+(const Hcoord& u, const Hcoord& v)
{
	return Hcoord(
		u.x + v.x,
		u.y + v.y,
		u.z + v.z,
		u.w + v.w
		);
}

Hcoord operator-(const Hcoord& u, const Hcoord& v)
{
	return Hcoord(
		u.x - v.x,
		u.y - v.y,
		u.z - v.z,
		u.w - v.w
	);
}

Hcoord operator-(const Hcoord& v)
{
	return Hcoord(
		-v.x,
		-v.y,
		-v.z,
		-v.w
	);
}

Hcoord operator*(float r, const Hcoord& v)
{
	return Hcoord(
		r * v.x,
		r * v.y,
		r * v.z,
		r * v.w
	);
}

Hcoord operator*(const Matrix& A, const Hcoord& v)
{
	// based on formula multiplied by 4x4 Matrix and 4x1 Hcoord.
	return Hcoord(
		A[0][0] * v.x + A[0][1] * v.y + A[0][2] * v.z + A[0][3] * v.w,
		A[1][0] * v.x + A[1][1] * v.y + A[1][2] * v.z + A[1][3] * v.w,
		A[2][0] * v.x + A[2][1] * v.y + A[2][2] * v.z + A[2][3] * v.w,
		A[3][0] * v.x + A[3][1] * v.y + A[3][2] * v.z + A[3][3] * v.w
	);
}

Matrix operator*(const Matrix& A, const Matrix& B)
{
	// based on formula multiplied by two 4x4 Matrices.
	Matrix result;
	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{
			for (int i = 0; i < 4; i++)
			{
				result[row][col] += A[row][i] * B[i][col];
			}
		}
	}

	return result;
}

float dot(const Vector& u, const Vector& v)
{
	return 
		u.x*v.x +
		u.y*v.y + 
		u.z*v.z
		;
}

float abs(const Vector& v)
{
	return sqrt(dot(v, v));
}

Vector normalize(const Vector& v)
{
	// formula of normalize is (v / magnitude(v))
	return (1 / abs(v)) * v;
}

Vector cross(const Vector& u, const Vector& v)
{
	return Vector(
		u.y * v.z - u.z * v.y,
		- (u.x * v.z - u.z * v.x),
		u.x * v.y - u.y * v.x
	);
}

Affine rotate(float t, const Vector& v)
{
	Affine result;

	const float cosTheta = cos(t);
	const float sinTheta = sin(t);
	const float magnitude = abs(v);

	result = Helper::Scale(cosTheta, scale(1));
	result = Helper::Addition(result, Helper::Scale((1 - cosTheta) / (magnitude * magnitude), Helper::OuterProductMatrix(v)));
	result = Helper::Addition(result, Helper::Scale(sinTheta / magnitude, Helper::CrossProductMatrix(v)));

	return result;
}

Affine translate(const Vector& v)
{
	Affine translation;
	for (int i = 0; i < 3; i++)
	{
		translation[i][i] = 1.f;
	}
	for (int row = 0; row < 3; row++)
	{
		translation[row][3] = v[row];
	}
	return translation;
}

Affine scale(float r)
{
	Affine scale;
	for (int  i = 0; i < 3; i++)
	{
		scale[i][i] = r;
	}
	return scale;
}

// returns the affine transformation Hrx,ry,rz for inhomogeneous scaling 
// by factors rx,ry,rz with respect to the origin.
Affine scale(float rx, float ry, float rz)
{
	Affine scale;
	scale[0][0] = rx;
	scale[1][1] = ry;
	scale[2][2] = rz;
	return scale;
}

Affine inverse(const Affine& A)
{
	// Divide A into linear part and translation part
	Affine resultOfInverseTranslation = Helper::GetInverseOfTranslationMatrix(A);
	Affine resultOfInverseLinear = Helper::GetInverseOfLinearMatrix(A);

	// merge resultOfInverseTranslation and inverseLinear
	return resultOfInverseLinear * resultOfInverseTranslation;
}

