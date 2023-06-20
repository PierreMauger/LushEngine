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

#define MAX_BONES 100
#define MAX_BONE_INFLUENCE 4

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 totalPosition = vec4(aPos, 1.0f);

    Normal = mat3(transpose(inverse(model))) * aNormal;
    FragPos = vec3(model * totalPosition);
    TexCoords = aTexCoords;

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

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

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

    vec3 ambient = light.ambient * 0.2f;
    vec3 diffuse = light.diffuse * diff;
    vec3 specular = light.specular * spec * object.specular;
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
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * 0.2f;
    vec3 diffuse = light.diffuse * diff;
    vec3 specular = light.specular * spec * object.specular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular) * object.diffuse;
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

    if (hasTexture) {
        result *= texture(tex.diffuse, TexCoords).a;
        if (texture(tex.diffuse, TexCoords).a <= 0.2f)
            discard;
    }

    FragColor = vec4(result + object.emission, 1.0f);
}
