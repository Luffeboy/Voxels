#pragma once
#include <set>

class GLFWwindow;
class Vector2;
class GameWindow;

class Input
{
private:
	static GameWindow* m_window;
	static bool m_justPressedKeys[];
	static bool m_justPressedKeyboardKeys[];
	static Vector2 m_mousePos;
	static Vector2 m_previousMousePos;
	static bool m_currentCurserLock;
	static void MousePressedCallBack(GLFWwindow* window, int button, int action, int mods);
	static void KeyPressedCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MousePositionCallback(GLFWwindow* window, double xpos, double ypos);
public:
	static void Reset();
	static void SetWindow(GameWindow* window);
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

	static Vector2 MousePosition();
	//static Vector2 MousePositionNormalized();
	static Vector2 MousePositionDelta();
	static void ToggleLockCurser();
	static void LockCurser(bool locked);
};