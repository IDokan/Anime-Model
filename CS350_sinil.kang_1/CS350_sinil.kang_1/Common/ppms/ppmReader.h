///* Start Header -------------------------------------------------------
//Copyright (C) FALL2021 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: ppmReader.h
//Purpose:	Header file to read ppm files
//Language: GLSL
//Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
//Project: sinil.kang_CS300_1
//Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
//Creation date: 11/5/2021
//End Header --------------------------------------------------------*/

#include <string>

// Return pixel data. Should delete memory after use it!
float* readImage(const char* fname, int& width, int& height);

float* readBMP(const char* filename, int& width, int& height);
