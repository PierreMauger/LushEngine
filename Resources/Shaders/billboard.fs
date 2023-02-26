#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D tex;

void main()
{
    vec4 color = texture(tex, TexCoords);
    if (color.a < 0.1)
        discard;

    FragColor = color;
}
