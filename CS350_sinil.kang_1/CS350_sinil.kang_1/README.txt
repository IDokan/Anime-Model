/* Start Header -------------------------------------------------------
Copyright (C) FALL2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: README.txt
Purpose: describe overview of my application
Language: English; none of computer language is used in here.
Platform: Compiler version: Compiler does not need for README, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: Project #1: Hierarchical Modeling and Animation
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 10/9/2022
End Header --------------------------------------------------------*/

A. Algorithms I used
	I used a cubic polynomial interpolation to form a path.
	I used Adaptive approach to construct an arc length table.
	I used binary search for inverse arc length function.
	I used Sine interpolation for distance-time function.

B. How to compile and run the executable
	Open CS300 Project.sln file.
	If build mode is Debug, change build mode to Release at the top left. It is located at left of Local Windows Debugger which has green triangle icon. (Press Debug and choose Release mode)
	Press Ctrl + F5 to compile and run it.

C. Code comments
	Assignment1/as1Scene.cpp&.h - contains scene codes
		InitPath function has initializing a path by given control points
		BuildTable function constructs arc length table.
		InverseArcLength function contains binary search for finding u given s.
		DistanceByTime function has a Sine interpolation code.
		
	Common/Meshes/binFileSources/binFileStructs.cpp&.h - contains VQS, Quaternion codes.
	Common/Meshes/Mesh.cpp&.h - has how to load model file. 
		Especially LoadBinFile(const std::string& path), ParseBinFile(FileObject* pFile), ReadMesh(FileObject* pFile), ReadSkeleton(FileObject* pFile), ReadAnimation(FileObject* pFile), ReadVqs(FileObject* pFile, Vqs& vqs)
	Common/shaders/SkinShader.vert&.frag, normalUniformShader.vert&.frag - contains skinned mesh and skeleton shader codes
	
	Common/Meshes/Mesh.cpp - Mesh::GetAnimatinoTransform function contains hierarchical transformation implementation.

D. How to move Camera and GUI description
WASD to move camera.
During move, press left shift to move faster.
Press right mouse button to look around.
Press E to move up.
Press Q to move down.

Basic Information section shows FPS, and velocity.
Skeleton section allows you to control toggle displaying skeleton
Animation section help you stop animation and display specific time of the animation.
Model section give you control to change complex model to simple model