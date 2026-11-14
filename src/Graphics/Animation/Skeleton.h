#pragma once

#include <string>
#include <vector>

#include <assimp/matrix4x4.h>
#include <assimp/scene.h>

struct SkeletonNode
{
    std::string name;

    // 부모 노드 기준 기본 변환 행렬
    aiMatrix4x4 localTransform;

    std::vector<SkeletonNode> children;
};

// Assimp의 노드 트리를 자체 구조체로 복사
inline SkeletonNode BuildSkeletonNode(const aiNode* node)
{
    SkeletonNode result;

    result.name = node->mName.C_Str();
    result.localTransform = node->mTransformation;

    result.children.reserve(node->mNumChildren);

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        result.children.push_back(
            BuildSkeletonNode(node->mChildren[i])
        );
    }

    return result;
}
