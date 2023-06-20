#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 5) in ivec4 boneIds;
layout (location = 6) in vec4 weights;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

#define MAX_BONES 100
#define MAX_BONE_INFLUENCE 4

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 totalPosition = vec4(aPos, 1.0f);

    Normal = mat3(transpose(inverse(model))) * aNormal;
    FragPos = vec3(model * totalPosition);
    TexCoords = aTexCoords;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec4 id;

void main()
{
    FragColor = id;
}
