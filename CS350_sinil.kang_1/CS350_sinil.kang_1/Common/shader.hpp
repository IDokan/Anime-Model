/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: shader.hpp
Purpose: Class which compiles shader
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS300_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 9/20/2021
End Header --------------------------------------------------------*/

#ifndef SHADER_HPP
#define SHADER_HPP

enum Primitive_Enum
{
    TriangleStrip = 0,
    Points,
    LineStrip
};

///////////////////
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

///////////////////
GLuint LoadPipeline(const char * vertex_file_path,const char * fragment_file_path, GLuint * programIDs );

// Load shaders where multiple shader files == one complete shader
// Show how code can be reused across shaders
GLuint LoadMultiShaders(const char *vertex_file_path, const char *fragment_file_path,
                        const char *geom_file_path, Primitive_Enum  out_primitive_type );


#endif
