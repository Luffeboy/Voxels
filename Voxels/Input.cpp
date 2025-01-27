#include "IncludeOpenGL.h"
#include "Input.h"
#include <iostream>
#include "Vector3.h"
#include "Graphics/GameWindow.h"

GameWindow *Input::m_window = nullptr;
bool Input:: m_justPressedKeys[10];
bool Input:: m_justPressedKeyboardKeys[512];
Vector2 Input::m_mousePos = {0,0};
Vector2 Input::m_previousMousePos = {0,0};
bool Input::m_currentCurserLock = false;

void Input::SetWindow(GameWindow* window)
{
	m_window = window;
	glfwSetMouseButtonCallback(m_window->Window(), MousePressedCallBack);
	glfwSetKeyCallback(m_window->Window(), KeyPressedCallBack);
	glfwSetCursorPosCallback(m_window->Window(), MousePositionCallback);
	LockCurser(true);
}
void Input::Reset()
{
	m_previousMousePos.x = m_mousePos.x;
	m_previousMousePos.y = m_mousePos.y;
	//if (m_currentCurserLock)
	//{
	//	Vector2 windowSize = m_window->WindowSize();
	//	double xPos = (windowSize.x - 1) / 2;
	//	double yPos = (windowSize.y - 1) / 2;
	//	glfwSetCursorPos(m_window->Window(), xPos, yPos);
	//	//MousePositionCallback(m_window->Window(), xPos, yPos);
	//}
}
bool Input::GetKey(int key)
{
	return glfwGetKey(m_window->Window(), key);
}
bool Input::GetKeyPressed(int key)
{
	bool b = m_justPressedKeyboardKeys[key];
	m_justPressedKeyboardKeys[key] = false;
	return b;
}
void Input::KeyPressedCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
		m_justPressedKeyboardKeys[key] = true;
	//else m_justPressedKeyboardKeys[key] = false; // might need to be removed -----------------------------------------------
}
void Input::MousePressedCallBack(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
		m_justPressedKeys[button] = true;
	else m_justPressedKeys[button] = false; // might need to be removed -----------------------------------------------
}

void Input::MousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	//std::cout << "Position: (" << xpos << ":" << ypos << ")";
	m_mousePos.x = xpos;
	m_mousePos.y = ypos;
}


bool Input::GetMousePressed(int key)
{
	bool b = m_justPressedKeys[key];
	m_justPressedKeys[key] = false;
	return b;
}
bool Input::GetMouseDown(int key)
{
	return glfwGetMouseButton(m_window->Window(), key);
}

Vector2 Input::MousePosition()
{
	return m_mousePos;
}
//Vector2 Input::MousePositionNormalized()
//{
//	auto windowSize = m_window->WindowSize();
//	return { m_mousePos.x * 2 / (windowSize.x - 1) - 1, m_mousePos.y * 2 / (windowSize.y - 1) - 1 };
//}
Vector2 Input::MousePositionDelta()
{
	return { m_mousePos.x - m_previousMousePos.x, m_mousePos.y - m_previousMousePos.y };
}
void Input::ToggleLockCurser()
{
	LockCurser(!m_currentCurserLock);
}
void Input::LockCurser(bool locked)
{
	m_currentCurserLock = locked;
	glfwSetInputMode(m_window->Window(), GLFW_CURSOR, locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}