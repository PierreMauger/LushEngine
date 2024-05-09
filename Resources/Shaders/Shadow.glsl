#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}

#shader fragment
#version 330 core

in vec2 TexCoords;

struct Texture {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

uniform Texture tex;
uniform bool hasTexture = false;

void main()
{
    if (hasTexture && texture(tex.diffuse, TexCoords).a == 0.0f)
        discard;
    gl_FragDepth = gl_FragCoord.z;
}
