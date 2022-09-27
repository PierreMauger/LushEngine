#version 410 core

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

in float Height;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in vec2 TexCoord2;

uniform vec3 viewPos;
uniform mat4 model;
uniform DirLight dirLight;
uniform sampler2D heightMap;
uniform sampler2D textureMap;
uniform vec2 textureCoords;

vec3 calcDirLight(vec3 color, DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(lightDir, reflectDir), 0.0f), 32.0f);
    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * color * 0.5f;
    vec3 specular = light.specular * spec;
    return ambient;
}

void main()
{
    float uTexelSizex = 1.0f / 2624.0f;
    float uTexelSizey = 1.0f / 1756.0f;

    float left = texture(heightMap, TexCoords + vec2(-uTexelSizex, 0.0f)).r * 32.0f * 2.0f - 1.0f;
    float right = texture(heightMap, TexCoords + vec2( uTexelSizex, 0.0f)).r * 32.0f * 2.0f - 1.0f;
    float up = texture(heightMap, TexCoords + vec2(0.0f,  uTexelSizey)).r * 32.0f * 2.0f - 1.0f;
    float down = texture(heightMap, TexCoords + vec2(0.0f, -uTexelSizey)).r * 32.0f * 2.0f - 1.0f;
    vec3 norm = normalize(vec3(down - up, 1.0f, left - right));
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = calcDirLight(texture(textureMap, TexCoords * 32.0f).rgb, dirLight, norm, viewDir);
    FragColor = vec4(result, 1.0f);
}
