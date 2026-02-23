#version 460 core

in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uCloudTex;

void main()
{
    // Simple vertical sky gradient
    vec3 sky = mix(
        vec3(0.6, 0.75, 1.0),
        vec3(0.1, 0.2, 0.4),
        vUV.y
    );

    // Upsampled cloud texture
    float cloud = texture(uCloudTex, vUV).r;

    // Blend clouds over sky
    vec3 color = mix(sky, vec3(1.0), cloud);

    FragColor = vec4(color, 1.0);
}