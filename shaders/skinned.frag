#version 330 core

in vec3 vNormal;
in vec3 vWorldPos;

out vec4 FragColor;

uniform vec3 uBaseColor;
uniform vec3 uLightDirection;
uniform vec3 uLightColor;
uniform float uLightIntensity;

void main()
{
    vec3 normal = normalize(vNormal);

    vec3 lightDir = normalize(-uLightDirection);

    float diffuse = max(
        dot(normal, lightDir),
        0.0
    );

    vec3 lighting =
        vec3(0.35) +
        uLightColor * diffuse * uLightIntensity;

    FragColor = vec4(
        uBaseColor * lighting,
        1.0
    );
}
