#version 330 core

out vec4 FragColor;

in float vHeight;

void main()
{
    float temp = vHeight / 32.0f;
    FragColor = vec4(temp, temp, temp, 1.0);
}
