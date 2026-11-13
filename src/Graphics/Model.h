#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Mesh.h"

struct aiScene;
struct aiNode;
struct aiMesh;

class Model
{
public:
    explicit Model(const std::string& path);

    void Draw() const;

    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    std::vector<Mesh*> GetMeshes() const;

private:
    std::vector<std::unique_ptr<Mesh>> meshes;

    void Load(const std::string& path);

    void ProcessNode(
        const aiNode* node,
        const aiScene* scene
    );

    std::unique_ptr<Mesh> ProcessMesh(
        const aiMesh* mesh
    );
};
