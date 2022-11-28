#include "Camera.h"

using namespace glm;

Camera::Camera(TransformComponent *transform, Display *display) :
    transform{ transform },
    fieldOfView{ 80 },
    nearPlane{ 0.1f },
    farPlane{ 100.0f },
    m_display{display}
{
}

mat4 Camera::viewMatrix()
{
    return lookAt(transform->position, transform->position + transform->forward(), transform->up());
}

glm::mat4 Camera::projectionMatrix()
{
    return glm::perspective(radians(fieldOfView), static_cast<float>(m_display->width()) / m_display->height(), nearPlane, farPlane);
}
