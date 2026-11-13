#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <stdexcept>
#include <utility>

Model::Model(const std::string& path)
{
    Load(path);
}

void Model::Load(const std::string& path)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_JoinIdenticalVertices |
        aiProcess_PreTransformVertices
    );

    if (!scene->mRootNode ||
        (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE))
    {
        std::cerr
            << "[Assimp] Incomplete scene: "
            << path << '\n'
            << "[Assimp] Error: "
            << importer.GetErrorString() << '\n';

        throw std::runtime_error(
            "Model scene is incomplete"
        );
    }

    ProcessNode(scene->mRootNode, scene);

    std::cout << "Model loaded: " << path << '\n';
    std::cout << "Mesh count: " << meshes.size() << '\n';
}

void Model::ProcessNode(
    const aiNode* node,
    const aiScene* scene
)
{
    // 현재 노드에 포함된 Mesh 처리
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        unsigned int meshIndex = node->mMeshes[i];

        const aiMesh* mesh = scene->mMeshes[meshIndex];

        meshes.push_back(ProcessMesh(mesh));
    }

    // 자식 노드를 재귀적으로 처리
    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

std::unique_ptr<Mesh> Model::ProcessMesh(
    const aiMesh* mesh
)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    vertices.reserve(mesh->mNumVertices);

    // 1. 정점 데이터 추출
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex vertex{};

        // Position
        vertex.position = Vec3(
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        );

        // Normal
        if (mesh->HasNormals())
        {
            vertex.normal = Vec3(
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            );
        }

        // UV (첫 번째 UV 채널)
        if (mesh->HasTextureCoords(0))
        {
            vertex.texCoord = Vec2(
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            );
        }

        vertices.push_back(vertex);
    }

    // 2. 인덱스 데이터 추출
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
        const aiFace& face = mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; ++j)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    return std::make_unique<Mesh>(vertices, indices);
}

void Model::Draw() const
{
    for (const auto& mesh : meshes)
    {
        mesh->Draw();
    }
}

std::vector<Mesh*> Model::GetMeshes() const
{
    std::vector<Mesh*> result;
    result.reserve(meshes.size());

    for (const auto& mesh : meshes)
    {
        result.push_back(mesh.get());
    }

    return result;
}