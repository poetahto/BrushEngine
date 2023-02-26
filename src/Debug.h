#pragma once

#include <cstdarg>
#include "Math.h"
#include "Color.h"

namespace Debug
{
    void log(const char* format, ...);
    void log(int verbosity, const char* format, ...);
    void logV(const char* format, va_list args);
    void logV(int verbosity, const char* format, va_list args);

    void drawLine(glm::vec3 start, glm::vec3 end, glm::mat4 vp, Color color = {1, 1, 1});
    void drawRay(glm::vec3 start, glm::vec3 direction, glm::mat4 vp, Color color = {1, 1, 1});

    static int Verbosity{ 0 };
}