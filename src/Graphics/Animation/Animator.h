#pragma once

#include <string>
#include <vector>

#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>
#include <assimp/vector3.h>

#include "AnimatedModel.h"
#include "Math/Mat4.h"

class Animator
{
public:
    explicit Animator(const AnimatedModel& model);

    // 같은 클립을 반복 요청하면 재생 시간 유지
    bool Play(
        const std::string& clipName,
        bool loop = true
    );

    void Restart();

    void Update(float deltaTime);

    const std::vector<Mat4>& GetFinalBoneMatrices() const
    {
        return finalBoneMatrices;
    }

    bool IsFinished() const
    {
        return finished;
    }

private:
    const AnimatedModel& model;
    const AnimationClip* currentClip = nullptr;

    double currentTime = 0.0; // 애니메이션 Tick 단위

    bool looping = true;
    bool finished = false;

    aiMatrix4x4 globalInverseTransform;

    std::vector<Mat4> finalBoneMatrices;

    void CalculateBoneTransforms(
        const SkeletonNode& node,
        const aiMatrix4x4& parentTransform
    );

    aiMatrix4x4 InterpolateNodeTransform(
        const BoneChannel& channel,
        const aiMatrix4x4& bindTransform
    ) const;

    static aiMatrix4x4 ToLocalMatrix(
        const aiVector3D& position,
        const aiQuaternion& rotation,
        const aiVector3D& scale
    );

    static Mat4 ToMat4(const aiMatrix4x4& matrix);
};
