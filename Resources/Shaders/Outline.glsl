#shader vertex
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0f);
}

#shader fragment
#version 330 core
out vec4 FragColor;

#define WIDTH 2

in vec2 TexCoords;

uniform int id;
uniform sampler2D silhouette;

void main()
{
    vec3 obj = texture(silhouette, TexCoords).rgb;
    float colorId = obj.r * 255.0f + obj.g * 255.0f * 255.0f + obj.b * 255.0f * 255.0f * 255.0f;
    FragColor = vec4(0.0f);

    if (id != 0 && colorId == id) {
        int w = WIDTH;

        // if the pixel isn't white, we are on the object silhouette
        if (obj != vec3(0.0f)) {
            vec2 size = 1.0f / textureSize(silhouette, 0);

            for (int i = -w; i <= +w; i++)  {
                for (int j = -w; j <= +w; j++) {
                    if (i == 0 && j == 0)
                        continue;
                    vec2 offset = vec2(i, j) * size;

                    // if one of the pixel-neighbor isn't the hovered object, we are on the border
                    if (texture(silhouette, TexCoords + offset).rgb != obj) {
                        FragColor = vec4(vec3(1.0f), 1.0f);
                        return;
                    }
                }
            }
        }
    }
}
