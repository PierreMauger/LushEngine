#version 330 core
out vec4 FragColor;

in vec3 nearPoint;
in vec3 farPoint;
in mat4 projectionMatrix;
in mat4 viewMatrix;

vec4 grid(vec3 fragPos3D, float scale) {
    vec2 coord = fragPos3D.xz * scale; // use the scale variable to set the distance between the lines
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.3, 0.3, 0.3, 1.0 - min(line, 1.0));
    // z axis
    if (fragPos3D.x > -1 / scale * minimumx && fragPos3D.x < 1 / scale * minimumx)
        color.rgb = vec3(0.1, 0.1, 1.0);
    // x axis
    if (fragPos3D.z > -1 / scale * minimumz && fragPos3D.z < 1 / scale * minimumz)
        color.rgb = vec3(1.0, 0.1, 0.1);
    return color;
}

float computeDepth(vec3 pos) {
    vec4 clip_space_pos = projectionMatrix * viewMatrix * vec4(pos.xyz, 1.0);
    return clip_space_pos.z / clip_space_pos.w * 0.5 + 0.5;
}

float computeLinearDepth(vec3 pos) {
    float near = 0.1;
    float far = 100.0;
    vec4 clip_space_pos = projectionMatrix * viewMatrix * vec4(pos.xyz, 1.0);
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0; // put back between -1 and 1
    float linearDepth = (2.0 * near * far) / (far + near - clip_space_depth * (far - near)); // get linear value between 0.01 and 100
    return linearDepth / far; // normalize
}

void main()
{
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);

    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);

    float linearDepth = computeLinearDepth(fragPos3D);
    float fading = max(0, (0.7 - linearDepth));

    gl_FragDepth = computeDepth(fragPos3D);
    FragColor = (grid(fragPos3D, 0.1) + grid(fragPos3D, 1)) * float(t > 0);
    FragColor.a *= fading;
}
