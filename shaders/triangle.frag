#version 330 core

in vec3 vNormal;
in vec2 vTextCoord;

out vec4 FragColor;

void main()
{
    vec3 normal = normalize(vNormal);

    vec3 color = normal * 0.5 + 0.5;
    FragColor = vec4(color, 1.0);
}
