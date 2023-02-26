#pragma once

#include <GLFW/glfw3.h>

class Display
{
public:
    void initialize(const char *title, int width, int height, int refreshRate);
    void shutdown();

    bool shouldClose();
    void swapBuffers(); // TEMP!!! REMOVE LATER

    void setFullscreen(bool isFullscreen);
    void setResolution(int width, int height);
    void setRefreshRate(int refreshRate);
    void setVSync(bool useVSync);

    bool fullscreen();
    int width();
    int height();
    int refreshRate();
    bool vSync();
    GLFWwindow *window();

private:
    GLFWwindow *p_window{};
    int m_refreshRate{};
    bool m_vSync{};
};