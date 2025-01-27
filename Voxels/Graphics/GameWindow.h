#pragma once
#include "../IncludeOpenGL.h"
#include "Renderer.h"

struct Camera;

class GameWindow
{
private:
	GLFWwindow* m_window;
	bool m_windowRunning;
	bool m_glfwWindowCreated;
	int m_WindowWidth;
	int m_WindowHeight;
public:
	Renderer m_renderer;
	GameWindow();
	~GameWindow();
	inline void Init(Camera* camera) 
	{ 
		m_renderer.SetCamera(camera);
		m_renderer.LoadShaders();
	}
	inline void SwapBuffer() 
	{
		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}
	inline int WindowIsRunning() { return m_windowRunning; }
	inline bool ShouldClose() { return WindowIsRunning() || glfwWindowShouldClose(m_window); }
	inline GLFWwindow* Window() { return m_window; }
	inline Vector2 WindowSize() { return { (float)m_WindowWidth, (float)m_WindowHeight }; }

private:
	static void windowResizeCallback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}
};