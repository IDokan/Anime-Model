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
#include <unordered_set>
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

#include <../Common/Meshes/Structs.h>

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
	oldX(0.f), oldY(0.f), cameraMovementOffset(0.004f), clearColor(0.4f, 0.4f, 0.4f), timer(0.f),
	dampingConstant(4.f), gravityConstant(0.5f), springConstant(4.f),
	resetFlag(false), sectionCount(2)
{
	sphereMesh = new Mesh();
	orbitMesh = new Mesh();
	floorMesh = new Mesh();
	centerMesh = new Mesh(true);

	centerMatrix = glm::mat4(1.f);

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


	eyePoint = Point(0.f, 5.f, 15.f);
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
	MeshGenerator::GenerateSphereMesh(*sphereMesh, 0.05f, 16);
	MeshGenerator::GenerateOrbitMesh(*orbitMesh, 1.f, 32);

	myReader->ReadObjFile("../Common/Meshes/models/quad.obj", floorMesh, true);

	MeshGenerator::GenerateCubeMesh(*centerMesh, static_cast<float>(sectionCount), sectionCount);
	// myReader->ReadObjFile("../Common/Meshes/models/cube.obj", centerMesh, true);
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

	skinShader = LoadShaders("../Common/shaders/SkinShader.vert",
		"../Common/shaders/SkinShader.frag");
}

int AS1Scene::preRender(float dt)
{
	timer += dt;

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

	if (resetFlag == true)
	{
		centerPhysics = initPhysics;

		resetFlag = false;
	}
	UpdatePhysics(dt);

	glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.f);

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// @@ Path control
	glm::vec3 scaleVector = glm::vec3(1.f);
	static const float displacementToPi = glm::pi<float>() / 180.f;



	centerMatrix =
		glm::translate(glm::vec3(0.f, 0.f, 0.f)) *
		glm::scale(scaleVector) * centerMesh->calcAdjustBoundingBoxMatrix();

	// @@ Path control

	floorMatrix = glm::translate(glm::vec3(0.f, -2.f, 0.f)) * glm::rotate(glm::half_pi<float>(), glm::vec3(1.f, 0.f, 0.f)) * glm::scale(glm::vec3(10.f, 10.f, 1.f)) * floorMesh->calcAdjustBoundingBoxMatrix();

	return 0;
}

int AS1Scene::Render(float dt)
{

	//floorObjMesh->SetShader(programID);
	//floorObjMesh->PrepareDrawing();

	//floorObjMesh->SendUniformFloatMatrix4("objToWorld", &floorMatrix[0][0]);
	//floorObjMesh->SendUniformFloatMatrix4("worldToNDC", &worldToNDC[0][0]);
	Point cameraP = camera.Eye();
	//glm::vec3 blue(0.f, 0.f, 1.f);
	//floorObjMesh->SendUniformFloat3("diffuseColor", &blue.x);
	//floorObjMesh->SendUniformFloat3("camera", &cameraP.x);



	//floorObjMesh->Draw(floorMesh->getIndexBufferSize());

	centerObjMesh->PrepareDrawing();

	centerObjMesh->SendUniformFloatMatrix4("objToWorld", &centerMatrix[0][0]);
	centerObjMesh->SendUniformFloatMatrix4("worldToNDC", &worldToNDC[0][0]);
	glm::vec3 red(1.f, 0.f, 0.f);
	centerObjMesh->SendUniformFloat3("diffuseColor", &red.x);
	centerObjMesh->SendUniformFloat3("camera", &cameraP.x);

	centerObjMesh->Draw(centerMesh->getIndexBufferSize());


	glDisable(GL_DEPTH_TEST);
	//skeleton->PrepareDrawing();


	//glm::vec3 lineColor;
	//lineColor.r = 1.f;
	//lineColor.g = 1.f;
	//lineColor.b = 1.f;
	//skeleton->SendUniformFloat3("lineColor", reinterpret_cast<float*>(&lineColor));
	//skeleton->SendUniformFloatMatrix4("objToWorld", &matrix[0][0]);
	//skeleton->SendUniformFloatMatrix4("worldToNDC", &worldToNDC[0][0]);
	//if (showSkeleton)
	//{
	//	skeleton->Draw(mesh->GetBoneCountForDisplay());
	//}

	glEnable(GL_DEPTH_TEST);

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

	centerObjMesh->SetShader(programID);
	// @@ TODO: Figure out that may I update obj mesh everytime?
	const unsigned int vertexCount = centerMesh->getVertexCount();
	GLfloat* vertices = centerMesh->getVertexBuffer();
	GLuint* indices = centerMesh->getIndexBuffer();
	centerObjMesh->Init(vertexCount, vertices, centerMesh->getVertexNormals(), centerMesh->getVertexUVs(),
		centerMesh->GetBoneIDs(), centerMesh->GetBoneWeights(), centerMesh->getIndexBufferSize(), indices);
	
	// Initialize physics
	{
		for (unsigned int i = 0; i < vertexCount; i++)
		{
			centerPhysics.push_back(Physics(glm::vec3(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]), 3.f));
		}

		const unsigned int triangleCount = centerMesh->getTriangleCount();
		//InitializeRelatedVertexIndexFocusOnFaces(vertexCount, triangleCount, indices);
		InitializeRelatedVertexIndexFocusOnUserDefined(vertexCount);
	
		InitializeSpringLength();

		initPhysics = centerPhysics;
	}

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

void AS1Scene::GetPreviousAndNextIndices(int i, int size, int& previous, int& next)
{
	previous = ((i - 1) < 0) ? 0 : (i - 1);
	next = ((i + 1) >= size) ? size - 1 : i + 1;
}

void AS1Scene::AddMembersToGUI()
{
	MyImGUI::SetRigidBodyReferences(&resetFlag, &gravityConstant, &springConstant, &dampingConstant);
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

void AS1Scene::UpdatePhysics(float dt)
{
	// Deformable - object
	
	const size_t physicsSize = centerPhysics.size();
	std::vector<glm::vec3> force(physicsSize);
	for (size_t i = 0; i < physicsSize - 2; i++)
	{
		const std::vector<unsigned int>& subjectVertices = subjectVertexIndices[i];
		
		glm::vec3 mainCOM = centerPhysics[i].centerOfMass;
		glm::vec3 mainVelocity = centerPhysics[i].linearVelocity;

		const size_t subjectVertexSize = subjectVertices.size();
		for (size_t subjectIndex = 0; subjectIndex < subjectVertexSize; ++subjectIndex)
		{
			glm::vec3 diffVector = mainCOM - centerPhysics[subjectVertices[subjectIndex]].centerOfMass;
			const float length = glm::length(diffVector);
			// spring force
			force[i] += springConstant * diffVector / length * (initSpringLength[i][subjectIndex] - length);

			//// Self collision
			//if (length <= 0.01f)
			//{
			//	std::cout << length << std::endl;
			//}

			// damping force
			force[i] += dampingConstant * (centerPhysics[subjectVertices[subjectIndex]].linearVelocity - mainVelocity);
		}

		// gravity force
		force[i] += centerPhysics[i].totalMass * gravityConstant * glm::vec3(0.f, -1.f, 0.f);
	}

	// std::cout << "x: " << force[5].x << ", y: " << force[5].y << ", z: " << force[5].z << std::endl;

	for (size_t i = 0; i < physicsSize; i++)
	{
		centerPhysics[i].UpdateByForce(dt, force[i]);
		centerMesh->SetVertex(i, centerPhysics[i].centerOfMass);
	}

	centerObjMesh->Init(centerMesh->getVertexBufferSize(), centerMesh->getVertexBuffer(), centerMesh->getVertexNormals(), centerMesh->getVertexUVs(),
		centerMesh->GetBoneIDs(), centerMesh->GetBoneWeights(), centerMesh->getIndexBufferSize(), centerMesh->getIndexBuffer());
}

void AS1Scene::InitializeRelatedVertexIndexFocusOnFaces(unsigned int vertexCount, unsigned int faceCount, unsigned int* indices)
{
	std::vector<std::unordered_set<unsigned int>> indexSetList(vertexCount);
	for (unsigned int i = 0; i < faceCount; i++)
	{
		unsigned int id0 = indices[i * 3];
		unsigned int id1 = indices[i * 3 + 1];
		unsigned int id2 = indices[i * 3 + 2];

		indexSetList[id0].insert(id1);
		indexSetList[id0].insert(id2);
		indexSetList[id1].insert(id0);
		indexSetList[id1].insert(id2);
		indexSetList[id2].insert(id0);
		indexSetList[id2].insert(id1);
	}

	subjectVertexIndices.clear();
	for (const std::unordered_set<unsigned int>& set : indexSetList)
	{
		subjectVertexIndices.push_back(std::vector<unsigned int>(set.begin(), set.end()));
	}
}

void AS1Scene::InitializeRelatedVertexIndexFocusOnUserDefined(int vertexCount)
{
	subjectVertexIndices.clear();
	subjectVertexIndices.resize(vertexCount);
	const int vertexPerEdge = sectionCount + 1;
	const int vertexPerEdgeSquared = vertexPerEdge * vertexPerEdge;
	for (int i = 0; i < vertexCount; i++)
	{
		std::vector<int> candidates{ i - vertexPerEdge, i + vertexPerEdge, i - 1, i + 1, i + vertexPerEdgeSquared, i - vertexPerEdgeSquared,
			i + 1 - vertexPerEdge + vertexPerEdgeSquared, i - 1 - vertexPerEdge + vertexPerEdgeSquared, i - 1 + vertexPerEdge + vertexPerEdgeSquared, i + 1 + vertexPerEdge + vertexPerEdgeSquared,
			i + 1 - vertexPerEdge - vertexPerEdgeSquared, i - 1 - vertexPerEdge - vertexPerEdgeSquared, i - 1 + vertexPerEdge - vertexPerEdgeSquared, i + 1 + vertexPerEdge - vertexPerEdgeSquared };
		//int back = i - vertexPerEdge;
		//int front = i + vertexPerEdge;
		//int left = i - 1;
		//int right = i + 1;
		//int top = i + vertexPerEdgeSquared;
		//int bottom = i - vertexPerEdgeSquared;
		//int TBR = i + 1 - vertexPerEdge + vertexPerEdgeSquared;
		//int TBL = i - 1 - vertexPerEdge + vertexPerEdgeSquared;
		//int TFL = i - 1 + vertexPerEdge + vertexPerEdgeSquared;
		//int TFR = i + 1 + vertexPerEdge + vertexPerEdgeSquared;
		//int BBR = i + 1 - vertexPerEdge - vertexPerEdgeSquared;
		//int BBL = i - 1 - vertexPerEdge - vertexPerEdgeSquared;
		//int BFL = i - 1 + vertexPerEdge - vertexPerEdgeSquared;
		//int BFR = i + 1 + vertexPerEdge - vertexPerEdgeSquared;

		for (int candidate : candidates)
		{
			if (candidate >= 0 && candidate < vertexCount)
			{
				subjectVertexIndices[i].push_back(candidate);
			}
		}
	}
}

void AS1Scene::InitializeSpringLength()
{
	const size_t vertexSize = subjectVertexIndices.size();
	initSpringLength.resize(vertexSize);

	for (size_t vertexIndex = 0; vertexIndex < vertexSize; ++vertexIndex)
	{
		const size_t subjectSize = subjectVertexIndices[vertexIndex].size();
		initSpringLength[vertexIndex].resize(subjectSize);
		for (size_t subjectIndex = 0; subjectIndex < subjectSize; ++subjectIndex)
		{
			const glm::vec3 mainVertex = centerPhysics[vertexIndex].centerOfMass;
			const glm::vec3 subjectVertex = centerPhysics[subjectVertexIndices[vertexIndex][subjectIndex]].centerOfMass;
			initSpringLength[vertexIndex][subjectIndex] = glm::length(mainVertex - subjectVertex);
		}
	}
}
