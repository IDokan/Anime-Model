///* Start Header -------------------------------------------------------
//Copyright (C) FALL2021 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: As2PhongLighting.frag
//Purpose:	This shader implements the vertex-based lighting model. 
//					This is the implementation of the OpenGL compatibility pipeline (version 2.1 or older). 
//					Lighting computations are done in the vertex shader, 
//					and only the final color is interpolated to the fragments. 
//					The only “out?variable from the vertex shader should be the final color of the fragment.
//Language: GLSL
//Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
//Project: sinil.kang_CS300_1
//Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
//Creation date: 10/20/2021
//End Header --------------------------------------------------------*/

#version 410 core

in vec3 colorCalculatedPerVertex;

out vec3 color;

void main()
{
color = colorCalculatedPerVertex;
}