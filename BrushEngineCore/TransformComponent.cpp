#include "TransformComponent.h"

using namespace glm;

TransformComponent::TransformComponent() :
    position{ constants::vec3::Zero },
    rotation{ constants::vec3::Zero },
    scale{ constants::vec3::One }
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
    return glm::rotate(rotation, constants::vec3::Forward);
}

vec3 TransformComponent::right()
{
    return glm::rotate(rotation, constants::vec3::Right);
}

vec3 TransformComponent::up()
{
    return glm::rotate(rotation, constants::vec3::Up);
}