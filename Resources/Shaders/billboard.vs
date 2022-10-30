#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec3 billboardPos = vec3(model[3].x, model[3].y, model[3].z);
    vec2 billboardSize = vec2(model[0].x, model[1].y);
    vec3 cameraRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 cameraUp = vec3(view[0][1], view[1][1], view[2][1]);

    vec3 pos = billboardPos
     + cameraRight * aPos.x * billboardSize.x
     + cameraUp * aPos.y * billboardSize.y;

    gl_Position = projection * view * vec4(pos, 1.0f);
    TexCoords = aTexCoords;
}
