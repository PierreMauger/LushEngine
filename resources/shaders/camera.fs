#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct MaterialTexture {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform MaterialTexture tex;
uniform Light light;
uniform bool hasTexture = false;
uniform float time;

void main()
{
    vec3 originAmbient = hasTexture ? material.ambient : material.ambient;
    vec3 originDiffuse = hasTexture ? texture(tex.diffuse, TexCoords).rgb : material.diffuse;
    vec3 originSpecular = hasTexture ? texture(tex.specular, TexCoords).rgb : material.specular;
    float shininess = hasTexture ? tex.shininess : material.shininess;
    vec3 emission = hasTexture ? texture(tex.emission, TexCoords).rgb : vec3(0.0f);
    // ambient
    vec3 ambient = light.ambient * originAmbient;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * (diff * originDiffuse);

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), shininess);
    vec3 specular = light.specular * (spec * originSpecular);

    vec3 result = ambient + diffuse + specular + emission;
    FragColor = vec4(result, 1.0f);
}
