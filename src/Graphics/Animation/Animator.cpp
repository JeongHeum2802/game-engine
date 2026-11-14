#include "Animator.h"

#include <algorithm>
#include <cmath>
#include <iostream>

namespace
{
    aiVector3D InterpolateVector(
        const std::vector<aiVectorKey>& keys,
        double time,
        const aiVector3D& fallback
    )
    {
        if (keys.empty())
            return fallback;

        if (keys.size() == 1 || time <= keys.front().mTime)
            return keys.front().mValue;

        if (time >= keys.back().mTime)
            return keys.back().mValue;

        for (size_t i = 0; i + 1 < keys.size(); ++i)
        {
            const auto& first = keys[i];
            const auto& second = keys[i + 1];

            if (time <= second.mTime)
            {
                double span = second.mTime - first.mTime;

                float factor = span > 0.0
                    ? static_cast<float>(
                        (time - first.mTime) / span
                    )
                    : 0.0f;

                return first.mValue * (1.0f - factor)
                     + second.mValue * factor;
            }
        }

        return keys.back().mValue;
    }

    aiQuaternion InterpolateRotation(
        const std::vector<aiQuatKey>& keys,
        double time,
        const aiQuaternion& fallback
    )
    {
        if (keys.empty())
            return fallback;

        if (keys.size() == 1 || time <= keys.front().mTime)
            return keys.front().mValue;

        if (time >= keys.back().mTime)
            return keys.back().mValue;

        for (size_t i = 0; i + 1 < keys.size(); ++i)
        {
            const auto& first = keys[i];
            const auto& second = keys[i + 1];

            if (time <= second.mTime)
            {
                double span = second.mTime - first.mTime;

                float factor = span > 0.0
                    ? static_cast<float>(
                        (time - first.mTime) / span
                    )
                    : 0.0f;

                aiQuaternion result;

                aiQuaternion::Interpolate(
                    result,
                    first.mValue,
                    second.mValue,
                    factor
                );

                result.Normalize();
                return result;
            }
        }

        return keys.back().mValue;
    }
}

Animator::Animator(const AnimatedModel& model)
    : model(model)
{
    finalBoneMatrices.resize(
        model.GetBones().size(),
        Mat4::Identity()
    );

    // Skeleton 루트의 기본 변환 역행렬
    globalInverseTransform =
        model.GetRootNode().localTransform;

    globalInverseTransform.Inverse();
}

bool Animator::Play(
    const std::string& clipName,
    bool loop
)
{
    const AnimationClip* clip =
        model.FindAnimation(clipName);

    if (!clip)
    {
        std::cerr << "[Animator] Clip not found: "
                  << clipName << '\n';
        return false;
    }

    // 같은 애니메이션이면 처음부터 다시 시작하지 않음
    if (currentClip == clip && looping == loop)
        return true;

    currentClip = clip;
    currentTime = 0.0;
    looping = loop;
    finished = false;

    return true;
}

void Animator::Update(float deltaTime)
{
    if (!currentClip)
        return;

    if (deltaTime < 0.0f)
        return;

    const double duration = currentClip->duration;
    const double ticksPerSecond =
        currentClip->ticksPerSecond;

    if (!finished || looping)
    {
        currentTime +=
            static_cast<double>(deltaTime) *
            ticksPerSecond;
    }

    if (duration > 0.0)
    {
        if (looping)
        {
            currentTime = std::fmod(
                currentTime,
                duration
            );
        }
        else if (currentTime >= duration)
        {
            currentTime = duration;
            finished = true;
        }
    }
    else
    {
        currentTime = 0.0;
        if (!looping)
            finished = true;
    }

    std::fill(
        finalBoneMatrices.begin(),
        finalBoneMatrices.end(),
        Mat4::Identity()
    );

    aiMatrix4x4 identity;

    CalculateBoneTransforms(
        model.GetRootNode(),
        identity
    );
}


aiMatrix4x4 Animator::ToLocalMatrix(
    const aiVector3D& position,
    const aiQuaternion& rotation,
    const aiVector3D& scale
)
{
    // Assimp 생성자 인자 순서: Scale, Rotation, Position
    return aiMatrix4x4(
        scale,
        rotation,
        position
    );
}

aiMatrix4x4 Animator::InterpolateNodeTransform(
    const BoneChannel& channel,
    const aiMatrix4x4& bindTransform
) const
{
    // 일부 키가 누락되면 기본 변환을 사용
    aiVector3D defaultScale;
    aiQuaternion defaultRotation;
    aiVector3D defaultPosition;

    bindTransform.Decompose(
        defaultScale,
        defaultRotation,
        defaultPosition
    );

    aiVector3D position = InterpolateVector(
        channel.positionKeys,
        currentTime,
        defaultPosition
    );

    aiQuaternion rotation = InterpolateRotation(
        channel.rotationKeys,
        currentTime,
        defaultRotation
    );

    aiVector3D scale = InterpolateVector(
        channel.scaleKeys,
        currentTime,
        defaultScale
    );

    return ToLocalMatrix(position, rotation, scale);
}


void Animator::CalculateBoneTransforms(
    const SkeletonNode& node,
    const aiMatrix4x4& parentTransform
)
{
    aiMatrix4x4 localTransform =
        node.localTransform;

    // 현재 노드의 애니메이션 키프레임 확인
    if (currentClip)
    {
        const BoneChannel* channel =
            currentClip->FindChannel(node.name);

        if (channel)
        {
            localTransform = InterpolateNodeTransform(
                *channel,
                node.localTransform
            );
        }
    }

    // 부모 변환 누적
    aiMatrix4x4 globalTransform =
        parentTransform * localTransform;

    // 이 노드가 실제 Skinning Bone인지 확인
    int boneIndex =
        model.GetBoneIndex(node.name);

    if (boneIndex >= 0)
    {
        const BoneInfo& bone =
            model.GetBones()[boneIndex];

        // 최종 스키닝 행렬
        aiMatrix4x4 finalTransform =
            globalInverseTransform *
            globalTransform *
            bone.offsetMatrix;

        finalBoneMatrices[boneIndex] =
            ToMat4(finalTransform);
    }

    // 자식 노드 재귀 처리
    for (const auto& child : node.children)
    {
        CalculateBoneTransforms(
            child,
            globalTransform
        );
    }
}


Mat4 Animator::ToMat4(const aiMatrix4x4& matrix)
{
    Mat4 result = Mat4::Identity();

    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            result(row, col) = matrix[row][col];
        }
    }

    return result;
}


void Animator::Restart()
{
    if (!currentClip)
        return;

    currentTime = 0.0;
    finished = false;
}