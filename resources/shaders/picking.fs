#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec4 id;

void main()
{
    FragColor = id;
}
