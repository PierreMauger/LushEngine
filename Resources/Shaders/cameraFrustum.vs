#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 frustum; // perspective matrix for the frustum

void main()
{
    vec4 pos = vec4(aPos, 1.0);

    pos = view * frustum * pos;
    gl_Position = projection * pos;
}
