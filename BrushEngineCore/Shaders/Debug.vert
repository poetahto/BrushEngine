#version 330 core

layout (location = 0) in vec3 iPos;

uniform mat4 transform;

void main()
{
   gl_Position = transform * vec4(iPos, 1.0);
}