#pragma once

#include "Display.h"
#include <glm/glm.hpp>
#include <unordered_map>

class Input
{
public:
    void initialize(GLFWwindow *window);
    void update();

    // Mouse Settings
    glm::vec2 mouseDelta();
    glm::vec2 mousePosition();
    void setCursorVisible(bool isVisible);
    void setRawInput(bool useRawInput);
    bool rawInput();
    bool cursorVisible();

    // Keyboard Settings
    float axis(int positive, int negative);
    bool keyHeld(int key);
    bool keyDown(int key);
    bool keyUp(int key);

    // Event Handlers
    static void handleMouseUpdate(double x, double y);
    static void handleKeyPress(int key, int scancode, int action, int mods);

private:
    struct InputState
    {
        bool down;
        bool up;
    };

    GLFWwindow *p_window;
    
    glm::vec2 m_mouseDelta;
    bool m_rawInput;
    bool m_cursorVisible;

    static glm::vec2 s_mousePosition;
    static std::unordered_map<int, InputState> s_inputMap;
};