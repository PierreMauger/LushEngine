#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in float Height[];

out float vHeight;

void main()
{
    vHeight = Height[0];

    for(int i = 0; i < 3; i++)
    {
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }

    EndPrimitive();

    for(int i = 0; i < 3; i++)
    {
        int j = (i + 1) % 3;

        vec3 edge1 = gl_in[j].gl_Position.xyz - gl_in[i].gl_Position.xyz;
        vec3 edge2 = gl_in[(j + 1) % 3].gl_Position.xyz - gl_in[j].gl_Position.xyz;
        vec3 normal = cross(edge1, edge2);

        if(normal.z < 0.0)
        {
            gl_Position = gl_in[i].gl_Position;
            EmitVertex();

            gl_Position = gl_in[j].gl_Position;
            EmitVertex();

            EndPrimitive();
        }
    }
}
