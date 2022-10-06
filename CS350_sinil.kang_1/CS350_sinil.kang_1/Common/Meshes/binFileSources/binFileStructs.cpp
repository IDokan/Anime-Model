
#include <glm/glm.hpp>
#include <iostream>
#include "binFileStructs.h"

Quaternion::Quaternion() noexcept
	:x(0), y(0), z(0), s(1)
{
}

Quaternion::Quaternion(float x, float y, float z, float s) noexcept
	:x(x), y(y), z(z), s(s)
{
}

Quaternion::Quaternion(glm::vec3 v, float s) noexcept
	:x(v.x), y(v.y), z(v.z), s(s)
{
}

Quaternion::Quaternion(float s, glm::vec3 v) noexcept
	:x(v.x), y(v.y), z(v.z), s(s)
{
}

Quaternion::~Quaternion()
{
}

Quaternion::Quaternion(const Quaternion& other)
	:x(other.x), y(other.y), z(other.z), s(other.s)
{

}

Quaternion::Quaternion(Quaternion&& other) noexcept
	: x(other.x), y(other.y), z(other.z), s(other.s)
{
}

Quaternion& Quaternion::operator=(const Quaternion& other)
{
	v = other.v;
	s = other.s;
	return *this;
}

Quaternion& Quaternion::operator=(Quaternion&& other)
{
	v = other.v;
	s = other.s;
	return *this;
}

Bone::Bone()
	:name(""), parentID(-1), toModelFromBone(), toBoneFromModel()
{
}

Bone::Bone(const Bone& other)
	: name(other.name), parentID(other.parentID), toModelFromBone(other.toModelFromBone), toBoneFromModel(toBoneFromModel)
{
}

Bone::Bone(Bone&& other)
	: name(other.name), parentID(other.parentID), toModelFromBone(other.toModelFromBone), toBoneFromModel(toBoneFromModel)
{
}

Bone& Bone::operator=(const Bone& other)
{
	name = other.name;
	parentID = other.parentID;
	toModelFromBone = other.toModelFromBone;
	toBoneFromModel = other.toBoneFromModel;

	return *this;
}

Bone& Bone::operator=(Bone&& other)
{
	name = other.name;
	parentID = other.parentID;
	toModelFromBone = other.toModelFromBone;
	toBoneFromModel = other.toBoneFromModel;

	return *this;
}

const Vqs& Bone::GetToModelFromBone() const
{
	return toModelFromBone;
}

Vqs::Vqs()
	: v(glm::vec3(0)), q(), s(1.f)
{
}

Vqs::Vqs(const Vqs& other)
	: v(other.v), q(other.q), s(other.s)
{
}

Vqs::Vqs(Vqs&& other)
	: v(other.v), q(other.q), s(other.s)
{
}

Vqs& Vqs::operator=(const Vqs& other)
{
	v = other.v;
	q = other.q;
	s = other.s;
	return *this;
}

Vqs& Vqs::operator=(Vqs&& other)
{
	v = other.v;
	q = other.q;
	s = other.s;
	return *this;
}

void operator+=(Quaternion& q1, const Quaternion& q2) noexcept
{
	q1.v += q2.v;
	q1.s += q2.s;
}

void operator-=(Quaternion& q1, const Quaternion& q2) noexcept
{
	operator+=(q1, -q2);
}

void operator*=(Quaternion& q1, float scale) noexcept
{
	q1.v *= scale;
	q1.s *= scale;
}

void operator/=(Quaternion& q1, float divisor) noexcept
{
	operator*=(q1, 1.f / divisor);
}

Quaternion operator-(const Quaternion& q1) noexcept
{
	return Quaternion(-q1.v, -q1.s);
}

Quaternion operator+(const Quaternion& q1, const Quaternion& q2) noexcept
{
	return Quaternion(q1.v + q2.v, q1.s + q2.s);
}

Quaternion operator-(const Quaternion& q1, const Quaternion& q2) noexcept
{
	return operator+(q1, -q2);
}

Quaternion operator*(const Quaternion& q, float scale) noexcept
{
	return Quaternion(q.v * scale, q.s * scale);
}

Quaternion operator*(float scale, const Quaternion& q) noexcept
{
	return operator*(q, scale);
}

Quaternion operator/(const Quaternion& q, float divisor) noexcept
{
	return operator*(q, 1.f / divisor);
}

Quaternion operator*(const Quaternion& q1, const Quaternion& q2) noexcept
{
	return Quaternion(q1.s * q2.v + q2.s * q1.v + glm::cross(q1.v, q2.v), q1.s * q2.s - glm::dot(q1.v, q2.v));
}

void operator*=(Quaternion& q1, const Quaternion& q2) noexcept
{
	glm::vec3 v1 = q1.v;
	float s1 = q1.s;

	q1.v = s1 * q2.v + q2.s * v1 + glm::cross(v1, q2.v);
	q1.s = s1 * q2.s - glm::dot(v1, q2.v);
}

float dot(const Quaternion& q1, const Quaternion& q2) noexcept
{
	return q1.s * q2.s + glm::dot(q1.v, q2.v);
}

float dot(Quaternion&& q1, Quaternion&& q2) noexcept
{
	return q1.s * q2.s + glm::dot(q1.v, q2.v);
}

float magnitude_squared(const Quaternion& q) noexcept
{
	return q.x * q.x + q.y * q.y + q.z * q.z + q.s * q.s;
}

float magnitude_squared(Quaternion&& q) noexcept
{
	return q.x * q.x + q.y * q.y + q.z * q.z + q.s * q.s;
}

float magnitude(const Quaternion& q) noexcept
{
	return sqrt(magnitude_squared(q));
}

float magnitude(Quaternion&& q) noexcept
{
	return sqrt(magnitude_squared(q));
}

Quaternion Inverse(const Quaternion& q) noexcept
{
	return Quaternion(-q.v, q.s) / magnitude_squared(q);
}

Quaternion Inverse(Quaternion&& q) noexcept
{
	return Quaternion(-q.v, q.s) / magnitude_squared(q);
}

void slerp(const Quaternion& q0, const Quaternion& q1, const float t, Quaternion& q)
{
	float cosHalfTheta = dot(q0, q1);

	if (std::abs(cosHalfTheta) >= 1.0)
	{
		q = q0;
		return;
	}

	bool reverseQ1 = false;
	if (cosHalfTheta < 0)	// Always follow the shortest path
	{
		reverseQ1 = true;
		cosHalfTheta = -cosHalfTheta;
	}

	// Calculate temporary values.
	const float halfTheta = acos(cosHalfTheta);
	const float sinHalfTheta = std::sqrt(1.0f - (cosHalfTheta * cosHalfTheta));
	// if theta = 180 degrees then result is not fully defined
	// we could rotate around any axis normal to qa or qb
	if (std::abs(sinHalfTheta) < 0.001)
	{
		if (!reverseQ1)
		{
			q = (1 - t) * q0 + t * q1;
		}
		else
		{
			q = (1 - t) * q0 - t * q1;
		}
		return;
	}

	const float A = sin((1 - t) * halfTheta) / sinHalfTheta;
	const float B = sin(t * halfTheta) / sinHalfTheta;
	if (!reverseQ1)
	{
		q = A * q0 + B * q1;
	}
	else
	{
		q = A * q0 - B * q1;
	}
}

glm::mat4 ConvertToMatrix4(const Quaternion& q) noexcept
{
	// if (magnitude_squared(q) < 1.f - std::numeric_limits<float>::epsilon() ||
	// 	magnitude_squared(q) > 1.f + std::numeric_limits<float>::epsilon())
	// {
	// 	std::cout << "Given quaternion is not a unit quaternion." << std::endl;
	// 	abort();
	// }
	glm::mat4 result;
	result[0][0] = 1 - 2*(q.y * q.y + q.z*q.z);
	result[0][1] = 2*(q.x*q.y + q.s*q.z);
	result[0][2] = 2 * (q.x * q.z - q.s * q.y);
	result[0][3] = 0.f;
	result[1][0] = 2 * (q.x * q.y - q.s * q.z);
	result[1][1] = 1 - 2 * (q.x * q.x + q.z * q.z);
	result[1][2] = 2 * (q.y * q.z + q.s * q.x);
	result[1][3] = 0.f;
	result[2][0] = 2 * (q.x * q.z + q.s * q.y);
	result[2][1] = 2 * (q.y * q.z - q.s * q.x);
	result[2][2] = 1 - 2 * (q.x * q.x + q.y * q.y);
	result[2][3] = 0.f;
	result[3][0] = 0.f;
	result[3][1] = 0.f;
	result[3][2] = 0.f;
	result[3][3] = 1.f;
	return result;
}

glm::mat3 ConvertToMatrix3(const Quaternion& q) noexcept
{
	// if (magnitude_squared(q) < 1.f - std::numeric_limits<float>::epsilon() ||
	// 	magnitude_squared(q) > 1.f + std::numeric_limits<float>::epsilon())
	// {
	// 	std::cout << "Given quaternion is not a unit quaternion." << std::endl;
	// 	abort();
	// }
	glm::mat3 result;
	result[0][0] = 1.f - 2.f * (q.y * q.y + q.z * q.z);
	result[0][1] = 2.f * (q.x * q.y + q.s * q.z);
	result[0][2] = 2.f * (q.x * q.z - q.s * q.y);
	result[1][0] = 2.f * (q.x * q.y - q.s * q.z);
	result[1][1] = 1.f - 2.f * (q.x * q.x + q.z * q.z);
	result[1][2] = 2.f * (q.y * q.z + q.s * q.x);
	result[2][0] = 2.f * (q.x * q.z + q.s * q.y);
	result[2][1] = 2.f * (q.y * q.z - q.s * q.x);
	result[2][2] = 1.f - 2.f * (q.x * q.x + q.y * q.y);
	return result;
}

Vqs operator*(const Vqs& vqs, float scaler) noexcept
{
	Vqs result;
	result.s = vqs.s * scaler;
	result.v = vqs.v * scaler;
	result.q = vqs.q * scaler;
	return result;
}

Vqs operator*(float scaler, const Vqs& vqs) noexcept
{
	return vqs * scaler;
}

Vqs operator*(const Vqs& lhs, const Vqs& rhs) noexcept
{
	Vqs result;
	result.s = lhs.s * rhs.s;
	result.q = lhs.q * rhs.q;
	result.v = lhs * rhs.v;
	return result;
}

Vqs operator+(const Vqs& lhs, const Vqs& rhs) noexcept
{
	Vqs result;
	result.q = lhs.q + rhs.q;
	result.v = lhs.v + rhs.v;
	result.s = lhs.s + rhs.s;
	return result;
}

glm::vec3 operator*(const Vqs& vqs, glm::vec3 v) noexcept
{
	return ConvertToMatrix3(vqs.q) * (vqs.s * v) + vqs.v;
}
