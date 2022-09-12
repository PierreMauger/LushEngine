#version 330 core
out vec4 FragColor;

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in float Height;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;
uniform DirLight dirLight;

vec3 calcDirLight(vec3 color, DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff;
    vec3 specular = light.specular * spec;
    return ambient + diffuse + specular;
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    float h = (Height + 16) / 32.0f;
    vec3 result = calcDirLight(vec3(0.0f, 0.0f, h), dirLight, norm, viewDir);

    FragColor = vec4(result, 1.0f);
}
