#version 330 core

in vec3 v_normal;
in vec3 v_position;
out vec4 fragColor;

uniform int  u_flatMode;   // 1 = ignore lighting, output u_flatColor
uniform vec3 u_flatColor;

void main()
{
    if (u_flatMode == 1) {
        fragColor = vec4(u_flatColor, 1.0);
        return;
    }

    // Light direction in view space (from above-right-front)
    vec3 lightDir = normalize(vec3(1.0, 1.0, 2.0));
    vec3 normal = normalize(v_normal);

    // Ambient
    vec3 ambient = vec3(0.15, 0.15, 0.15);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * vec3(0.7, 0.7, 0.7);

    // Specular (Blinn-Phong)
    vec3 viewDir = normalize(-v_position);
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfDir), 0.0), 32.0);
    vec3 specular = spec * vec3(0.3, 0.3, 0.3);

    // Object color (steel blue-grey)
    vec3 objectColor = vec3(0.6, 0.65, 0.7);

    vec3 result = (ambient + diffuse + specular) * objectColor;
    fragColor = vec4(result, 1.0);
}
