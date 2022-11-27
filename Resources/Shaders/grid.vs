#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 projection;
uniform mat4 view;

out vec3 nearPoint;
out vec3 farPoint;
out mat4 projectionMatrix;
out mat4 viewMatrix;

vec3 UnprojectPoint(float x, float y, float z, mat4 view, mat4 projection) {
    mat4 viewInv = inverse(view);
    mat4 projInv = inverse(projection);
    vec4 unprojectedPoint =  viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main()
{
    nearPoint = UnprojectPoint(aPos.x, aPos.y, 0.0, view, projection).xyz;
    farPoint = UnprojectPoint(aPos.x, aPos.y, 1.0, view, projection).xyz;
    projectionMatrix = projection;
    viewMatrix = view;

    gl_Position = vec4(aPos, 1.0f);
}
