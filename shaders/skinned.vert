#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

layout(location = 3) in ivec4 aBoneIDs;
layout(location = 4) in vec4 aWeights;

const int MAX_BONES = 100;

uniform mat4 uBoneMatrices[MAX_BONES];

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 vNormal;
out vec3 vWorldPos;

void main()
{
    mat4 skinMatrix = mat4(0.0);
    float totalWeight = 0.0;

    for (int i = 0; i < 4; ++i)
    {
        int boneID = aBoneIDs[i];
        float weight = aWeights[i];

        if (boneID < 0 || weight <= 0.0)
            continue;

        if (boneID >= MAX_BONES)
            continue;

        skinMatrix +=
            uBoneMatrices[boneID] * weight;

        totalWeight += weight;
    }

    // 본 가중치가 없는 정점은 원래 위치 유지
    if (totalWeight < 0.0001)
    {
        skinMatrix = mat4(1.0);
    }
    else
    {
        skinMatrix /= totalWeight;
    }

    vec4 skinnedPosition =
        skinMatrix * vec4(aPosition, 1.0);

    vec4 worldPosition =
        uModel * skinnedPosition;

    vWorldPos = worldPosition.xyz;

    // 스키닝된 Normal
    vec3 skinnedNormal =
        mat3(skinMatrix) * aNormal;

    vNormal = normalize(
        transpose(inverse(mat3(uModel))) *
        skinnedNormal
    );

    gl_Position =
        uProjection * uView * worldPosition;
}
