#include "AnimatedModelResource.h"

#include <stdexcept>
#include <string>

AnimatedModelResource::AnimatedModelResource(
    const std::string& path
)
    : model(path)
{
    // AnimatedModel에 로딩된 Mesh 데이터를
    // 실제 GPU Mesh로 변환한다.
    for (const auto& meshData : model.GetMeshes())
    {
        meshes.push_back(
            std::make_unique<AnimatedMesh>(
                meshData.vertices,
                meshData.indices
            )
        );
    }
}

void AnimatedModelResource::Draw(
    Shader& shader,
    const std::vector<Mat4>& boneMatrices
) const
{
    constexpr size_t MAX_BONES = 100;

    if (boneMatrices.size() > MAX_BONES)
    {
        throw std::runtime_error(
            "Too many bones for skinned shader"
        );
    }

    shader.Use();

    // 1. Animator가 계산한 본 행렬 전달
    for (size_t i = 0; i < boneMatrices.size(); ++i)
    {
        shader.SetMat4(
            "uBoneMatrices[" + std::to_string(i) + "]",
            boneMatrices[i]
        );
    }

    // 2. 메시별 Material 색상 적용 후 렌더링
    const auto& meshData = model.GetMeshes();

    for (size_t i = 0; i < meshes.size(); ++i)
    {
        shader.SetVec3(
            "uBaseColor",
            meshData[i].baseColor
        );

        meshes[i]->Draw();
    }
}
