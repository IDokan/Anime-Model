/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: GLApplication.cpp
Purpose: main function container
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS300_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 9/18/2021
End Header --------------------------------------------------------*/

#include <Windows.h>
// Include standard headers
#include <cstdio>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <iostream>

// Local / project headers
#include "../Common/Scene.h"
#include "shader.hpp"
#include "../Assignment1/as1Scene.h"
#include <../Common/Input.h>

#include "../Common/Utility/Timer.h"

//////////////////////////////////////////////////////////////////////

GLFWwindow *window;
Scene  *scene;

constexpr int windowWidth = 1024;
constexpr int windowHeight = 768;

constexpr int guiWidth = 600;
constexpr int guiHeight = 800;

///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
namespace WindowCallbackFunctions
{
    struct WindowSizePosition
    {
        WindowSizePosition(float width, float height, float x = 0.f, float y = 0.f)
            : width(width), height(height), x(x), y(y)
        {}

        float width;
        float height;
        float x;
        float y;
    };


    WindowSizePosition GetWindowValueByResolution(WindowSizePosition raw)
    {
        float  virtual_width = guiWidth;
        float virtual_height = guiHeight;
        float targetAspectRadio = virtual_width / virtual_height;

        float adjustedWidth = static_cast<float>(raw.width);
        float adjustedHeight = (adjustedWidth / targetAspectRadio + 0.5f);
        if (adjustedHeight > static_cast<float>(raw.height))
        {
            adjustedHeight = static_cast<float>(raw.height);
            adjustedWidth = (adjustedHeight * targetAspectRadio + 0.5f);
        }
        float vp_x = (static_cast<float>(raw.width) / 2.f) - (adjustedWidth / 2.f);
        float vp_y = (static_cast<float>(raw.height) / 2.f) - (adjustedHeight / 2.f);

        return WindowSizePosition{ adjustedWidth, adjustedHeight, vp_x, vp_y };
    }

    void MousePositionCallback(GLFWwindow* _window, double xPos, double yPos)
    {
        glm::ivec2 size;
        glfwGetWindowSize(_window, &size.x, &size.y);

        WindowSizePosition result = GetWindowValueByResolution(WindowSizePosition{ static_cast<float>(size.x), static_cast<float>(size.y) });

        xPos = (xPos - result.x) * guiWidth / result.width;
        yPos = (yPos - result.y) * guiHeight / result.height;

        input.SetMousePos(window, static_cast<float>(xPos), static_cast<float>(yPos));
    }

    void MouseButtonCallback(GLFWwindow*, int button, int action, int)
    {
        input.SetMouseButtonInput(button, action);
    }

    void MouseWheelScroll(GLFWwindow*, double xOffset, double yOffset)
    {
        input.SetMouseWheel(xOffset, yOffset);
    }

    void GUIMousePositionCallback(GLFWwindow* _window, double xPos, double yPos)
    {
        glm::ivec2 size;
        glfwGetWindowSize(_window, &size.x, &size.y);

        WindowSizePosition result = GetWindowValueByResolution(WindowSizePosition{ static_cast<float>(size.x), static_cast<float>(size.y) });

        xPos = (xPos - result.x) * guiWidth / result.width;
        yPos = (yPos - result.y) * guiHeight / result.height;

        input.SetMousePos(_window, static_cast<float>(xPos), static_cast<float>(yPos));
    }

    void GUIMouseButtonCallback(GLFWwindow*, int button, int action, int)
    {
        input.SetMouseButtonInput(button, action);
    }

    void GUIMouseWheelScroll(GLFWwindow*, double xOffset, double yOffset)
    {
        input.SetMouseWheel(xOffset, yOffset);
    }

    void KeyCallback(GLFWwindow*, int key, int, int action, int)
    {
        input.SetKeyboardInput(key, action);
    }
}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
int main()
{
    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    // Setting up OpenGL properties
    glfwWindowHint(GLFW_SAMPLES, 1); // change for anti-aliasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(windowWidth, windowHeight, // window dimensions
                              "Sample 1 - Simple scene (Quad) with Scene Class", // window title
                              nullptr, // which monitor (if full-screen mode)
                              nullptr); // if sharing context with another window
    if (window == nullptr)
    {
        fprintf(stderr,
                "Failed to open GLFW window. Check if your GPU is 4.0 compatible.\n");
        glfwTerminate();
        return -1;
    }

    GLFWwindow* guiWindow = glfwCreateWindow(guiWidth, guiHeight, "MyGUI", nullptr, nullptr);
    if (guiWindow == nullptr)
    {
        fprintf(stderr,
            "Failed to open GLFW window. Check if your GPU is 4.0 compatible.\n");
        glfwTerminate();
        return -1;
    }
    glfwSetCursorPosCallback(guiWindow, WindowCallbackFunctions::GUIMousePositionCallback);
    glfwSetScrollCallback(guiWindow, WindowCallbackFunctions::GUIMouseWheelScroll);
    glfwSetMouseButtonCallback(guiWindow, WindowCallbackFunctions::GUIMouseButtonCallback);
    glfwSetKeyCallback(guiWindow, WindowCallbackFunctions::KeyCallback);
    glfwSetCursorPosCallback(window, WindowCallbackFunctions::MousePositionCallback);
    glfwSetScrollCallback(window, WindowCallbackFunctions::MouseWheelScroll);
    glfwSetMouseButtonCallback(window, WindowCallbackFunctions::MouseButtonCallback);
    glfwSetKeyCallback(window, WindowCallbackFunctions::KeyCallback);
    input.Init();

    // OpenGL resource model - "glfwCreateWindow" creates the necessary data storage for the OpenGL
    // context but does NOT make the created context "current". We MUST make it current with the following
    // call
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = static_cast<GLboolean>(true); // Needed for core profile
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(guiWindow, GLFW_STICKY_KEYS, GL_TRUE);

    // Initialize the scene
    scene = new AS1Scene( windowWidth, windowHeight );

    // Scene::Init encapsulates setting up the geometry and the texture
    // information for the scene
    glfwMakeContextCurrent(window);
    scene->Init();

    glfwMakeContextCurrent(guiWindow);
    scene->SetupNanoGUI(guiWindow);

    do
    {
        Timer* timer = Timer::GetTimer();
        float dt = static_cast<float>(timer->GetDeltaTime());

        glfwMakeContextCurrent(window);

        // Now render the scene
        // Scene::Display method encapsulates pre-, render, and post- rendering operations
        scene->Display(dt);

        // Swap buffers
        glfwSwapBuffers(window);


        glfwMakeContextCurrent(guiWindow);
        scene->UpdateGUI();
        glfwMakeContextCurrent(guiWindow);
        glfwSwapBuffers(guiWindow);

        input.TriggeredReset();

        std::cout << input.GetMousePosition().x << ", " << input.GetMousePosition().y << std::endl;
        
        glfwPollEvents();
    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwGetKey(guiWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 && glfwWindowShouldClose(guiWindow) == 0);
    scene->CleanUp();
    delete scene;

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////