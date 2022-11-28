#include "Time.h"
#include <GLFW/glfw3.h>

void Time::update()
{
    m_time = static_cast<float>(glfwGetTime());
    m_deltaTime = m_time - m_lastFrame;
    m_lastFrame = m_time;
}

float Time::time()
{
    return m_time;
}

float Time::deltaTime()
{
    return m_deltaTime;
}