#include "Input.h"
#include "Debug.h"

using namespace glm;

void Input::initialize(GLFWwindow *window)
{
    p_window = window;
}

vec2 Input::mouseDelta()
{
    return m_mouseDelta;
}

vec2 Input::mousePosition()
{
    return s_mousePosition;
}

void Input::setCursorVisible(bool isVisible)
{
    if (isVisible)
        glfwSetInputMode(p_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    else glfwSetInputMode(p_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    m_cursorVisible = isVisible;
}

void Input::setRawInput(bool useRawInput)
{
    if (glfwRawMouseMotionSupported())
    {
        glfwSetInputMode(p_window, GLFW_RAW_MOUSE_MOTION, useRawInput ? GLFW_TRUE : GLFW_FALSE);
        m_rawInput = useRawInput;
    }
    else m_rawInput = false;
}

bool Input::rawInput()
{
    return m_rawInput;
}

bool Input::cursorVisible()
{
    return m_cursorVisible;
}

float Input::axis(int positive, int negative)
{
    float result{ 0 };

    if (keyHeld(positive))
        ++result;

    if (keyHeld(negative))
        --result;

    return result;
}

bool Input::keyHeld(int key)
{
    s_inputMap.try_emplace(key, InputState{});
    return glfwGetKey(p_window, key);
}

bool Input::keyDown(int key)
{
    s_inputMap.try_emplace(key, InputState{});
    return s_inputMap.at(key).down;
}

bool Input::keyUp(int key)
{
    s_inputMap.try_emplace(key, InputState{});
    return s_inputMap.at(key).up;
}

void Input::update()
{
    vec2 oldPosition = s_mousePosition;

    for (auto& iterator : s_inputMap)
    {
        iterator.second.up = false;
        iterator.second.down = false;
    }

    glfwPollEvents();

    m_mouseDelta = s_mousePosition - oldPosition;
}

void Input::handleMouseUpdate(double x, double y)
{
    Debug::log(1, "Mouse update: %f %f", x, y);

    s_mousePosition = vec2{ x, y };
}

void Input::handleKeyPress(int key, int scancode, int action, int mods)
{
    if (action != GLFW_REPEAT)
    {
        Debug::log(1, "Key event: %i", key);

        s_inputMap.try_emplace(key, InputState{});

        s_inputMap.at(key).down = action == GLFW_PRESS;
        s_inputMap.at(key).up = action == GLFW_RELEASE;
    }
}

glm::vec2 Input::s_mousePosition{};
std::unordered_map<int, Input::InputState> Input::s_inputMap{};