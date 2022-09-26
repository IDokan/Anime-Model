
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
	result[1][0] = 2 * (q.x * q.y - q.s * q.z);
	result[1][1] = 1 - 2 * (q.x * q.x + q.z * q.z);
	result[1][2] = 2 * (q.y * q.z + q.s * q.x);
	result[2][0] = 2 * (q.x * q.z + q.s * q.y);
	result[2][1] = 2 * (q.y * q.z - q.s * q.x);
	result[2][2] = 1 - 2 * (q.x * q.x + q.y * q.y);
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
	result[0][0] = 1 - 2 * (q.y * q.y + q.z * q.z);
	result[0][1] = 2 * (q.x * q.y + q.s * q.z);
	result[0][2] = 2 * (q.x * q.z - q.s * q.y);
	result[1][0] = 2 * (q.x * q.y - q.s * q.z);
	result[1][1] = 1 - 2 * (q.x * q.x + q.z * q.z);
	result[1][2] = 2 * (q.y * q.z + q.s * q.x);
	result[2][0] = 2 * (q.x * q.z + q.s * q.y);
	result[2][1] = 2 * (q.y * q.z - q.s * q.x);
	result[2][2] = 1 - 2 * (q.x * q.x + q.y * q.y);
	return result;
}

Vqs operator*(const Vqs& lhs, const Vqs& rhs) noexcept
{
	Vqs result;
	result.s = lhs.s * rhs.s;
	result.q = lhs.q * rhs.q;
	result.v = lhs * rhs.v;
	return result;
}

glm::vec3 operator*(const Vqs& vqs, glm::vec3 v) noexcept
{
	return ConvertToMatrix3(vqs.q) * (vqs.s * v) + vqs.v;
}
