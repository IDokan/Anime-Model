/* Start Header -------------------------------------------------------
Copyright (C) FALL2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Input.h
Purpose: Class which manage all inputs
Language: C++
Platform: Windows SDK version: 10.0.19041.0, OS: Windows 10. GPU: NVIDIA GeForce840M. OpenGL Driver version: 10.18.15.4279, 8-24-2015
Project: sinil.kang_CS300_1
Author: Sinil Kang = sinil.kang = Colleague ID: 0052782
Creation date: 9/25/2021
End Header --------------------------------------------------------*/
#pragma once
#include <bitset>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Input
{
public:
	void Init();
	void TriggeredReset();
	void SetKeyboardInput(int key, int action);
	void SetMousePos(GLFWwindow* window, float xPos, float yPos);
	void SetRawMousePos(float xPos, float yPos);
	void SetMouseButtonInput(int button, int action);
	void SetMouseWheel(double x, double y);

	bool IsKeyTriggered(int key);
	bool IsKeyPressed(int key);
	bool IsKeyReleased(int key);

	glm::vec2 GetPresentMousePosition() const noexcept;
	void SetPresentMousePosition(const glm::vec2& mousePosition) noexcept;
	glm::vec2 GetMousePosition() const  noexcept;
	glm::vec2 GetMouseRawPosition() const noexcept;
	double MouseWheelScroll();
	bool IsMouseButtonPressed(int button);
	bool IsMouseButtonTriggered(int button);
	bool IsMouseButtonReleased(int button);
	bool IsMouseDoubleClicked(int button);

private:
	std::bitset<GLFW_KEY_LAST> keyTriggered;
	std::bitset<GLFW_KEY_LAST> keyPressed;
	std::bitset<GLFW_KEY_LAST> keyReleased;

	glm::vec2 presentMousePosition{};
	glm::vec2 mousePosition{};
	glm::vec2 mouseRawPosition{};
	std::bitset<GLFW_MOUSE_BUTTON_LAST> mouseButtonTriggered;
	std::bitset<GLFW_MOUSE_BUTTON_LAST> mouseButtonPressed;
	std::bitset<GLFW_MOUSE_BUTTON_LAST> mouseButtonReleased;
	std::bitset<GLFW_MOUSE_BUTTON_LAST> mouseButtonDoubleClicked;

	double xOffset = 0.0;
	double yOffset = 0.0;
};

extern Input input;