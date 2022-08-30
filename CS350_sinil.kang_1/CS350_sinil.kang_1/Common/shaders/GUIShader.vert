///* Start Header -------------------------------------------------------
//Copyright (C) FALL2021 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: GUIShader.vert
//Purpose: shader for GUI. Currently, display simple quads
//Language: GLSL
//Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
//Project: sinil.kang_CS300_1
//Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
//Creation date: 9/22/2021
//End Header --------------------------------------------------------*/

#version 410 core

uniform mat4  vertexTransform;

layout(location = 0) in vec3    vPosition;
layout(location = 1) in vec3 vertexNormal;


// Interpolating vertex attributes over the rasterizer
out VS_OUT
{
    vec4 vertexPosition;
    vec3 vertexNormal;
    
} vs_out;

void main()
{
    vs_out.vertexPosition = vertexTransform *  vec4( vPosition, 1.0f );
    vs_out.vertexNormal = vertexNormal;
    
    gl_Position = vs_out.vertexPosition;
}
