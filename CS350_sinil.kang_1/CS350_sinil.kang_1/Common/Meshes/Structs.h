/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Structs.h
Purpose: Structure for physics
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS460
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 12/01/2021
End Header --------------------------------------------------------*/
#include <glm/glm.hpp>
#include <vector>

class Mesh;

struct Change
{
public:
	Change();
	Change(glm::vec3 linearVelocity, glm::vec3 angularVelocity, glm::vec3 force, glm::vec3 torque);
	~Change();
	Change(const Change& c);
	Change(Change&& c);
	Change& operator=(const Change& c);
	Change& operator=(Change&& c);

	glm::vec3 linearVelocity;
	glm::vec3 angularVelocity;
	glm::vec3 force;
	glm::vec3 torque;
};

struct Physics
{
public:
	// Mesh ctor
	Physics(Mesh&& mesh);
	// Point Mass ctor
	Physics(glm::vec3 vertex, float mass = 1.f);
	~Physics();

	Physics(const Physics& p);
	Physics(Physics&& p);
	Physics& operator=(const Physics& p);
	Physics& operator=(Physics&& p);

	// Mesh Update
	void UpdateByForce(float dt, glm::vec3 force, glm::vec3 torque);
	// Point Mass Update
	void UpdateByForce(float dt, glm::vec3 force);
	Change Derivative();


	glm::vec3 centerOfMass;
	glm::vec3 translation;
	glm::mat3 rotation;
	glm::vec3 linearMomentum;
	glm::vec3 angularMomentum;
	glm::vec3 linearVelocity;
	glm::mat3 inertiaTensorInverse;
	glm::mat3 inertiaTensorObj;
	glm::vec3 angularVelocity;

	glm::vec3 force;
	glm::vec3 torque;

	float totalMass;
private:


private:
	std::vector<float> masses;
	std::vector<glm::vec3> vertices;

private:
	glm::mat3 Tilde(glm::vec3 v);
};