#pragma once

#include <memory>
#include <string>
#include <vector>

#include "AnimatedModel.h"
#include "AnimatedMesh.h"
#include "Shader.h"
#include "Math/Mat4.h"

class AnimatedModelResource
{
public:
    explicit AnimatedModelResource(
        const std::string& path
    );

    // Animator가 참조할 모델 데이터
    const AnimatedModel& GetModel() const
    {
        return model;
    }

    // GPU에 본 행렬을 전달하고 메시 렌더링
    void Draw(
        Shader& shader,
        const std::vector<Mat4>& boneMatrices
    ) const;

private:
    AnimatedModel model;

    std::vector<std::unique_ptr<AnimatedMesh>> meshes;
};
