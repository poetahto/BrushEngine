#include <assert.h>
#include "Color.h"

Color ColorRgbPercent(float r, float g, float b, float a)
{
    return Color{ r, g, b, a };
}

Color ColorRgbDecimal(float r, float g, float b, float a)
{
    return Color{ r / 255, g / 255, b / 255, a / 255 };
}

Color ColorHsl(float h, float s, float l, float a)
{
    float r, g, b;

    // Early short-circuit if we know it has no color data.
    if (s == 0)
    {
        r = g = b = l;
    }
    else 
    {
        // Adapted from http://en.wikipedia.org/wiki/HSL_color_space

        auto hue2rgb = [](float p, float q, float t)
        {
            if (t < 0) t++;
            if (t > 1) t--;
            if (t < 1.0 / 6.0) return p + (q - p) * 6 * t;
            if (t < 0.5)       return q;
            if (t < 2.0 / 3.0) return p + (q - p) * (2.0f / 3.0f - t) * 6;
            return p;
        };

        float q = l < 0.5 
            ? l * (1 + s) 
            : l + s - l * s;

        float p = 2 * l - q;

        r = hue2rgb(p, q, h + 1.0 / 3.0);
        g = hue2rgb(p, q, h);
        b = hue2rgb(p, q, h - 1.0 / 3.0);
    }

    return Color{ r, g, b, a };
}

Color ColorHex(int hex)
{
    assert(false && "Not implmented yet!");
    return Color{};
}