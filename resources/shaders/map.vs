#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec3 FragPos;
out vec2 TexCoord;
out vec2 TexCoord2;

void main()
{
    FragPos = aPos;
    TexCoord = aTex;
    gl_Position = vec4(aPos, 1.0);
}
