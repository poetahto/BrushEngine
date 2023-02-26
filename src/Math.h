#pragma once

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Math
{
    float lerp(float current, float target, float t);
    float clamp(float value, float min, float max);
    float wrap(float current, float min, float max);

    glm::vec3 moveTowards(glm::vec3 current, glm::vec3 target, float maxDelta);
    glm::vec3 lerp(glm::vec3 current, glm::vec3 target, float t);
    float sqrMagnitude(glm::vec3 vector);

    namespace Hashing
    {
        const int hashCrc32(const char* string, size_t length);
    }
}

namespace Constants
{
    const glm::vec3 one{ 1,  1,  1 };
    const glm::vec3 zero{ 0,  0,  0 };
    const glm::vec3 up{ 0,  1,  0 };
    const glm::vec3 down{ 0, -1,  0 };
    const glm::vec3 right{ 1,  0,  0 };
    const glm::vec3 left{ -1,  0,  0 };
    const glm::vec3 forward{ 0,  0,  1 };
    const glm::vec3 backward{ 0,  0, -1 };
}