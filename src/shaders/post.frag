#version 460 core

in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uSceneTex;

void main()
{
    // Pixel color
    vec3 col = texture(uSceneTex, vUV).rgb;

    // Gamma correction
    col = pow(col, vec3(1.0 / 2.2));

    // Output color
    FragColor = vec4(col, 1.0);
}