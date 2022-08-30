///* Start Header -------------------------------------------------------
//Copyright (C) FALL2021 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: CheckRenderedTextureByFBO.frag
//Purpose: shader which draw rendered texture by FBO to confirm.
//Language: GLSL
//Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
//Project: sinil.kang_CS300_1
//Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
//Creation date: 11/29/2021
//End Header --------------------------------------------------------*/

#version 410 core

out vec4 color;

in vec2 uv;

uniform sampler2D tex;

void main()
{
	color = texture(tex, uv);
}