#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

namespace math
{
    float lerp(float current, float target, float t);
    float clamp(float value, float min, float max);
    float wrap(float current, float min, float max);

    glm::vec3 moveTowards(glm::vec3 current, glm::vec3 target, float maxDelta);
    glm::vec3 lerp(glm::vec3 current, glm::vec3 target, float t);
    float sqrMagnitude(glm::vec3 vector);

    namespace hashing
    {
        const int hashCrc32(const char* string, size_t length);
    }
}

namespace constants
{
    namespace vec3
    {
        const glm::vec3 One{ 1,  1,  1 };
        const glm::vec3 Zero{ 0,  0,  0 };
        const glm::vec3 Up{ 0,  1,  0 };
        const glm::vec3 Down{ 0, -1,  0 };
        const glm::vec3 Right{ 1,  0,  0 };
        const glm::vec3 Left{ -1,  0,  0 };
        const glm::vec3 Forward{ 0,  0,  1 };
        const glm::vec3 Backward{ 0,  0, -1 };
    }
}