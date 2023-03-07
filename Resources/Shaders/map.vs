#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec2 v_TexCoord;

uniform sampler2D heightMap;

void main()
{
    v_TexCoord = aTex;
    vec2 texSize = textureSize(heightMap, 0);
    vec3 pos = aPos;
    pos.x *= texSize.x;
    pos.z *= texSize.y;

    gl_Position = vec4(pos, 1.0);
}
