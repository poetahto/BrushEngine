#include "TransformComponent.h"

using namespace glm;

TransformComponent::TransformComponent() :
    position{ Constants::zero },
    rotation{ Constants::zero },
    scale{ Constants::one }
{
}

mat4 TransformComponent::transformMatrix()
{
    mat4 model{ 1 };
    model = translate(model, position);
    model *= glm::toMat4(rotation);
    model = glm::scale(model, scale);

    return model;
}

vec3 TransformComponent::forward()
{
    return glm::rotate(rotation, Constants::forward);
}

vec3 TransformComponent::right()
{
    return glm::rotate(rotation, Constants::right);
}

vec3 TransformComponent::up()
{
    return glm::rotate(rotation, Constants::up);
}