#pragma once

#include "Types.h"
#include "Math.h"
#include "TransformComponent.h"
#include "Display.h"

struct Camera
{
    Camera(TransformComponent *transform, Display *display);

    TransformComponent *transform;
    
    F32 fieldOfView;
    F32 nearPlane;
    F32 farPlane;

    glm::mat4 viewMatrix();
    glm::mat4 projectionMatrix();

private:
    Display *m_display;
};

