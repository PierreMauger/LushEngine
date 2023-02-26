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

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D specularTexture;
uniform sampler2D diffuseTexture2;
uniform sampler2D diffuseTexture3;

uniform int dirLightCount;
uniform DirLight dirLights[NB_DIR_LIGHTS];
uniform int pointLightCount;
uniform PointLight pointLights[NB_POINT_LIGHTS];

vec3 calcDirLight(vec3 color, DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0f);
    // specular shading
    // combine results

    vec3 ambient = light.ambient * 0.2f;
    vec3 diffuse = light.diffuse * diff;
    return (ambient + diffuse) * color;
}

void main()
{
    float uTexelSizex = 1.0f / 2624.0f;
    float uTexelSizey = 1.0f / 1756.0f;

    float left = texture(heightMap, te_TexCoord + vec2(-uTexelSizex, 0.0f)).r * 32.0f * 2.0f - 1.0f;
    float right = texture(heightMap, te_TexCoord + vec2( uTexelSizex, 0.0f)).r * 32.0f * 2.0f - 1.0f;
    float up = texture(heightMap, te_TexCoord + vec2(0.0f,  uTexelSizey)).r * 32.0f * 2.0f - 1.0f;
    float down = texture(heightMap, te_TexCoord + vec2(0.0f, -uTexelSizey)).r * 32.0f * 2.0f - 1.0f;
    vec3 norm = normalize(vec3(down - up, 1.0f, left - right));
    vec3 viewDir = normalize(viewPos - FragPos);

    // vec3 result = calcDirLight(texture(diffuseTexture, te_TexCoord * 32.0f).rgb, dirLight, norm, viewDir);
    // vec3 result = texture(diffuseTexture, te_TexCoord).rgb;
    // vec3 result = texture(diffuseTexture, te_TexCoord * 32.0f).rgb;

    vec3 diffuseMap;
    if (norm.y > 0.6f) {
        diffuseMap = texture(diffuseTexture, te_TexCoord * 128.0f).rgb;
    } else if (norm.y > 0.3f) {
        diffuseMap = texture(diffuseTexture2, te_TexCoord * 128.0f).rgb;
    } else {
        diffuseMap = texture(diffuseTexture3, te_TexCoord * 128.0f).rgb;
    }

    // Compute the final color using the modified normal
    // vec3 lightDirection = normalize(vec3(1.0, 1.0, 1.0));
    // float diffuseFactor = max(0.3, dot(norm, lightDirection));
    // vec3 result = diffuseMap * diffuseFactor;
    vec3 result = vec3(0.0f);
    for (int i = 0; i < dirLightCount && i < NB_DIR_LIGHTS; i++)
        result += calcDirLight(diffuseMap, dirLights[i], norm, viewDir);

    // for (int i = 0; i < pointLightCount && i < NB_POINT_LIGHTS; i++)
        // result += calcPointLight(object, pointLights[i], norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0f);
}
