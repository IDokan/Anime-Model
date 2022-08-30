///* Start Header -------------------------------------------------------
//Copyright (C) FALL2021 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: GUIShader.frag
//Purpose: shader for GUI. Currently, display simple quads
//Language: GLSL
//Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
//Project: sinil.kang_CS300_1
//Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
//Creation date: 9/22/2021
//End Header --------------------------------------------------------*/

#version 410 core

uniform vec3 uniformColor;
in VS_OUT
{
    vec4 vertexPosition;
    vec3 vertexNormal;

} fs_in;

// Output data
out vec3 color;


void main()
{
	color = uniformColor;
}
