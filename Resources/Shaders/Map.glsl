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
#version 410 core

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NB_DIR_LIGHTS 2
#define NB_POINT_LIGHTS 4

out vec4 FragColor;

in float Height;
in vec3 Normal;
in vec3 FragPos;
in vec2 te_TexCoord;

uniform vec3 viewPos;
uniform sampler2D heightMap;
uniform mat4 model;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D diffuseTexture2;
uniform sampler2D diffuseTexture3;

uniform int dirLightCount;
uniform DirLight dirLights[NB_DIR_LIGHTS];
uniform int pointLightCount;
uniform PointLight pointLights[NB_POINT_LIGHTS];

vec3 calcDirLight(vec3 color, DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    lightDir = vec3(lightDir.z, lightDir.y, lightDir.x);
    float diff = max(dot(normal, lightDir), 0.0f);

    vec3 ambient = light.ambient * 0.5f;
    vec3 diffuse = light.diffuse * diff;
    return (ambient + diffuse) * color;
}

void main()
{
    vec2 heightTexSize = textureSize(heightMap, 0);
    float uTexelSizex = 1.0f / heightTexSize.x;
    float uTexelSizey = 1.0f / heightTexSize.y;

    float left = texture(heightMap, te_TexCoord + vec2(-uTexelSizex, 0.0f)).r * 32.0f - 1.0f;
    float right = texture(heightMap, te_TexCoord + vec2( uTexelSizex, 0.0f)).r * 32.0f - 1.0f;
    float up = texture(heightMap, te_TexCoord + vec2(0.0f,  uTexelSizey)).r * 32.0f - 1.0f;
    float down = texture(heightMap, te_TexCoord + vec2(0.0f, -uTexelSizey)).r * 32.0f - 1.0f;
    vec3 objectNormal = vec3(down - up, 1.0f, left - right);
    vec3 worldNormal = normalize(mat3(inverse(model)) * objectNormal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 diffuseMap;
    if (worldNormal.y > 0.4f) {
        vec3 normalMap = texture(normalTexture, te_TexCoord * heightTexSize / 8).rgb;
        normalMap = normalize(normalMap * 2.0f - 1.0f);
        diffuseMap = texture(diffuseTexture, te_TexCoord * heightTexSize / 8).rgb;
        diffuseMap = diffuseMap * (1.0f - normalMap.y);
    } else if (worldNormal.y > 0.3f) {
        diffuseMap = texture(diffuseTexture2, te_TexCoord * heightTexSize / 8).rgb;
    } else {
        diffuseMap = texture(diffuseTexture3, te_TexCoord * heightTexSize / 8).rgb;
    }

    vec3 result = vec3(0.0f);
    for (int i = 0; i < dirLightCount && i < NB_DIR_LIGHTS; i++)
        result += calcDirLight(diffuseMap, dirLights[i], worldNormal, viewDir);

    // for (int i = 0; i < pointLightCount && i < NB_POINT_LIGHTS; i++)
        // result += calcPointLight(object, pointLights[i], worldNormal, FragPos, viewDir);

    FragColor = vec4(result, 1.0f);
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
