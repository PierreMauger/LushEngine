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
out vec4 FragPosLightSpace;

#define MAX_BONES 100
#define MAX_BONE_INFLUENCE 4

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main()
{
    vec4 totalPosition = vec4(aPos, 1.0f);

    Normal = mat3(transpose(inverse(model))) * aNormal;
    FragPos = vec3(model * totalPosition);
    TexCoords = aTexCoords;
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

    gl_Position = projection * view * vec4(FragPos, 1.0);
}

#shader fragment
#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emission;
    float shininess;
};

struct Texture {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct Base {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emission;
    float shininess;
};

struct DirLight {
    vec3 direction;
    vec3 color;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;

    float constant;
    float linear;
    float quadratic;
};

#define NB_DIR_LIGHTS 2
#define NB_POINT_LIGHTS 4

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in vec4 FragPosLightSpace;
uniform sampler2D shadowMap;

uniform vec3 viewPos;
uniform Material material;
uniform Texture tex;
uniform bool hasTexture = false;

uniform int dirLightCount;
uniform DirLight dirLights[NB_DIR_LIGHTS];
uniform int pointLightCount;
uniform PointLight pointLights[NB_POINT_LIGHTS];

uniform float time;

vec3 calcDirLight(Base object, DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0f);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), object.shininess);
    // combine results

    vec3 ambient = light.color * 0.5f;
    vec3 diffuse = light.color * diff;
    vec3 specular = light.color * spec * object.specular;
    return (ambient + diffuse + specular) * object.diffuse;
}

vec3 calcPointLight(Base object, PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0f);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), object.shininess);
    // attenuation
    float distance = length(light.position - fragPos) / light.intensity;
    // float attenuation = 1.0f / (light.constant + light.linear * sqrt(distance) + light.quadratic * distance);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.color * 0.2f;
    vec3 diffuse = light.color * diff;
    vec3 specular = light.color * spec * object.specular;
    return (ambient + diffuse + specular) * object.diffuse * attenuation;
}
float calcShadow(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5f + 0.5f;
    if (projCoords.x < 0.0f || projCoords.y < 0.0f || projCoords.z < 0.0f || projCoords.x > 1.0f || projCoords.y > 1.0f || projCoords.z > 1.0f)
        return 0.0f;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    // float shadow = currentDepth - 0.005f > closestDepth ? 0.5f : 0.0f;
    float shadow = 0.0f;
    vec2 texelSize = 1.0f / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth > pcfDepth  ? 0.5f : 0.0f;
        }
    }
    shadow /= 9.0;
    return shadow;
}

void main()
{
    Base object;
    object.ambient = hasTexture ? vec3(0.0f) : material.ambient;
    object.diffuse = hasTexture ? texture(tex.diffuse, TexCoords).rgb : material.diffuse;
    object.specular = hasTexture ? texture(tex.specular, TexCoords).rgb : material.specular;
    object.emission = hasTexture ? texture(tex.emission, TexCoords).rgb : material.emission;
    object.shininess = hasTexture ? tex.shininess : material.shininess;

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0f);

    for (int i = 0; i < dirLightCount && i < NB_DIR_LIGHTS; i++)
        result += calcDirLight(object, dirLights[i], norm, viewDir);

    for (int i = 0; i < pointLightCount && i < NB_POINT_LIGHTS; i++)
        result += calcPointLight(object, pointLights[i], norm, FragPos, viewDir);

    if (hasTexture && texture(tex.diffuse, TexCoords).a == 0.0f)
        discard;

    float shadow = calcShadow(FragPosLightSpace);
    result *= 1.0f - shadow;

    FragColor = vec4(result + object.emission, texture(tex.diffuse, TexCoords).a);
}
