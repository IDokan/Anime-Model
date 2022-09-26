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

AS1Scene::AS1Scene(int width, int height)
	:Scene(width, height),
	angleOfRotate(0), vertexNormalFlag(false), faceNormalFlag(false),
	oldX(0.f), oldY(0.f), cameraMovementOffset(0.004f), clearColor(0.4f, 0.4f, 0.4f), skeletonVQSBlockNames(nullptr), skeletonVQSBlockNameSize(-1)
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
	centerObjMesh = new ObjectMesh();

 	sphericalViewPoint = Point(1.f, 0.f, 3.14f);
	cartesianViewVector = Vector(0.f, 0.f, -1.f);
	cartesianRightVector = Vector(1.f);
	cartesianUpVector = Vector(0.f, 1.f, 0.f);


	eyePoint = Point(0.f, 0.f, 5.f);
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
}

AS1Scene::~AS1Scene()
{
}

int AS1Scene::Init()
{
#ifndef _DEBUG
	MeshGenerator::GenerateSphereMesh(*sphereMesh, 0.05f, 16);
	MeshGenerator::GenerateOrbitMesh(*orbitMesh, 1.f, 32);

	myReader->ReadObjFile("../Common/Meshes/models/quad.obj", floorMesh, true);
	// model = new Model("../Common/Meshes/models/Bomber.bin");
#endif

	centerMesh->LoadBinFile("../Common/Meshes/models/Tad.bin");
	CreateSkeletonVQSBlockNames(centerMesh->GetSkeleton().size());

	AddMembersToGUI();

	LoadAllShaders();

	InitGraphics();

	SetupCamera();

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

	//
	// mainModelShader = new AssimpShader(programID);
}

int AS1Scene::preRender(float dt)
{
	glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.f);

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glm::vec3 scaleVector = glm::vec3(1.f);
	const float displacementToPi = glm::pi<float>() / 180.f;
	//modelMatrix = 
	//	glm::rotate(180.f * displacementToPi, glm::vec3(0.f, 1.f, 0.f)) *
	//	glm::scale(scaleVector) * model->CalcAdjustBoundingBoxMatrix();
	centerMatrix =
		glm::rotate(180.f * displacementToPi, glm::vec3(0.f, 1.f, 0.f)) *
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


	centerObjMesh->SetShader(programID);
	centerObjMesh->PrepareDrawing();

	centerObjMesh->SendUniformFloatMatrix4("objToWorld", &centerMatrix[0][0]);
	centerObjMesh->SendUniformFloatMatrix4("worldToNDC", &worldToNDC[0][0]);
	glm::vec3 red(1.f, 0.f, 0.f);
	centerObjMesh->SendUniformFloat3("diffuseColor", &red.x);
	centerObjMesh->SendUniformFloat3("camera", &cameraP.x);

	centerObjMesh->Draw(centerMesh->getIndexBufferSize());


	glDisable(GL_DEPTH_TEST);
	skeletonLines->PrepareDrawing();

	glm::vec3 lineColor;
	lineColor.r = 1.f;
	lineColor.g = 1.f;
	lineColor.b = 1.f;
	const std::vector<Bone>& centerMeshSkeleton = centerMesh->GetSkeleton();
	const int skeletonCount = static_cast<int>(centerMeshSkeleton.size());
	std::vector<glm::mat3> quaternionMatrices(skeletonCount);
	std::vector<glm::vec3> translations(skeletonCount);
	std::vector<float> scalers(skeletonCount);
	for (int i = 0; i < skeletonCount; i++)
	{
		Vqs toModel = centerMeshSkeleton[i].GetToModelFromBone();
		quaternionMatrices[i] = ConvertToMatrix3(toModel.q);
		translations[i] = toModel.v;
		scalers[i] = toModel.s;
	}
	// Since VQS has three parameters, block property count should be multiplied by 3.
	const GLsizei blockPropertyCount = skeletonCount * 3;
	skeletonLines->SendUniformBlockFloatVQS("Block", blockPropertyCount, skeletonVQSBlockNames, quaternionMatrices.data(), translations.data(), scalers.data());
	skeletonLines->SendUniformFloat3("lineColor", reinterpret_cast<float*>(&lineColor));
	skeletonLines->SendUniformFloatMatrix4("objToWorld", &centerMatrix[0][0]);
	skeletonLines->SendUniformFloatMatrix4("worldToNDC", &worldToNDC[0][0]);
	skeletonLines->Draw(centerMesh->GetBoneCountForDisplay());

	glEnable(GL_DEPTH_TEST);

	////////////////////////////////////////////////////////////////////////////////////// Draw ends\

	//mainModelShader->Use();
	//mainModelShader->SendUniformFloatMatrix4("objToWorld", &modelMatrix[0][0]);
	//mainModelShader->SendUniformFloatMatrix4("worldToNDC", &worldToNDC[0][0]);
	//glm::vec3 red(1.f, 0.f, 0.f);
	//mainModelShader->SendUniformFloat3("diffuseColor", &red.x);
	//
	//model->Draw(programID);
	
	DrawDebuggingObjects();

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


	MyImGUI::ClearImGUI();

	textureManager.Clear();

	DestroySkeletonVQSBlockNames();

	delete sphereMesh;
	delete normalMesh;
	delete faceNormalMesh;
	delete spheres;
	delete sphereOrbit;
	delete orbitMesh;

	delete floorObjMesh;
	delete centerObjMesh;

	delete myReader;

	delete skeletonLines;
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
	textureManager.AddTexture(
		"../Common/ppms/metal_roof_diff_512x512.ppm",
		"diffuseTexture", Texture::TextureType::PPM);
	textureManager.AddTexture(
		"../Common/ppms/metal_roof_spec_512x512.ppm",
		"specularTexture", Texture::TextureType::PPM);


	std::vector<glm::vec2> uvs;
	uvs.push_back(glm::vec2(0.f, 0.f));
	uvs.push_back(glm::vec2(1.f, 0.f));
	uvs.push_back(glm::vec2(1.f, 1.f));
	uvs.push_back(glm::vec2(0.f, 1.f));
	floorObjMesh->SetShader(programID);
	floorObjMesh->Init(floorMesh->getVertexCount(), floorMesh->getVertexBuffer(), floorMesh->getVertexNormals(), reinterpret_cast<GLfloat*>(uvs.data()),
		floorMesh->getIndexBufferSize(), floorMesh->getIndexBuffer());

	centerObjMesh->SetShader(programID);
	centerObjMesh->Init(centerMesh->getVertexCount(), centerMesh->getVertexBuffer(), centerMesh->getVertexNormals(), centerMesh->getVertexUVs(),
		centerMesh->getIndexBufferSize(), centerMesh->getIndexBuffer());

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

void AS1Scene::AddMembersToGUI()
{
	MyImGUI::SetNormalDisplayReferences(&vertexNormalFlag, &faceNormalFlag);
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
	if (vertexNormalFlag)
	{
		DrawVertexNormals();
	}

	if (faceNormalFlag)
	{
		DrawFaceNormals();
	}
}

void AS1Scene::PrepareSkeletons()
{
	skeletonLines->SetShader(normalUniformProgramID);
	skeletonLines->Init(centerMesh->GetBoneCountForDisplay(), centerMesh->GetBonesForDisplay());
}

void AS1Scene::CreateSkeletonVQSBlockNames(const size_t size)
{
	skeletonVQSBlockNameSize = static_cast<GLsizei>(size * 3);
	skeletonVQSBlockNames = new GLchar * [skeletonVQSBlockNameSize];
	constexpr int MAX_STRING_SIZE = 40;

	for (int i = 0; i < skeletonVQSBlockNameSize; i++)
	{
		skeletonVQSBlockNames[i] = new GLchar[MAX_STRING_SIZE];
	}

	for (int i = 0; i < skeletonVQSBlockNameSize; i++)
	{
		int blockIndex = i / 3;
		std::string result;
		switch(i % 3)
		{
		case 0:
			result = std::string(std::string("Block.item[") + std::to_string(blockIndex) + std::string("].toModelFromBone"));
			strcpy(skeletonVQSBlockNames[i], result.c_str());
			break;
		case 1:
			result = std::string(std::string("Block.item[") + std::to_string(blockIndex) + std::string("].v")).c_str();
			strcpy(skeletonVQSBlockNames[i], result.c_str());
			break;
		case 2:
			result = std::string(std::string("Block.item[") + std::to_string(blockIndex) + std::string("].s")).c_str();
			strcpy(skeletonVQSBlockNames[i], result.c_str());
			break;
		default:
			break;
		}
	}
}

void AS1Scene::DestroySkeletonVQSBlockNames()
{
	if (skeletonVQSBlockNameSize < 0)
	{
		return;
	}
	for (size_t i = 0; i < skeletonVQSBlockNameSize; i++)
	{
		delete[] skeletonVQSBlockNames[i];
	}
	delete[] skeletonVQSBlockNames;
}
