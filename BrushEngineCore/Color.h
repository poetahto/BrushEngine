#pragma once

struct Color
{
    float r{ 0 };
    float g{ 0 };
    float b{ 0 };
    float a{ 1 };
};

Color ColorRgbPercent(float r, float g, float b, float a = 1);
Color ColorRgbDecimal(float r, float g, float b, float a = 1);
Color ColorHsl(float h, float s, float l, float a = 1);
Color ColorHex(int hex);