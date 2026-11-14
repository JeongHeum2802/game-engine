#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <string>
#include <unordered_set>

inline void InspectAnimation(const std::string& path)
{
    Assimp::Importer importer;

    // PreTransformVertices 사용하지 않음!
    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate |
        aiProcess_LimitBoneWeights
    );

    if (!scene || !scene->mRootNode)
    {
        std::cerr << "[Animation] Load failed: "
                  << importer.GetErrorString() << '\n';
        return;
    }

    std::unordered_set<std::string> boneNames;

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
    {
        const aiMesh* mesh = scene->mMeshes[i];

        for (unsigned int j = 0; j < mesh->mNumBones; ++j)
        {
            boneNames.insert(
                mesh->mBones[j]->mName.C_Str()
            );
        }
    }

    std::cout << "\n[Animation Inspector] " << path << '\n';
    std::cout << "Unique bones: " << boneNames.size() << '\n';
    std::cout << "Animations: " << scene->mNumAnimations << '\n';

    for (unsigned int i = 0; i < scene->mNumAnimations; ++i)
    {
        const aiAnimation* animation = scene->mAnimations[i];

        std::cout << "  Clip: " << animation->mName.C_Str()
                  << ", Channels: " << animation->mNumChannels
                  << ", Duration: " << animation->mDuration
                  << ", TicksPerSecond: "
                  << animation->mTicksPerSecond << '\n';
    }
}
