/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: as1Scene.cpp
Purpose: main scene source code of this assignment
Language: C++
Platform: Compiler version: -1, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS300_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 9/18/2021
End Header --------------------------------------------------------*/
#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <math.h>
#include <../Assignment1/as1Scene.h>
#include <../Common/Meshes/MyObjReader.h>
#include <../Common/Meshes/Mesh.h>
#include <../Common/shader.hpp>
#include <../Common/Meshes/MeshGenerator.h>

#include <../Common/Meshes/MyMeshes/LineMesh.h>
#include <../Common/Meshes/MyMeshes/ObjectMesh.h>

#include <../Common/Cameras/Projection.h>
#include <../Common/Cameras/Affine.h>
#include <../Common/Input.h>

// include ImGUI
#include <../myGUI/myImGUI.h>

#include <../Common/Meshes/models/Model.h>
#include <../Common/shaders/AssimpShader.h>

#include <../Common/Meshes/binFileSources/binFileStructs.h>

#include <../Common/Meshes/MyMeshes/BoneObjectMesh.h>
#include <iostream>
#include <queue>

AS1Scene::AS1Scene(int width, int height)
	:Scene(width, height),
	angleOfRotate(0), showSkeleton(false),
	oldX(0.f), oldY(0.f), cameraMovementOffset(0.004f), clearColor(0.4f, 0.4f, 0.4f),
	animationMat4BlockNames(nullptr), animationMat4BlockNameSize(-1), timer(0.f), playAnimation(true), velocity(0.f), startPosition(), ballPosition()
{
	sphereMesh = new Mesh();
	orbitMesh = new Mesh();
	floorMesh = new Mesh();
	centerMesh = new Mesh(true);

	centerMatrix = glm::mat4(1.f);
	// modelMatrix = glm::mat4(1.f);

	normalMesh = new LineMesh();
	faceNormalMesh = new LineMesh();
	spheres = new ObjectMesh();
	sphereOrbit = new LineMesh();
	floorObjMesh = new ObjectMesh();
	centerObjMesh = new BoneObjectMesh();

	sphericalViewPoint = Point(1.f, 0.f, 3.14f);
	cartesianViewVector = Vector(0.f, 0.f, -1.f);
	cartesianRightVector = Vector(1.f);
	cartesianUpVector = Vector(0.f, 1.f, 0.f);


	eyePoint = Point(0.f, 5.f,15.f);
	targetPoint = Point(0.f, 0.f, 0.f);
	fov = static_cast<float>(M_PI_2);
	width = _windowWidth;
	height = _windowHeight;
	aspect = static_cast<float>(width) / height;
	nearDistance = 0.01f;
	farDistance = 200.f;
	relativeUp = Vector(0.f, 1.f, 0.f);

	myReader = new MyObjReader();

	skeletonLines = new LineMesh();

	pathLine = new LineMesh();

	startPosition = glm::vec3(8.f, -4.9f, -2.f);
	ballPosition = glm::vec3(-8.f, -4.9f, 0.f);
}

AS1Scene::~AS1Scene()
{
}

int AS1Scene::Init()
{
	MeshGenerator::GenerateSphereMesh(*sphereMesh, 0.05f, 16);
	MeshGenerator::GenerateOrbitMesh(*orbitMesh, 1.f, 32);

	myReader->ReadObjFile("../Common/Meshes/models/quad.obj", floorMesh, true);





	centerMesh->LoadBinFile("../Common/Meshes/models/Joe.bin");
	CreateAnimationMat4BlockNames(animationMat4BlockNames, animationMat4BlockNameSize, centerMesh->GetSkeleton().size());

	AddMembersToGUI();

	LoadAllShaders();

	InitGraphics();

	SetupCamera();

	InitPath();
	BuildTable();
	// Assume whole time is 8 seconds,
		// 1 seconds to finish a track.

	return Scene::Init();
}

void AS1Scene::LoadAllShaders()
{
	programID = LoadShaders("../Common/shaders/As1DiffuseShader.vert",
		"../Common/shaders/As1DiffuseShader.frag");

	normalDisplayProgramID = LoadShaders("../Common/shaders/normalDisplayShader.vert",
		"../Common/shaders/normalDisplayShader.frag");

	normalUniformProgramID = LoadShaders("../Common/shaders/normalUniformShader.vert",
		"../Common/shaders/normalUniformShader.frag");

	skinShader = LoadShaders("../Common/shaders/SkinShader.vert",
		"../Common/shaders/SkinShader.frag");
}

int AS1Scene::preRender(float dt)
{
	if (playAnimation)
	{

		if (timer < 8.f)
		{
			timer += dt;
		}
	}
	centerMesh->UpdateAnimationTimer(dt, velocity);

	glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.f);

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glm::vec3 scaleVector = glm::vec3(1.f);
	static const float displacementToPi = glm::pi<float>() / 180.f;
	const float u = InverseArcLength(DistanceByTime(timer));
	glm::vec3 position = BezierCurve(u);

	const float deltaU = 0.01f;

	static const glm::vec3 globalY(0.f, 1.f, 0.f);
	glm::vec3 roll = glm::normalize(ballPosition - startPosition);
	glm::vec3 pitch = glm::cross(globalY, roll);
	glm::vec3 yaw = glm::cross(roll, pitch);
	glm::vec4 last = glm::vec4(0.f, 0.f, 0.f, 1.f);
	glm::mat4 pathControl = glm::mat4(glm::vec4(pitch, 0.f), glm::vec4(yaw, 0.f), glm::vec4(roll, 0.f), last);

	centerMatrix =
		glm::translate(position) *
		glm::translate(glm::vec3(0.f, 2.f, 0.f)) * 
		pathControl *
		glm::rotate(270.f * displacementToPi, glm::vec3(1.f, 0.f, 0.f)) *
		glm::scale(scaleVector);
	floorMatrix = glm::translate(glm::vec3(0.f, -5.f, 0.f)) * glm::rotate(glm::half_pi<float>(), glm::vec3(1.f, 0.f, 0.f)) * glm::scale(glm::vec3(10.f, 10.f, 1.f)) * floorMesh->calcAdjustBoundingBoxMatrix();

	UpdateCamera();

	Matrix wTN = (CameraToNDC(camera) * WorldToCamera(camera));
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			worldToNDC[i][j] = wTN[i][j];
		}
	}
	worldToNDC = glm::transpose(worldToNDC);

	return 0;
}

int AS1Scene::Render(float dt)
{

	floorObjMesh->SetShader(programID);
	floorObjMesh->PrepareDrawing();

	floorObjMesh->SendUniformFloatMatrix4("objToWorld", &floorMatrix[0][0]);
	floorObjMesh->SendUniformFloatMatrix4("worldToNDC", &worldToNDC[0][0]);
	Point cameraP = camera.Eye();
	glm::vec3 blue(0.f, 0.f, 1.f);
	floorObjMesh->SendUniformFloat3("diffuseColor", &blue.x);
	floorObjMesh->SendUniformFloat3("camera", &cameraP.x);



	floorObjMesh->Draw(floorMesh->getIndexBufferSize());

	DrawModelAndAnimation(centerMesh, centerObjMesh, skeletonLines, cameraP, animationMat4BlockNames, centerMatrix, dt);

	spheres->PrepareDrawing();
	glm::mat4 sphereMatrix = glm::translate(ballPosition) * glm::scale(glm::vec3(0.1f, 0.1f, 0.1f)) * sphereMesh->calcAdjustBoundingBoxMatrix();
	glm::vec3 white(1.f, 1.f, 1.f);
	spheres->SendUniformFloatMatrix4("worldToNDC", &worldToNDC[0][0]);
	spheres->SendUniformFloatMatrix4("objToWorld", &sphereMatrix[0][0]);
	spheres->SendUniformFloat3("diffuseColor", &white[0]);
	spheres->Draw(sphereMesh->getIndexBufferSize());


	const unsigned int EEIndex = 19;
	Vqs toEE = centerMesh->GetAnimationTransform(EEIndex);
	glm::mat4 objToEEToWorld = centerMatrix * glm::translate(toEE.v) * ConvertToMatrix4(toEE.q) * glm::scale(glm::vec3(toEE.s));

	glm::vec3 EEcolor(0.f, 1.f, 0.f);
	spheres->PrepareDrawing();
	spheres->SendUniformFloatMatrix4("objToWorld", &objToEEToWorld[0][0]);
	spheres->SendUniformFloat3("diffuseColor", &EEcolor[0]);
	spheres->Draw(sphereMesh->getIndexBufferSize());

	DrawPath();

	////////////////////////////////////////////////////////////////////////////////////// Draw ends

	return 0;
}

int AS1Scene::postRender(float dt)
{
	constexpr float rotatePerFrame = 0.015f;
	angleOfRotate += rotatePerFrame;
	return 0;
}

void AS1Scene::CleanUp()
{
	glDeleteProgram(programID);
	glDeleteProgram(normalDisplayProgramID);
	glDeleteProgram(normalUniformProgramID);
	glDeleteProgram(skinShader);


	MyImGUI::ClearImGUI();

	textureManager.Clear();

	DestroyAnimationMat4BlockNames(animationMat4BlockNames, animationMat4BlockNameSize);

	delete sphereMesh;
	delete normalMesh;
	delete faceNormalMesh;
	delete spheres;
	delete sphereOrbit;
	delete orbitMesh;

	delete centerMesh;

	delete floorObjMesh;
	delete centerObjMesh;

	delete myReader;

	delete skeletonLines;

	delete pathLine;
}

void AS1Scene::SetupNanoGUI(GLFWwindow* window)
{
	MyImGUI::InitImGUI(window);
}

void AS1Scene::UpdateGUI()
{
	MyImGUI::UpdateImGUI();
}

void AS1Scene::InitGraphics()
{
	//textureManager.AddTexture(
	//	"../Common/ppms/metal_roof_diff_512x512.ppm",
	//	"diffuseTexture", Texture::TextureType::PPM);
	//textureManager.AddTexture(
	//	"../Common/ppms/metal_roof_spec_512x512.ppm",
	//	"specularTexture", Texture::TextureType::PPM);


	std::vector<glm::vec2> uvs;
	uvs.push_back(glm::vec2(0.f, 0.f));
	uvs.push_back(glm::vec2(1.f, 0.f));
	uvs.push_back(glm::vec2(1.f, 1.f));
	uvs.push_back(glm::vec2(0.f, 1.f));
	floorObjMesh->SetShader(programID);
	floorObjMesh->Init(floorMesh->getVertexCount(), floorMesh->getVertexBuffer(), floorMesh->getVertexNormals(), reinterpret_cast<GLfloat*>(uvs.data()),
		floorMesh->getIndexBufferSize(), floorMesh->getIndexBuffer());

	centerObjMesh->SetShader(skinShader);
	centerObjMesh->Init(centerMesh->getVertexCount(), centerMesh->getVertexBuffer(), centerMesh->getVertexNormals(), centerMesh->getVertexUVs(),
		centerMesh->GetBoneIDs(), centerMesh->GetBoneWeights(), centerMesh->getIndexBufferSize(), centerMesh->getIndexBuffer());

	// normal inits
	normalMesh->SetShader(normalDisplayProgramID);
	normalMesh->Init(centerMesh->getVertexNormalCount(), centerMesh->getVertexNormalsForDisplay());

	// @@@@ Disable temporarily
	// // Face Inits
	// faceNormalMesh->SetShader(normalDisplayProgramID);
	// faceNormalMesh->Init(model->GetFaceNormalCount(), model->GetFaceNormalsForDisplay());


	PrepareSkeletons();

	spheres->SetShader(programID);
	spheres->Init(sphereMesh->getVertexCount(), sphereMesh->getVertexBuffer(), sphereMesh->getVertexNormals(), sphereMesh->getVertexUVs(),
		sphereMesh->getIndexBufferSize(), sphereMesh->getIndexBuffer());

	sphereOrbit->SetShader(normalDisplayProgramID);
	sphereOrbit->Init(orbitMesh->getVertexBufferSize(), orbitMesh->getVertexBuffer());
}

void AS1Scene::InitPath()
{
	InitControlPoints();

	constexpr int lineSegmentCount = 80;
	path.resize(lineSegmentCount);

	const int halfCount = lineSegmentCount / 2;
	for (int i = 0; i < halfCount; i++)
	{
		glm::vec3 interpolatedPoint = BezierCurve(i * (1.f / halfCount));
		path[i * 2] = interpolatedPoint;
		glm::vec3 interpolatedNextPoint = BezierCurve(((i + 1)) * (1.f / halfCount));
		path[(i * 2) + 1] = interpolatedNextPoint;
	}

	pathLine->SetShader(normalDisplayProgramID);
	pathLine->Init(lineSegmentCount, reinterpret_cast<GLfloat*>(path.data()));
}

void AS1Scene::InitControlPoints()
{
	constexpr int pointSize = 8;
	controlPoints.resize(pointSize);
	//controlPoints[0] = startPosition;
	//for (int i = 0; i < pointSize - 2; i++)
	//{
	//	controlPoints[i + 1] = static_cast<float>(i) / (pointSize - 3) * ballPosition + (static_cast<float>(pointSize - 3 - i) / (pointSize - 3) * startPosition);
	//}
	//controlPoints[pointSize - 1] = ballPosition;
	for (int i = 0; i < pointSize; i++)
	{
		controlPoints[i] = static_cast<float>(i) / (pointSize - 1) * ballPosition + (static_cast<float>(pointSize - 1 - i) / (pointSize - 1) * startPosition);
	}

	interpolatedPointsForCurve.resize(pointSize);
	for (int i = 0; i < pointSize; i++)
	{
		int previousIndex;
		int nextIndex;
		GetPreviousAndNextIndices(i, pointSize, previousIndex, nextIndex);

		glm::vec3 offset = (controlPoints[nextIndex] - controlPoints[previousIndex]) / static_cast<float>(pointSize);
		interpolatedPointsForCurve[i].first = controlPoints[i] - offset;
		interpolatedPointsForCurve[i].second = controlPoints[i] + offset;
	}


}

void AS1Scene::DrawPath()
{
	glm::mat4 pathToWorld = glm::scale(glm::vec3(0.05f, 0.05f, 0.05f)) * sphereMesh->calcAdjustBoundingBoxMatrix();
	glm::vec3 red = glm::vec3(1.f, 0.f, 0.f);
	pathLine->PrepareDrawing();
	pathLine->SendUniformFloatMatrix4("worldToNDC", &worldToNDC[0][0]);
	pathLine->SendUniformFloatMatrix4("objToWorld", &pathToWorld[0][0]);
	pathLine->SendUniformFloat3("lineColor", &red[0]);
	pathLine->Draw(static_cast<int>(path.size()));
}

void AS1Scene::DrawControlPoints()
{

	glm::vec3 white = glm::vec3(1.f, 1.f, 1.f);
	for (size_t i = 0; i < controlPoints.size(); i++)
	{
		spheres->PrepareDrawing();
		glm::mat4 sphereMatrix = glm::translate(controlPoints[i]) * glm::scale(glm::vec3(0.1f, 0.1f, 0.1f)) * sphereMesh->calcAdjustBoundingBoxMatrix();

		spheres->SendUniformFloatMatrix4("worldToNDC", &worldToNDC[0][0]);
		spheres->SendUniformFloatMatrix4("objToWorld", &sphereMatrix[0][0]);
		spheres->SendUniformFloat3("diffuseColor", &white[0]);
		spheres->Draw(sphereMesh->getIndexBufferSize());
	}
}

glm::vec3 AS1Scene::BezierCurve(float u)
{
	if (u < 0.f)
	{
		u = 0.f;
	}
	else if (u > 1.f)
	{
		u = 1.f;
	}
	const int controlPointSize = static_cast<int>(controlPoints.size());
	float step = 1.f / (controlPointSize - 1);
	int startControlPointIndex = (static_cast<int>(floor(u / step)));
	if (startControlPointIndex >= controlPointSize)
	{
		startControlPointIndex = controlPointSize - 1;
	}
	int nextIndex = ((startControlPointIndex + 1) <= controlPointSize) ? startControlPointIndex + 1 : controlPointSize - 1;

	float u0 = step * startControlPointIndex;
	float u1 = step * (startControlPointIndex + 1);
	u = (u - u0) / (u1 - u0);


	glm::vec3 p0 = controlPoints[startControlPointIndex];
	glm::vec3 p1 = interpolatedPointsForCurve[startControlPointIndex].second;
	glm::vec3 p2 = interpolatedPointsForCurve[nextIndex].first;
	glm::vec3 p3 = controlPoints[nextIndex];

	if (p0 == p3)
	{
		return p0;
	}

	return (-p0 + 3.f * p1 - 3.f * p2 + p3) * (u * u * u) + (3.f * p0 - 6.f * p1 + 3.f * p2) * (u * u) + (-3.f * p0 + 3.f * p1) * u + p0;
}

void AS1Scene::GetPreviousAndNextIndices(int i, int size, int& previous, int& next)
{
	previous = ((i - 1) < 0) ? 0 : (i - 1);
	next = ((i + 1) >= size) ? size - 1 : i+1;
}

void AS1Scene::BuildTable()
{
	// Specify temporary classes to make it easy.
	typedef std::pair<float, float> SegList;

	class myComparator
	{
	public:
		int operator() (const SegList& s1, const SegList& s2)
		{
			return s1.first > s2.first;
		}
	};

	SegList startSegList = SegList(0.f, 1.f);
	std::priority_queue<SegList, std::vector<SegList>, myComparator> segLists;
	segLists.push(startSegList);

	const float distanceThreshold = 0.1f;
	const float parameterThreshold = 0.01f;

	while (!segLists.empty())
	{
		const SegList current = segLists.top();
		segLists.pop();
		const float ua = current.first;
		const float ub = current.second;

		const float um = (ua + ub) / 2.f;
		const glm::vec3 uaLoc = BezierCurve(ua);
		const glm::vec3 ubLoc = BezierCurve(ub);
		const glm::vec3 umLoc = BezierCurve(um);
		const float A = glm::distance(uaLoc, umLoc);
		const float B = glm::distance(umLoc, ubLoc);
		const float C = glm::distance(uaLoc, ubLoc);
		const float d = abs(A + B - C);

		if (d > distanceThreshold || abs(ub - ua) > parameterThreshold)
		{
			segLists.push(SegList(ua, um));
			segLists.push(SegList(um, ub));
		}
		else
		{
			arcLengthTable.insert(std::pair(um, A + arcLengthTable[ua]));
			arcLengthTable.insert(std::pair(ub, B + arcLengthTable[um]));
		}
	}

	// Code to normalize elements in arc Length table.
	const float maxElement = arcLengthTable[1.f];
	for (auto& t : arcLengthTable)
	{
		t.second = t.second / maxElement;
	}
}

float AS1Scene::InverseArcLength(float s)
{
	if (s <= 0.f)
	{
		return 0.f;
	}
	else if (s >= 1.f)
	{
		return 1.f;
	}

	float ua = 0.f;
	float ub = 1.f;

	float um = 0.f;
	float sm = 0.f;

	// Get 2 * delta arc length
	float tmp = (++arcLengthTable.begin())->second;
	float tmp2 = arcLengthTable.begin()->second;
	const float arcLengthEpsilon = 2.f * (tmp - tmp2);
	do
	{
		um = (ua + ub) / 2.f;
		sm = arcLengthTable[um];
		if (sm == 0.f && um > 0.f)
		{
			arcLengthTable.erase(um);
			break;
		}

		if (abs(sm - s) <= arcLengthEpsilon)
		{
			break;
		}

		if (sm < s)
		{
			ua = um;
		}
		else
		{
			ub = um;
		}

	} while (true);

	if (sm > s)
	{
		const float si = arcLengthTable[ua];
		return ua + (um - ua) * (s - si) / (sm - si);
	}
	else
	{
		const float si1 = arcLengthTable[ub];
		return um + (ub - um) * (s - sm) / (si1 - sm);
	}
}

// @@ TODO: Test distance by time function
float AS1Scene::DistanceByTime(float t)
{
	//const static float t1 = centerMesh->GetAnimationDuration() / 8.f, t7 = centerMesh->GetAnimationDuration() * 7.f / 8.f, t8 = centerMesh->GetAnimationDuration();
	const static float t1 = 2.f, t7 = 6.f, t8 = 8.f;


	if (t > t8)
	{
		return t8;
	}

	float result = 0.f;
	if (t < t1)
	{
		result = EaseIn(t, t1);
	}
	else if (t > t7)
	{
		result = EaseOut(t, t7, t8, t1);
	}
	else
	{
		result = Linear(t, t1);
	}

	static const float max = EaseOut(t8, t7, t8, t1);
	result /= max;

	return result;
}

float AS1Scene::VelocityByTime(float t)
{
	const static float t1 = 2.f, t7 = 6.f, t8 = 8.f;


	if (t > t8)
	{
		return 0.f;
	}

	float result = 0.f;
	if (t < t1)
	{
		result = DerivativeEaseIn(t, t1);
	}
	else if (t > t7)
	{
		result = DerivativeEaseOut(t, t7, t8);
	}
	else
	{
		result = DerivativeLinear();
	}

	return result;
}

float AS1Scene::EaseIn(float t, float maxT)
{
	const float pi = glm::pi<float>();
	return maxT * sin(pi / 2 * (t - maxT) / maxT) + (maxT);
}

float AS1Scene::DerivativeEaseIn(float t, float maxT)
{
	const float pi = glm::pi<float>();
	return pi * cos(pi*(t - maxT)/(2*maxT)) / 2.f;
}

float AS1Scene::Linear(float t, float t1)
{
	const float pi = glm::pi<float>();
	return pi / 2.f * (t - t1) + t1;
}

float AS1Scene::DerivativeLinear()
{
	return glm::pi<float>() / 2.f;
}

float AS1Scene::EaseOut(float t, float minT, float maxT, float t1)
{
	const float pi = glm::pi<float>();
	return (maxT - minT) * sin(pi / 2 * (t - minT) / (maxT - minT)) + ((minT - t1) * pi / 2) + (t1);
}

float AS1Scene::DerivativeEaseOut(float t, float minT, float maxT)
{
	const float pi = glm::pi<float>();
	return pi * cos(pi * (t - minT) / (2 * (maxT - minT))) / 2.f;
}

void AS1Scene::AddMembersToGUI()
{
	MyImGUI::SetNormalDisplayReferences(&showSkeleton);
	MyImGUI::SetAnimationReferences(&playAnimation, &timer, centerMesh->GetAnimationDuration());
	MyImGUI::SetDisplayReferences(&velocity);
}

void AS1Scene::DrawVertexNormals()
{
	normalMesh->PrepareDrawing();

	glm::vec3 lineColor;
	lineColor.r = 1.f;
	lineColor.g = 1.f;
	lineColor.b = 1.f;
	normalMesh->SendUniformFloat3("lineColor", reinterpret_cast<float*>(&lineColor));
	normalMesh->SendUniformFloatMatrix4("objToWorld", &centerMatrix[0][0]);
	normalMesh->SendUniformFloatMatrix4("worldToNDC", &worldToNDC[0][0]);
	normalMesh->Draw(centerMesh->getVertexNormalCount());
}

void AS1Scene::DrawFaceNormals()
{
	// @@@@@ Disable temporarily
	// faceNormalMesh->PrepareDrawing();
	// 
	// glm::vec3 lineColor;
	// lineColor.r = 1.f;
	// lineColor.g = 1.f;
	// lineColor.b = 0.f;
	// faceNormalMesh->SendUniformFloat3("lineColor", &lineColor[0]);
	// faceNormalMesh->SendUniformFloatMatrix4("objToWorld", &modelMatrix[0][0]);
	// faceNormalMesh->SendUniformFloatMatrix4("worldToNDC", &worldToNDC[0][0]);
	// faceNormalMesh->Draw(model->GetFaceNormalCount());
}

void AS1Scene::SetupCamera()
{
	camera = Camera(eyePoint, targetPoint - eyePoint, relativeUp, fov, aspect, nearDistance, farDistance);
}

void AS1Scene::MoveViewOfCamera(int x, int y)
{

	sphericalViewPoint.z -= (x - oldX) * cameraMovementOffset;
	oldX = static_cast<float>(x);
	sphericalViewPoint.y += (y - oldY) * cameraMovementOffset;
	if (sphericalViewPoint.y >= static_cast<float>(M_PI_2))
	{
		sphericalViewPoint.y = static_cast<float>(M_PI_2);
	}
	else if (sphericalViewPoint.y <= -static_cast<float>(M_PI_2))
	{
		sphericalViewPoint.y = -static_cast<float>(M_PI_2);
	}
	oldY = static_cast<float>(y);

	cartesianViewVector.x = cos(sphericalViewPoint.y) * sin(sphericalViewPoint.z);
	cartesianViewVector.y = sin(sphericalViewPoint.y);
	cartesianViewVector.z = cos(sphericalViewPoint.y) * cos(sphericalViewPoint.z);

	cartesianViewVector = normalize(cartesianViewVector);
	cartesianRightVector = cross(cartesianViewVector, relativeUp);
	cartesianUpVector = cross(cartesianRightVector, cartesianViewVector);

	targetPoint = eyePoint + cartesianViewVector;

	camera = Camera(eyePoint, targetPoint - eyePoint, relativeUp, fov, aspect, nearDistance, farDistance);
}

void AS1Scene::MovePositionOfCamera(float speed, short moveDirection)
{
	Hcoord displacementOfEye;

	if (moveDirection & FORWARD)
	{
		displacementOfEye = displacementOfEye + (speed * cartesianViewVector);
	}
	if (moveDirection & BACKWARD)
	{
		displacementOfEye = displacementOfEye + (-speed * cartesianViewVector);
	}
	if (moveDirection & RIGHTWARD)
	{
		displacementOfEye = displacementOfEye + (speed * cartesianRightVector);
	}
	if (moveDirection & LEFTWARD)
	{
		displacementOfEye = displacementOfEye + (-speed * cartesianRightVector);
	}
	if (moveDirection & UPWARD)
	{
		displacementOfEye = displacementOfEye + (speed * cartesianUpVector);
	}
	if (moveDirection & DOWNWARD)
	{
		displacementOfEye = displacementOfEye + (-speed * cartesianUpVector);
	}

	eyePoint.x += displacementOfEye.x;
	eyePoint.y += displacementOfEye.y;
	eyePoint.z += displacementOfEye.z;

	targetPoint = eyePoint + cartesianViewVector;


	camera = Camera(eyePoint, targetPoint - eyePoint, relativeUp, fov, aspect, nearDistance, farDistance);
}

void AS1Scene::UpdateCamera()
{
	float speed = 0.01f;

	if (input.IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
	{
		speed = 0.03f;
	}

	short moveDirection = 0;

	if (input.IsKeyPressed(GLFW_KEY_W))
	{
		moveDirection |= FORWARD;
	}
	if (input.IsKeyPressed(GLFW_KEY_S))
	{
		moveDirection |= BACKWARD;
	}
	if (input.IsKeyPressed(GLFW_KEY_A))
	{
		moveDirection |= LEFTWARD;
	}
	if (input.IsKeyPressed(GLFW_KEY_D))
	{
		moveDirection |= RIGHTWARD;
	}
	if (input.IsKeyPressed(GLFW_KEY_E))
	{
		moveDirection |= UPWARD;
	}
	if (input.IsKeyPressed(GLFW_KEY_Q))
	{
		moveDirection |= DOWNWARD;
	}

	MovePositionOfCamera(speed, moveDirection);

	if (input.IsMouseButtonTriggered(GLFW_MOUSE_BUTTON_RIGHT))
	{
		glm::vec2 mousePos = input.GetMousePosition();
		oldX = mousePos.x;
		oldY = mousePos.y;
	}
	else if (input.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
	{
		glm::vec2 mousePos = input.GetMousePosition();
		MoveViewOfCamera(static_cast<int>(mousePos.x), static_cast<int>(mousePos.y));
	}
}

void AS1Scene::DrawDebuggingObjects()
{
	// if (vertexNormalFlag)
	{
		DrawVertexNormals();
	}

	// if (faceNormalFlag)
	{
		DrawFaceNormals();
	}
}

void AS1Scene::PrepareSkeletons()
{
	skeletonLines->SetShader(normalUniformProgramID);
	skeletonLines->Init(centerMesh->GetBoneCountForDisplay(), centerMesh->GetBonesForDisplay());
}

void AS1Scene::CreateAnimationMat4BlockNames(GLchar**& names, GLsizei& nameSizeRef, const size_t size)
{
	nameSizeRef = static_cast<GLsizei>(size);
	names = new GLchar * [nameSizeRef];
	constexpr int MAX_STRING_SIZE = 40;

	for (int i = 0; i < nameSizeRef; i++)
	{
		names[i] = new GLchar[MAX_STRING_SIZE];
	}

	for (int i = 0; i < nameSizeRef; i++)
	{
		std::string result;
		result = std::string(std::string("Block.item[") + std::to_string(i) + std::string("].toModelFromBone"));
		strcpy(names[i], result.c_str());
	}
}

void AS1Scene::DestroyAnimationMat4BlockNames(GLchar**& names, GLsizei& nameSizeRef)
{
	if (nameSizeRef < 0)
	{
		return;
	}
	for (size_t i = 0; i < nameSizeRef; i++)
	{
		delete[] names[i];
	}
	delete[] names;
}

void AS1Scene::DrawModelAndAnimation(Mesh* mesh, BoneObjectMesh* objMesh, LineMesh* skeleton, Point& p, GLchar**& blockNames, glm::mat4& matrix, float dt)
{
	velocity = VelocityByTime(timer);

	std::vector<Vqs> toBoneFromModel;
	mesh->GetToBoneFromModel(toBoneFromModel);
	std::vector<Vqs> transformsData;
	mesh->GetAnimationTransform(transformsData);
	const int skeletonCount = static_cast<int>(transformsData.size());
	std::vector<glm::mat4> animationMat4Data(skeletonCount);
	for (int i = 0; i < skeletonCount; i++)
	{
		Vqs toModel = transformsData[i] * toBoneFromModel[i];
		toModel.q = toModel.q / magnitude(toModel.q);
		animationMat4Data[i] = glm::translate(toModel.v) * ConvertToMatrix4(toModel.q) * glm::scale(glm::vec3(toModel.s));
	}

	objMesh->PrepareDrawing();

	objMesh->SendUniformFloatMatrix4("objToWorld", &matrix[0][0]);
	objMesh->SendUniformFloatMatrix4("worldToNDC", &worldToNDC[0][0]);
	objMesh->SendUniformBlockMatrix4("Block", skeletonCount, blockNames, animationMat4Data.data());
	glm::vec3 red(1.f, 0.f, 0.f);
	objMesh->SendUniformFloat3("diffuseColor", &red.x);
	objMesh->SendUniformFloat3("camera", &p.x);

	objMesh->Draw(mesh->getIndexBufferSize());


	glDisable(GL_DEPTH_TEST);
	skeleton->PrepareDrawing();


	glm::vec3 lineColor;
	lineColor.r = 1.f;
	lineColor.g = 1.f;
	lineColor.b = 1.f;
	// skeletonLines->SendUniformBlockMatrix4("Block", skeletonCount, animationMat4BlockNames, animationMat4Data.data());
	skeleton->SendUniformFloat3("lineColor", reinterpret_cast<float*>(&lineColor));
	skeleton->SendUniformFloatMatrix4("objToWorld", &matrix[0][0]);
	skeleton->SendUniformFloatMatrix4("worldToNDC", &worldToNDC[0][0]);
	if (showSkeleton)
	{
		skeleton->Draw(mesh->GetBoneCountForDisplay());
	}

	glEnable(GL_DEPTH_TEST);
}
