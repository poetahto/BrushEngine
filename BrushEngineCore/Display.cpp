#include <gl/glew.h>

#include <GLFW/glfw3.h>
#include <cassert>

#include "Debug.h"
#include "Display.h"
#include "Input.h"

// todo: store window display settings in config (CVar implementation?)

static void handleError(int error, const char* description)
{
    debug::log("GLFW error %i: %s", error, description);
}

static void handleWindowResize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

static void handleMouseUpdate(GLFWwindow* window, double x, double y)
{
    Input::handleMouseUpdate(x, y);
}

static void handleKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::handleKeyPress(key, scancode, action, mods);
}

bool Display::shouldClose()
{
    return glfwWindowShouldClose(p_window);
}

void Display::swapBuffers()
{
    glfwSwapBuffers(p_window);
}

void GLAPIENTRY
MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    debug::log("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message);
}

void Display::initialize(const char *title, int width, int height, int refreshRate)
{
    m_refreshRate = refreshRate;
    
    if (glfwInit() == GLFW_FALSE)
    {
        const char* description{};
        glfwGetError(&description);
        debug::log("Error initializing display: %s", description);
        assert(false);
    }

    glfwSetErrorCallback(handleError);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_REFRESH_RATE, refreshRate);

    p_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    glfwSetFramebufferSizeCallback(p_window, handleWindowResize);
    glfwSetCursorPosCallback(p_window, handleMouseUpdate);
    glfwSetKeyCallback(p_window, handleKeyPress);
    glfwMakeContextCurrent(p_window);

    glewExperimental = GL_TRUE;
    glewInit();

    glViewport(0, 0, width, height);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    debug::log("Successfully initialized display.");
}

void Display::shutdown()
{
    glfwTerminate();
}

GLFWwindow *Display::window()
{
    return p_window;
}

void Display::setFullscreen(bool isFullscreen)
{
    int x, y, width, height;
    glfwGetWindowPos(p_window, &x, &y);
    glfwGetWindowSize(p_window, &width, &height);

    if (isFullscreen)
        glfwSetWindowMonitor(p_window, glfwGetPrimaryMonitor(), x, y, width, height, m_refreshRate);
    
    else glfwSetWindowMonitor(p_window, nullptr, x, y, width, height, m_refreshRate);
}

void Display::setResolution(int width, int height)
{
    glfwSetWindowSize(p_window, width, height);
}

void Display::setRefreshRate(int refreshRate)
{
    m_refreshRate = refreshRate;
    
    int x, y, width, height;
    glfwGetWindowPos(p_window, &x, &y);
    glfwGetWindowSize(p_window, &width, &height);
    
    if (fullscreen())
        glfwSetWindowMonitor(p_window, glfwGetWindowMonitor(p_window), 0, 0, width, height, refreshRate);

    else glfwSetWindowMonitor(p_window, nullptr, x, y, width, height, refreshRate);
}

void Display::setVSync(bool useVSync)
{
    glfwSwapInterval(useVSync ? 1 : 0);
    m_vSync = useVSync;
}

bool Display::fullscreen()
{
    return glfwGetWindowMonitor(p_window) == glfwGetPrimaryMonitor();
}

int Display::width()
{
    int width, height;
    glfwGetWindowSize(p_window, &width, &height);
    return width;
}

int Display::height()
{
    int width, height;
    glfwGetWindowSize(p_window, &width, &height);
    return height;
}

int Display::refreshRate()
{
    return m_refreshRate;
}

bool Display::vSync()
{
    return m_vSync;
}
