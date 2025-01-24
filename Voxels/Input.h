#pragma once
#include <set>

class GLFWwindow;

class Input
{
private:
	static GLFWwindow* m_window;
	static bool m_justPressedKeys[];
	static bool m_justPressedKeyboardKeys[];
	static void MousePressedCallBack(GLFWwindow* window, int button, int action, int mods);
	static void KeyPressedCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
public:
	static void SetWindow(GLFWwindow* window);
	/// <summary>
	/// use GLFW_KEY_<>
	/// </summary>
	/// <param name="key"></param>
	/// <returns></returns>
	static bool GetKey(int key);
	/// <summary>
	/// use GLFW_KEY_<>
	/// </summary>
	/// <param name="key"></param>
	/// <returns></returns>
	static bool GetKeyPressed(int key);
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