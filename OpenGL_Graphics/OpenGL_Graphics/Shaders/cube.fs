#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 u_lightDir;
uniform vec3 u_lightColor;
uniform float u_lightIntensity;
uniform vec3 u_ambientLight;
uniform vec3 u_viewPos;
uniform float u_selected; // 0.0 = normal, 1.0 = selected

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(u_viewPos - FragPos);
    
    // Light direction is pointing at origin, so negate it to get vector pointing towards light source
    vec3 lightDir = normalize(-u_lightDir);
    
    // Ambient lighting
    vec3 ambient = u_ambientLight * ourColor;
    
    // Diffuse lighting
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_lightColor * u_lightIntensity * ourColor;
    
    // Specular lighting (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * u_lightColor * u_lightIntensity * 0.4; // Specular intensity 0.4
    
    // Combine results
    vec3 resultColor = ambient + diffuse + specular;
    
    // Apply golden highlight overlay if the object is selected
    if (u_selected > 0.5) {
        resultColor = mix(resultColor, vec3(1.0, 0.9, 0.0), 0.4);
    }
    
    FragColor = vec4(resultColor, 1.0);
}
