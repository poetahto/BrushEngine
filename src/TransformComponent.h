#pragma once

#include "Math.h"

struct TransformComponent
{
    TransformComponent();

    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    glm::mat4 transformMatrix();
    glm::vec3 forward();
    glm::vec3 right();
    glm::vec3 up();
};