#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <assimp/texture.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

Model::Model(const std::string& path)
{
    Load(path);
}

void Model::Load(const std::string& path)
{
    modelDirectory = std::filesystem::path(path).parent_path();

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_JoinIdenticalVertices |
        aiProcess_PreTransformVertices
    );

    if (!scene ||
          !scene->mRootNode ||
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

        meshes.push_back(ProcessMesh(mesh, scene));
    }

    // 자식 노드를 재귀적으로 처리
    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

std::unique_ptr<Mesh> Model::ProcessMesh(
    const aiMesh* mesh,
    const aiScene* scene
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

    MaterialData material = LoadMaterial(mesh, scene);

    return std::make_unique<Mesh>(vertices, indices, std::move(material));
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


MaterialData Model::LoadMaterial(
    const aiMesh* mesh,
    const aiScene* scene
)
{
    MaterialData result;

    if (mesh->mMaterialIndex >= scene->mNumMaterials)
        return result;

    const aiMaterial* material =
        scene->mMaterials[mesh->mMaterialIndex];

    // GLB의 BaseColor Factor
    aiColor4D color;

    if (material->Get(
        AI_MATKEY_BASE_COLOR, color
    ) == AI_SUCCESS)
    {
        result.baseColor = Vec3(
            color.r, color.g, color.b
        );
    }
    else
    {
        // OBJ 등의 전통적인 Diffuse Color
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

    // GLB의 BaseColor Texture 우선 사용
    aiTextureType type = aiTextureType_BASE_COLOR;

    if (material->GetTextureCount(type) == 0)
        type = aiTextureType_DIFFUSE;

    if (material->GetTextureCount(type) > 0)
    {
        aiString texturePath;

        if (material->GetTexture(
            type, 0, &texturePath
        ) == AI_SUCCESS)
        {
            result.baseColorTexture =
                LoadTexture(scene, texturePath);
        }
    }

    return result;
}



std::shared_ptr<Texture2D> Model::LoadTexture(
    const aiScene* scene,
    const aiString& texturePath
)
{
    std::string path = texturePath.C_Str();

    // 동일 텍스처 중복 로딩 방지
    auto found = textureCache.find(path);

    if (found != textureCache.end())
        return found->second;

    std::shared_ptr<Texture2D> texture;

    // GLB 내부에 포함된 텍스처 확인
    const aiTexture* embedded =
        scene->GetEmbeddedTexture(path.c_str());

    if (embedded)
    {
        if (embedded->mHeight == 0)
        {
            // PNG / JPEG 등 압축 이미지
            const unsigned char* bytes =
                reinterpret_cast<const unsigned char*>(
                    embedded->pcData
                );

            texture = std::make_shared<Texture2D>(
                bytes,
                embedded->mWidth
            );
        }
        else
        {
            // 압축되지 않은 BGRA 이미지
            const unsigned char* pixels =
                reinterpret_cast<const unsigned char*>(
                    embedded->pcData
                );

            texture = std::make_shared<Texture2D>(
                pixels,
                static_cast<int>(embedded->mWidth),
                static_cast<int>(embedded->mHeight),
                GL_BGRA
            );
        }
    }
    else
    {
        // 외부 이미지 파일 로딩
        std::filesystem::path fullPath =
            modelDirectory / path;

        std::ifstream file(fullPath, std::ios::binary);

        if (!file)
        {
            throw std::runtime_error(
                "Texture not found: " + fullPath.string()
            );
        }

        std::vector<unsigned char> bytes(
            std::istreambuf_iterator<char>{file},
            std::istreambuf_iterator<char>{}
        );

        texture = std::make_shared<Texture2D>(
            bytes.data(),
            bytes.size()
        );
    }

    textureCache[path] = texture;

    return texture;
}
