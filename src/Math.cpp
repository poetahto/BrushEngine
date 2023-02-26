#include "Math.h"

using namespace glm;

vec3 Math::moveTowards(vec3 current, vec3 target, float maxDelta)
{
    vec3 toVector = target - current;
    float lengthSqr = sqrMagnitude(toVector);

    if (lengthSqr == 0 || (maxDelta >= 0 && lengthSqr <= maxDelta * maxDelta))
        return target;

    float length = static_cast<float>(sqrt(lengthSqr));
    float t = maxDelta / length;
    toVector *= t;
    return current + toVector;
}

float Math::clamp(float value, float min, float max)
{
    if (value > max)
        return max;

    if (value < min)
        return min;

    return value;
}

float Math::wrap(float current, float min, float max)
{
    if (min > max)
        return wrap(current, max, min);

    return (current >= 0 ? min : max) + fmodf(current, max - min);
}

float Math::sqrMagnitude(vec3 vector)
{
    return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}

float Math::lerp(float a, float b, float t)
{
    return a * (1.0f - t) + (b * t);
}

vec3 Math::lerp(vec3 a, vec3 b, float t)
{
    return vec3(
        Math::lerp(a.x, b.x, t),
        Math::lerp(a.y, b.y, t),
        Math::lerp(a.z, b.z, t)
    );
}

// Algorithm taken from here: https://lxp32.github.io/docs/a-simple-example-crc32-calculation/
const int Math::Hashing::hashCrc32(const char* string, size_t length)
{
    int crc = 0xFFFFFFFF;

    for (size_t i = 0; i < length; i++) {
        char ch = string[i];
        for (size_t j = 0; j < 8; j++) {
            int b = (ch ^ crc) & 1;
            crc >>= 1;
            if (b) crc = crc ^ 0xEDB88320;
            ch >>= 1;
        }
    }

    return ~crc;
}