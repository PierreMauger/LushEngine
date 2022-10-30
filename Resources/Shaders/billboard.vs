#version 330 core
layout (location = 0) in vec3 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec3 BillboardPos = vec3(model[3].x, model[3].y, model[3].z);
    vec2 BillboardSize = vec2(model[0].x, model[1].y);
    vec3 CameraRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 CameraUp = vec3(view[0][1], view[1][1], view[2][1]);

    vec3 pos = BillboardPos
     + CameraRight * aTexCoords.x * BillboardSize.x
     + CameraUp * aTexCoords.y * BillboardSize.y;

    gl_Position = projection * view * vec4(pos, 1.0f);

    TexCoords = aTexCoords.xy + vec2(0.5f, 0.5f);
}
