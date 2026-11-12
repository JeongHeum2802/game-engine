#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextCoord;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

uniform vec3 uLightDirection;
uniform vec3 uOjectColor;

out vec3 vWorldPos;
out vec3 vNormal;
out vec2 vTextCoord;

void main()
{
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    vWorldPos = worldPos.xyz;

    gl_Position = uProjection * uView * worldPos;

    mat3 normalMatrix = transpose(inverse(mat3(uModel)));
    vNormal = normalize(normalMatrix * aNormal);
    
    vTextCoord = aTextCoord;
}
