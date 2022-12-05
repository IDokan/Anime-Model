/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Structs.cpp
Purpose: Structure for physics
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS460
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 12/01/2021
End Header --------------------------------------------------------*/
#include <../Common/Meshes/Structs.h>
#include <../Common/Meshes/Mesh.h>

Physics::Physics(Mesh&& mesh)
	:centerOfMass(glm::vec3(0.f)), translation(glm::vec3(0.f)), rotation(glm::mat4(1.f)), linearMomentum(glm::vec3(0.f)), angularMomentum(glm::vec3(0.f)),
	linearVelocity(glm::vec3(0.f)), inertiaTensorInverse(glm::mat3(0.f)), inertiaTensorObj(glm::mat3(0.f)), angularVelocity(glm::vec3(0.f)), force(glm::vec3(0.f)), torque(glm::vec3(0.f)),
	totalMass(0.f), vertices(mesh.vertexBuffer)
{
	const unsigned int vertexSize = vertices.size();

	masses = mesh.mass;
	unsigned int massSize = masses.size();


	// Initialize masses if mass is invalid.
	if (massSize < vertexSize)
	{
		massSize = vertexSize;
		masses.resize(vertexSize);
		for (unsigned int i = 0; i < vertexSize; i++)
		{
			centerOfMass += vertices[i];
			masses[i] = 1.f;
		}
		totalMass = static_cast<float>(vertexSize);
		centerOfMass /= totalMass;
		// Update mesh's mass if it is updated
		mesh.mass = masses;
	}
	else
	{
		for (unsigned int i = 0; i < vertexSize; i++)
		{
			const float mass = masses[i];
			centerOfMass += vertices[i] * mass;
			totalMass += mass;
		}
		centerOfMass /= totalMass;
	}

	// Calculate inertiaTensor
	for (unsigned int i = 0; i < vertexSize; i++)
	{
		glm::vec3 ri = (vertices[i] - centerOfMass);
		const float mass = masses[i];

		inertiaTensorObj[0][0] += mass * (ri.y * ri.y + ri.z * ri.z);
		inertiaTensorObj[1][1] += mass * (ri.x * ri.x + ri.z * ri.z);
		inertiaTensorObj[2][2] += mass * (ri.x * ri.x + ri.y * ri.y);

		inertiaTensorObj[0][1] -= mass * ri.x * ri.y;

		inertiaTensorObj[0][2] -= mass * ri.x * ri.z;

		inertiaTensorObj[1][2] -= mass * ri.y * ri.z;
	}
	inertiaTensorObj[1][0] = inertiaTensorObj[0][1];
	inertiaTensorObj[2][0] = inertiaTensorObj[0][2];
	inertiaTensorObj[2][1] = inertiaTensorObj[1][2];

	inertiaTensorInverse = rotation * glm::inverse(inertiaTensorObj) * glm::transpose(rotation);
}

Physics::~Physics()
{
}

Physics::Physics(const Physics& p)
	: centerOfMass(p.centerOfMass), translation(glm::vec3(0.f)), rotation(p.rotation), linearMomentum(p.linearMomentum), angularMomentum(p.angularMomentum),
	linearVelocity(p.linearVelocity), inertiaTensorInverse(p.inertiaTensorInverse), inertiaTensorObj(p.inertiaTensorObj), angularVelocity(p.angularVelocity), force(p.force), torque(p.torque),
	totalMass(p.totalMass), vertices(p.vertices)
{
}

Physics::Physics(Physics&& p)
	: centerOfMass(p.centerOfMass), translation(glm::vec3(0.f)), rotation(p.rotation), linearMomentum(p.linearMomentum), angularMomentum(p.angularMomentum),
	linearVelocity(p.linearVelocity), inertiaTensorInverse(p.inertiaTensorInverse), inertiaTensorObj(p.inertiaTensorObj), angularVelocity(p.angularVelocity), force(p.force), torque(p.torque),
	totalMass(p.totalMass), vertices(p.vertices)
{
}

Physics& Physics::operator=(const Physics& p)
{
	centerOfMass = p.centerOfMass;
	translation = p.translation;
	rotation = p.rotation;
	linearMomentum = p.linearMomentum;
	angularMomentum = p.angularMomentum;

	linearVelocity = p.linearMomentum;
	inertiaTensorInverse = p.inertiaTensorInverse;
	inertiaTensorObj = p.inertiaTensorObj;
	angularVelocity = p.angularVelocity;
	force = p.force;
	torque = p.torque;

	totalMass = p.totalMass;
	vertices = p.vertices;

	return *this;
}

Physics& Physics::operator=(Physics&& p)
{
	centerOfMass = p.centerOfMass;
	translation = p.translation;
	rotation = p.rotation;
	linearMomentum = p.linearMomentum;
	angularMomentum = p.angularMomentum;

	linearVelocity = p.linearMomentum;
	inertiaTensorInverse = p.inertiaTensorInverse;
	inertiaTensorObj = p.inertiaTensorObj;
	angularVelocity = p.angularVelocity;
	force = p.force;
	torque = p.torque;

	totalMass = p.totalMass;
	vertices = p.vertices;

	return *this;
}

void Physics::UpdateByForce(float dt, glm::vec3 _force, glm::vec3 _torque)
{
	force = _force;
	torque = _torque;

	linearMomentum += dt * force;
	angularMomentum += dt * torque;

	linearVelocity = linearMomentum / totalMass;
	angularMomentum = inertiaTensorInverse * torque;

	translation += dt * linearVelocity;
	rotation += dt * (Tilde(angularVelocity) * rotation);

	// Update inertia tensor
	glm::mat3 newInertiaTensorObj(0.f);
	centerOfMass = rotation * centerOfMass + translation;
	const size_t vertexSize = vertices.size();
	for (size_t i = 0; i < vertexSize; ++i)
	{
		glm::vec3& vertex = vertices[i];
		const float mass = masses[i];

		vertex = rotation * vertex + translation;

		glm::vec3 ri = vertex - centerOfMass;

		inertiaTensorObj[0][0] += mass * (ri.y * ri.y + ri.z * ri.z);
		inertiaTensorObj[1][1] += mass * (ri.x * ri.x + ri.z * ri.z);
		inertiaTensorObj[2][2] += mass * (ri.x * ri.x + ri.y * ri.y);

		inertiaTensorObj[0][1] -= mass * ri.x * ri.y;

		inertiaTensorObj[0][2] -= mass * ri.x * ri.z;

		inertiaTensorObj[1][2] -= mass * ri.y * ri.z;
	}
	inertiaTensorObj[1][0] = inertiaTensorObj[0][1];
	inertiaTensorObj[2][0] = inertiaTensorObj[0][2];
	inertiaTensorObj[2][1] = inertiaTensorObj[1][2];

	inertiaTensorInverse = rotation * glm::inverse(inertiaTensorObj) * glm::transpose(rotation);
}

Change Physics::Derivative()
{
	return Change(linearVelocity, angularVelocity, force, torque);
}

glm::mat3 Physics::Tilde(glm::vec3 v)
{
	glm::mat3 result(0.f);
	result[0][1] = -v.z;
	result[0][2] = v.y;
	result[1][0] = v.z;
	result[1][2] = -v.x;
	result[2][0] = -v.y;
	result[2][1] = v.x;

	return result;
}

Change::Change()
	: linearVelocity(glm::vec3(0.f)), angularVelocity(glm::vec3(0.f)), force(glm::vec3(0.f)), torque(glm::vec3(0.f))
{
}

Change::Change(glm::vec3 linearVelocity, glm::vec3 angularVelocity, glm::vec3 force, glm::vec3 torque)
	: linearVelocity(linearVelocity), angularVelocity(angularVelocity), force(force), torque(torque)
{
}

Change::~Change()
{
}

Change::Change(const Change& c)
	: linearVelocity(c.linearVelocity), angularVelocity(c.angularVelocity), force(c.force), torque(c.torque)
{
}

Change::Change(Change&& c)
	: linearVelocity(c.linearVelocity), angularVelocity(c.angularVelocity), force(c.force), torque(c.torque)
{
}

Change& Change::operator=(const Change& c)
{
	linearVelocity = c.linearVelocity;
	angularVelocity = c.angularVelocity;
	force = c.force;
	torque = c.torque;

	return *this;
}

Change& Change::operator=(Change&& c)
{
	linearVelocity = c.linearVelocity;
	angularVelocity = c.angularVelocity;
	force = c.force;
	torque = c.torque;

	return *this;
}
