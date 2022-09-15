#version 410 core

out vec4 FragColor;

in float Height;

void main()
{
    float h = (Height + 16) / 64.0f;
    FragColor = vec4(h, h, h, 1.0f);
}
