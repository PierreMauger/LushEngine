#version 330 core
out int FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform int id;

void main()
{
    FragColor = id;
}
