#include "AnimatedModel.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/material.h>

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <utility>

AnimatedModel::AnimatedModel(const std::string& path)
{
    Load(path);
}

void AnimatedModel::Load(const std::string& path)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_JoinIdenticalVertices |
        aiProcess_LimitBoneWeights
    );

    if (!scene ||
        !scene->mRootNode ||
        (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE))
    {
        throw std::runtime_error(
            "AnimatedModel load failed: " +
            std::string(importer.GetErrorString())
        );
    }

    // 1. Skeleton 계층 구조 복사
    rootNode = BuildSkeletonNode(scene->mRootNode);

    // 2. 애니메이션 클립 복사
    animations.clear();
    animations.reserve(scene->mNumAnimations);

    for (unsigned int i = 0; i < scene->mNumAnimations; ++i)
    {
        animations.emplace_back(
            scene->mAnimations[i]
        );
    }

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
    {
        ProcessMesh(scene->mMeshes[i], scene);
    }

    std::cout << "\n[AnimatedModel] "
              << path << '\n';

    std::cout << "Meshes: "
              << meshes.size() << '\n';

    std::cout << "Bones: "
              << bones.size() << '\n';

    std::cout << "Animations: "
              << scene->mNumAnimations << '\n';

    for (const auto& mesh : meshes)
    {
        unsigned int weightedVertices = 0;

        for (const auto& vertex : mesh.vertices)
        {
            float sum = 0.0f;

            for (float weight : vertex.weights)
                sum += weight;

            if (sum > 0.0f)
                ++weightedVertices;
        }

        std::cout
            << "  Mesh: " << mesh.name
            << ", vertices: " << mesh.vertices.size()
            << ", weighted: " << weightedVertices
            << '\n';
    }
}

void AnimatedModel::ProcessMesh(const aiMesh* mesh, const aiScene* scene)
{
    AnimatedMeshData result;
    result.name = mesh->mName.C_Str();

    result.vertices.resize(mesh->mNumVertices);

    // 1. 일반 정점 데이터
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        AnimatedVertex& vertex = result.vertices[i];

        vertex.position = Vec3(
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        );

        if (mesh->HasNormals())
        {
            vertex.normal = Vec3(
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            );
        }

        if (mesh->HasTextureCoords(0))
        {
            vertex.texCoord = Vec2(
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            );
        }
    }

    // 2. 인덱스 데이터
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
        const aiFace& face = mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; ++j)
        {
            result.indices.push_back(face.mIndices[j]);
        }
    }

    // 3. 본과 정점 가중치
    for (unsigned int i = 0; i < mesh->mNumBones; ++i)
    {
        const aiBone* bone = mesh->mBones[i];

        int boneID = GetOrCreateBone(bone);

        for (unsigned int j = 0; j < bone->mNumWeights; ++j)
        {
            const aiVertexWeight& influence =
                bone->mWeights[j];

            unsigned int vertexID = influence.mVertexId;

            if (vertexID >= result.vertices.size())
                continue;

            AddBoneInfluence(
                result.vertices[vertexID],
                boneID,
                influence.mWeight
            );
        }
    }

    // 4. 정점 가중치 정규화
    for (auto& vertex : result.vertices)
    {
        NormalizeWeights(vertex);
    }


    // Material BaseColor 추출
    if (mesh->mMaterialIndex < scene->mNumMaterials)
    {
        const aiMaterial* material =
            scene->mMaterials[mesh->mMaterialIndex];

        aiColor4D color;

        if (material->Get(
            AI_MATKEY_BASE_COLOR,
            color
        ) == AI_SUCCESS)
        {
            result.baseColor = Vec3(
                color.r,
                color.g,
                color.b
            );
        }
        else
        {
            aiColor3D diffuse;

            if (material->Get(
                AI_MATKEY_COLOR_DIFFUSE,
                diffuse
            ) == AI_SUCCESS)
            {
                result.baseColor = Vec3(
                    diffuse.r,
                    diffuse.g,
                    diffuse.b
                );
            }
        }
    }


    meshes.push_back(std::move(result));
}

int AnimatedModel::GetOrCreateBone(const aiBone* bone)
{
    std::string name = bone->mName.C_Str();

    auto it = boneMap.find(name);

    if (it != boneMap.end())
        return it->second;

    int id = static_cast<int>(bones.size());

    BoneInfo info;
    info.name = name;
    info.offsetMatrix = bone->mOffsetMatrix;

    bones.push_back(info);
    boneMap[name] = id;

    return id;
}

int AnimatedModel::GetBoneIndex(
    const std::string& name
) const
{
    auto it = boneMap.find(name);

    if (it == boneMap.end())
        return -1;

    return it->second;
}

void AnimatedModel::AddBoneInfluence(
    AnimatedVertex& vertex,
    int boneID,
    float weight
)
{
    if (weight <= 0.0f)
        return;

    // 빈 슬롯부터 사용
    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
    {
        if (vertex.boneIDs[i] == -1)
        {
            vertex.boneIDs[i] = boneID;
            vertex.weights[i] = weight;
            return;
        }
    }

    // 이미 4개가 있으면 가장 작은 가중치 교체
    int smallestIndex = 0;

    for (int i = 1; i < MAX_BONE_INFLUENCE; ++i)
    {
        if (vertex.weights[i] <
            vertex.weights[smallestIndex])
        {
            smallestIndex = i;
        }
    }

    if (weight > vertex.weights[smallestIndex])
    {
        vertex.boneIDs[smallestIndex] = boneID;
        vertex.weights[smallestIndex] = weight;
    }
}

void AnimatedModel::NormalizeWeights(
    AnimatedVertex& vertex
)
{
    float sum = 0.0f;

    for (float weight : vertex.weights)
        sum += weight;

    if (sum <= 0.0f)
        return;

    for (float& weight : vertex.weights)
        weight /= sum;
}
