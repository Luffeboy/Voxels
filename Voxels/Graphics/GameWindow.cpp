#include "GameWindow.h"
#include <iostream>

GameWindow::GameWindow() : m_window(nullptr), m_WindowWidth(640), m_WindowHeight(480)
{
    m_windowRunning = false;
    m_glfwWindowCreated = false;
    //std::cout << offsetof(((Vertex*)nullptr).rgb);
    //return 0;


    /* Initialize the library */
    if (!glfwInit())
        return;

    /* Create a windowed mode window and its OpenGL context */
    m_window = glfwCreateWindow(m_WindowWidth, m_WindowHeight, "Hello there", NULL, NULL);
    if (!m_window)
    {
        glfwTerminate();
        return;
    }
    m_glfwWindowCreated = true;

    /* Make the window's context current */
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
        std::cout << "Error initializeing glew" << std::endl;

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSetWindowSizeCallback(m_window, windowResizeCallback);

    // verison and stuff
    //std::cout << glGetString(GL_VERSION) << std::endl;
}

GameWindow::~GameWindow()
{
    if (m_glfwWindowCreated)
        glfwTerminate();
}
