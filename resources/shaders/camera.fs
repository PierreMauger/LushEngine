#version 330 core

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

#define NB_POINT_LIGHTS 4

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform Texture tex;
uniform DirLight dirLight;
uniform PointLight pointLights[NB_POINT_LIGHTS];
uniform int pointLightCount;
uniform bool hasTexture = false;
uniform float time;

vec3 calcDirLight(Base object, DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), object.shininess);
    vec3 ambient = light.ambient * object.ambient;
    vec3 diffuse = light.diffuse * (diff * object.diffuse);
    vec3 specular = light.specular * (spec * object.specular);
    return ambient + diffuse + specular;
}

vec3 calcPointLight(Base object, PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), object.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * object.ambient;
    vec3 diffuse = light.diffuse * diff * object.diffuse;
    vec3 specular = light.specular * spec * object.specular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return ambient + diffuse + specular;
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

    vec3 result = calcDirLight(object, dirLight, norm, viewDir);

    for (int i = 0; i < pointLightCount && i < NB_POINT_LIGHTS; i++)
        result += calcPointLight(object, pointLights[i], norm, FragPos, viewDir);

    FragColor = vec4(result + object.emission, 1.0f);
}
