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
Creation date: 11/20/2022
End Header --------------------------------------------------------*/

A. Algorithms I used
	I used CCD algorithm.
	I implemnted enforced joint constraints, but it does not produce the accurate end-effector's final location.

B. How to compile and run the executable
	Open CS300 Project.sln file.
	Press Ctrl + F5 to compile and run it.

C. Code comments
	Assignment1/as1Scene.cpp&.h - contain scene codes		
	Common/Meshes/Mesh.cpp&.h - have CCD algorithms. 
		Mesh::CalculateInverseKinematics(glm::vec3 targetPositionInModelSpace, bool enforced, int interpolatedPositionCount)
			is a public callable function to go to the CCD algorithm code. It interpolates mid points, Call helper function to do CCD.
		Mesh::CalculateInverseKinematics(std::vector<glm::mat4>& frame, const int frameSize, const glm::vec3 targetPositionInModelSpace, const glm::vec3 startPosition, bool enforced)
			is a private helper function. It actually contains CCD algorithm codes.
			Details are implemented from class lecture.
		
		- Angle constraints functions
		Mesh::ExportEulerAngleData(glm::mat4 rotationMatrix)
			helper function to export eular angles from the given rotation matrix.
		Mesh::ClampAngles(glm::vec3& eularAngles, size_t manipulatorIndex)
			clamps angles here.
		

D. How to move Camera and GUI description
@@@@@@@@@@@@@@@@@@@@@@@@
Inverse Kinematics section
please check enforced joint constraints to see results enforced version.
interpolated point count determintes how many mid points to go to the final destination.
Please use ball height parameter if you want to adjust height of final destination.
@@@@@@@@@@@@@@@@@@@@@@@@@
WASD to move camera.
During move, press left shift to move faster.
Press right mouse button to look around.
Press E to move up.
Press Q to move down.

Basic Information section shows FPS, and velocity.
Skeleton section allows you to control toggle displaying skeleton
Animation section help you stop animation and display specific time of the animation.

