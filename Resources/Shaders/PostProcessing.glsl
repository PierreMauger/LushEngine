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

in vec2 TexCoords;

uniform sampler2D tex;

// const float offset_x = 1.0f / 800.0f;
// const float offset_y = 1.0f / 800.0f;

// vec2 offsets[9] = vec2[]
// (
//     vec2(-offset_x,  offset_y), vec2( 0.0f,    offset_y), vec2( offset_x,  offset_y),
//     vec2(-offset_x,  0.0f),     vec2( 0.0f,    0.0f),     vec2( offset_x,  0.0f),
//     vec2(-offset_x, -offset_y), vec2( 0.0f,   -offset_y), vec2( offset_x, -offset_y)
// );

// float kernel[9] = float[]
// (
//     0,  -1, 0,
//     -1, 5, -1,
//     0,  -1, 0
// );

void main()
{
    // 3 - kernel filter
    // vec3 color = vec3(0.0f);
    // for(int i = 0; i < 9; i++)
        // color += vec3(texture(tex, TexCoords.st + offsets[i])) * kernel[i];
    vec3 color = texture(tex, TexCoords).rgb;
    // 1 - negative
    // color = 1.0 - color;
    // 2 - gamma correction
    // color.rgb = pow(color.rgb, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0f);
}
