#shader vertex
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

// the percentage of the screen that the texture will take up (0.0 - 100.0)
uniform vec2 size;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos.x * size.x / 100.0f, aPos.y * size.y / 100.0f, 0.0f, 1.0f);
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D tex;

void main()
{
    vec4 color = texture(tex, TexCoords);
    if (color.a == 0.0f)
        discard;

    FragColor = color;
}
