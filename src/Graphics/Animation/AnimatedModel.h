#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include <assimp/scene.h>

#include "AnimatedVertex.h"
#include "Skeleton.h"
#include "AnimationClip.h"

// 하나의 본에 대한 정보
struct BoneInfo
{
    std::string name;

    // Assimp 형식으로 임시 저장
    // 이후 GPU Skinning 단계에서 Mat4로 변환
    aiMatrix4x4 offsetMatrix;
};

// 본 가중치가 포함된 하나의 메시
struct AnimatedMeshData
{
    std::string name;

    std::vector<AnimatedVertex> vertices;
    std::vector<unsigned int> indices;

    Vec3 baseColor = Vec3(1.0f, 1.0f, 1.0f);
};

class AnimatedModel
{
public:
    explicit AnimatedModel(const std::string& path);

    const std::vector<AnimatedMeshData>& GetMeshes() const
    {
        return meshes;
    }

    const std::vector<BoneInfo>& GetBones() const
    {
        return bones;
    }

    int GetBoneIndex(const std::string& name) const;

    const SkeletonNode& GetRootNode() const
    {
        return rootNode;
    }

    const std::vector<AnimationClip>& GetAnimations() const
    {
        return animations;
    }

    const AnimationClip* FindAnimation(
        const std::string& name
    ) const
    {
        for (const auto& clip : animations)
        {
            if (clip.name == name)
                return &clip;
        }

        return nullptr;
    }


private:
    void Load(const std::string& path);

    void ProcessMesh(const aiMesh* mesh, const aiScene* scene);

    int GetOrCreateBone(
        const aiBone* bone
    );

    static void AddBoneInfluence(
        AnimatedVertex& vertex,
        int boneID,
        float weight
    );

    static void NormalizeWeights(
        AnimatedVertex& vertex
    );

private:
    std::vector<AnimatedMeshData> meshes;
    std::vector<BoneInfo> bones;

    std::unordered_map<std::string, int> boneMap;

    SkeletonNode rootNode;
    std::vector<AnimationClip> animations;
};
