#shader vertex
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

#shader fragment
#version 330 core

out vec4 FragColor;

in float vHeight;

void main()
{
    float temp = vHeight / 32.0f;
    FragColor = vec4(temp, temp, temp, 1.0);
}

#shader geometry
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

#shader tessControl
#version 410 core

layout (vertices = 4) out;

in vec2 v_TexCoord[];
out vec2 tc_TexCoord[];

uniform mat4 model;
uniform mat4 view;

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    tc_TexCoord[gl_InvocationID] = v_TexCoord[gl_InvocationID];

    if (gl_InvocationID == 0) {
        const int MIN_TESS_LEVEL = 4;
        const int MAX_TESS_LEVEL = 64;
        const float MIN_DISTANCE = 20;
        const float MAX_DISTANCE = 400;

        vec4 eyeSpacePos00 = view * model * gl_in[0].gl_Position;
        vec4 eyeSpacePos01 = view * model * gl_in[1].gl_Position;
        vec4 eyeSpacePos10 = view * model * gl_in[2].gl_Position;
        vec4 eyeSpacePos11 = view * model * gl_in[3].gl_Position;

        // "distance" from camera scaled between 0 and 1
        float distance00 = clamp((abs(eyeSpacePos00.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0f, 1.0f);
        float distance01 = clamp((abs(eyeSpacePos01.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0f, 1.0f);
        float distance10 = clamp((abs(eyeSpacePos10.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0f, 1.0f);
        float distance11 = clamp((abs(eyeSpacePos11.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0f, 1.0f);

        float tessLevel0 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance10, distance00));
        float tessLevel1 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance00, distance01));
        float tessLevel2 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance01, distance11));
        float tessLevel3 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance11, distance10));

        gl_TessLevelOuter[0] = tessLevel0;
        gl_TessLevelOuter[1] = tessLevel1;
        gl_TessLevelOuter[2] = tessLevel2;
        gl_TessLevelOuter[3] = tessLevel3;

        gl_TessLevelInner[0] = max(tessLevel1, tessLevel3);
        gl_TessLevelInner[1] = max(tessLevel0, tessLevel2);
    }
}

#shader tessEval
#version 410 core

layout (quads, fractional_odd_spacing, cw) in;

out float Height;
out vec3 Normal;
out vec3 FragPos;

in vec2 tc_TexCoord[];
out vec2 te_TexCoord;

uniform sampler2D heightMap;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec2 t00 = tc_TexCoord[0];
    vec2 t01 = tc_TexCoord[1];
    vec2 t10 = tc_TexCoord[2];
    vec2 t11 = tc_TexCoord[3];

    vec2 t0 = (t01 - t00) * u + t00;
    vec2 t1 = (t11 - t10) * u + t10;

    te_TexCoord = (t1 - t0) * v + t0;
    Height = texture(heightMap, te_TexCoord).y * 32.0f - 16.0f;

    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    vec4 p11 = gl_in[3].gl_Position;

    vec4 uVec = p01 - p00;
    vec4 vVec = p10 - p00;
    vec4 normal = normalize(vec4(cross(vVec.xyz, uVec.xyz), 0.0f));

    vec4 p0 = (p01 - p00) * u + p00;
    vec4 p1 = (p11 - p10) * u + p10;
    vec4 p = (p1 - p0) * v + p0 + normal * Height;
    p.y = Height;

    Normal = vec3(model * p);

    gl_Position = projection * view * model * p;
    FragPos = gl_Position.xyz;
}
