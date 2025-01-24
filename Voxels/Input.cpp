#include "IncludeOpenGL.h"
#include "Input.h"
#include <iostream>

GLFWwindow* Input::m_window = nullptr;
bool Input:: m_justPressedKeys[10];
bool Input:: m_justPressedKeyboardKeys[512];

void Input::SetWindow(GLFWwindow* window)
{
	m_window = window;
	glfwSetMouseButtonCallback(m_window, MousePressedCallBack);
	glfwSetKeyCallback(m_window, KeyPressedCallBack);
}
bool Input::GetKey(int key)
{
	return glfwGetKey(m_window, key);
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


bool Input::GetMousePressed(int key)
{
	bool b = m_justPressedKeys[key];
	m_justPressedKeys[key] = false;
	return b;
}
bool Input::GetMouseDown(int key)
{
	return glfwGetMouseButton(m_window, key);
}