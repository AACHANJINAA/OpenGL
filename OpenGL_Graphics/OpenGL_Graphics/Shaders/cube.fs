#version 330 core
out vec4 FragColor;

in vec3 ourColor;

uniform float u_selected; // 0.0 = normal, 1.0 = selected

void main()
{
    vec3 baseColor = ourColor;
    if (u_selected > 0.5) {
        // Blend with yellow (80% original, 20% bright yellow/gold glow)
        baseColor = mix(ourColor, vec3(1.0, 0.9, 0.0), 0.4);
    }
    FragColor = vec4(baseColor, 1.0);
}
