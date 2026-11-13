#version 330 core

#define MAX_POINT_LIGHTS 8

struct DirectionalLight
{
    vec3 direction;
    vec3 color;
    float intensity;
};

struct PointLight
{
    vec3 position;
    vec3 color;
    float intensity;

    float constant;
    float linear;
    float quadratic;
};

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vTexCoord;

out vec4 FragColor;

uniform vec3 uObjectColor;

uniform DirectionalLight uDirectionalLight;

uniform int uPointLightCount;
uniform PointLight uPointLights[MAX_POINT_LIGHTS];

// Directional Light 계산
vec3 CalculateDirectionalLight(
    DirectionalLight light,
    vec3 normal
)
{
    vec3 lightDir = normalize(-light.direction);

    float diffuse = max(dot(normal, lightDir), 0.0);

    return diffuse * light.color * light.intensity;
}

// Point Light 계산
vec3 CalculatePointLight(
    PointLight light,
    vec3 normal,
    vec3 worldPos
)
{
    vec3 lightVector = light.position - worldPos;
    float distance = length(lightVector);

    vec3 lightDir = normalize(lightVector);

    float diffuse = max(dot(normal, lightDir), 0.0);

    float attenuation = 1.0 /
        (
            light.constant +
            light.linear * distance +
            light.quadratic * distance * distance
        );

    return diffuse
        * attenuation
        * light.color
        * light.intensity;
}

void main()
{
    vec3 normal = normalize(vNormal);

    // 기본 환경광
    vec3 lighting = vec3(0.33);

    // Directional Light
    lighting += CalculateDirectionalLight(
        uDirectionalLight,
        normal
    );

    // Point Light
    for (int i = 0; i < uPointLightCount; ++i)
    {
        lighting += CalculatePointLight(
            uPointLights[i],
            normal,
            vWorldPos
        );
    }

    vec3 result = uObjectColor * lighting;

    FragColor = vec4(result, 1.0);
}
