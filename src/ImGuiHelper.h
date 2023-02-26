#pragma once

#include <GLFW/glfw3.h>

class ImGuiHelper
{
public:
    void initialize(GLFWwindow *window);
    void shutdown();

    void preUpdate();
    void render();
};