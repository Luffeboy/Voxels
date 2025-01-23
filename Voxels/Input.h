#pragma once
#include <set>

class GLFWwindow;

class Input
{
private:
	static GLFWwindow* m_window;
	static bool m_justPressedKeys[];
	static void MousePressedCallBack(GLFWwindow* window, int button, int action, int mods);
public:
	static void SetWindow(GLFWwindow* window);
	//static void Reset();
	/// <summary>
	/// use GLFW_KEY_<>
	/// </summary>
	/// <param name="key"></param>
	/// <returns></returns>
	static bool GetKey(int key);
	/// <summary>
	/// if the mouse button was just pressed
	/// </summary>
	/// <param name="key"></param>
	/// <returns></returns>
	static bool GetMousePressed(int key = GLFW_MOUSE_BUTTON_1);
	/// <summary>
	/// If the mouse button is down
	/// </summary>
	/// <param name="key"></param>
	/// <returns></returns>
	static bool GetMouseDown(int key = GLFW_MOUSE_BUTTON_1);
};