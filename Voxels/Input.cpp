#include "IncludeOpenGL.h"
#include "Input.h"
#include <iostream>

GLFWwindow* Input::m_window = nullptr;
bool Input:: m_justPressedKeys[10];

void Input::SetWindow(GLFWwindow* window)
{
	m_window = window;
	glfwSetMouseButtonCallback(m_window, MousePressedCallBack);
}
bool Input::GetKey(int key)
{
	return glfwGetKey(m_window, key);
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