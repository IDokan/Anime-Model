///* Start Header -------------------------------------------------------
//Copyright (C) FALL2021 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: ppmReader.cpp
//Purpose:	Source code to read ppm files
//Language: GLSL
//Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
//Project: sinil.kang_CS300_1
//Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
//Creation date: 11/5/2021
//End Header --------------------------------------------------------*/

#define _CRT_SECURE_NO_WARNINGS
#include <../Common/ppms/ppmReader.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>


//ignore the comment in PPM    
void ignoreComment(std::ifstream& f) {
    char buf[1024];
    char t;
    t = f.peek();
    while (t == '\n' || t == '\r') {
        f.get();
        t = f.peek();
    }
    if (t == '#') {
        f.getline(buf, 1023);
    }

}

float* readImage(const char* fname, int& width, int& height) {
    std::ifstream f;
    f.open(fname, std::ifstream::in | std::ifstream::binary);
    if (!f.is_open()) {
        std::cout << "Cannot open the file" << std::endl;
        return nullptr;
    }

    //get type
    ignoreComment(f);
    std::string temp;
    f >> temp;
    if ((!(temp[0] == 'P') && !(temp[0] == 'p')) ||
        (!(temp[1] == '6') && !(temp[1] == '3'))
        ) {
        std::cout << temp << std::endl;
        std::cout << "cannot read this format" << std::endl;
        f.close();
        return nullptr;
    }

    int type = 0;

    if (temp[1] == '3') {
        type = 3;
    }
    if (temp[1] == '6') {
        type = 6;
    }


    //get width,height
    ignoreComment(f);
    f >> width;
    ignoreComment(f);
    f >> height;
    ignoreComment(f);
    int maxValue = 1;
    f >> maxValue;

    if (width < 1 || height < 1) {
        std::cout << "Cannot get the size or gray level" << std::endl;
        f.close();
        return nullptr;
    }

    constexpr unsigned rgb = 3;
    int wholeLength = rgb * width * height;

    float* dataByte = nullptr;
    if (type == 3 || type == 6) {
        dataByte = new float[wholeLength];
        int w;
        if (type == 3)
            w = width;
        if (type == 6)
            w = 3 * width;
        int count = 0; int counth = 0;

        for (int h = 0; h < height; h++)
        {
            for (int w = 0; w < width; w++)
            {
                for (int i = 0; i < rgb; i++)
                {
                    unsigned index = h * width * rgb + w * rgb + i;
                    int data = -1;
                    f >> data;
                    dataByte[index] = static_cast<float>(data) / maxValue;
                }
            }
        }
    }
    f.close();

    return dataByte;
}

float* readBMP(const char* filename, int& width, int& height)
{
    int i;
    FILE* f = fopen(filename, "rb");
    unsigned char info[54];

    // read the 54-byte header
    fread(info, sizeof(unsigned char), 54, f);

    // extract image height and width from header
    width = *(int*)&info[18];
    height = *(int*)&info[22];

    // allocate 3 bytes per pixel
    int size = 3 * width * height;
    unsigned char* data = new unsigned char[size];

    // read the rest of the data at once
    fread(data, sizeof(unsigned char), size, f);
    fclose(f);

    for (i = 0; i < size; i += 3)
    {
        // flip the order of every 3 bytes
        unsigned char tmp = data[i];
        data[i] = data[i + 2];
        data[i + 2] = tmp;
    }

    float* realData = new float[size];
    for (int i = 0; i < size; i++)
    {
        realData[i] = data[i] / 255.f;
    }

    return realData;
}