#version 330 core

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vTextCoord;

uniform vec3 uObjectColor;
uniform vec3 uLightPosition;
uniform vec3 uLightColor;

// 거리 감쇠 계수
uniform float uConstant;
uniform float uLinear;
uniform float uQuadratic;

out vec4 FragColor;

void main()
{
    vec3 normal = normalize(vNormal);

    // 1. 표면에서 광원으로 향하는 벡터
    vec3 lightVector = uLightPosition - vWorldPos;

    float distance = length(lightVector);
    vec3 lightDir = normalize(lightVector);

    // 2. Diffuse 계산
    float diffuse = max(dot(normal, lightDir), 0.0);

    // 3. 거리 감쇠
    float attenuation = 1.0 /
        (uConstant +
         uLinear * distance +
         uQuadratic * distance * distance);

    // 4. 조명 성분
    vec3 ambient = 0.1 * uObjectColor;

    vec3 diffuseColor =
        diffuse * attenuation *
        uLightColor * uObjectColor;

    // 5. 최종 색상
    vec3 result = ambient + diffuseColor;

    FragColor = vec4(result, 1.0);
}
