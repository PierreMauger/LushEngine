#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform vec3 lightColor;
uniform vec3 objectColor;
uniform bool hasTexture = false;
uniform bool outline = false;
uniform float time;

uniform sampler2D textureDiffuse;

void main()
{
    if (outline) {
        FragColor = vec4(1.0f, 1.0f, 1.0f, 0.5f);
        return;
    }

    // ambient
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    vec3 specular = specularStrength * spec * lightColor;

    if (hasTexture) {
        vec3 textureColor = texture(textureDiffuse, TexCoords).rgb;
        vec3 result = (ambient + diffuse + specular) * textureColor;
        FragColor = vec4(result, 1.0f);
    } else {
        vec3 result = (ambient + diffuse + specular) * objectColor;
        FragColor = vec4(result, 1.0f);
    }

    float mixing = 0.1f;
    FragColor.rgb = mix(FragColor.rgb, vec3(mixing), 0.25f - cos(time * 2.0f) * 0.25f); // mix between 0.0f and 0.5f
}
