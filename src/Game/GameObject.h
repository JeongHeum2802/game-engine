#pragma once

#include <iostream>

#include "../Math/Vec3.h"
#include "../Math/Mat4.h"
#include "../Math/Transform.h"
#include "../Graphics/Mesh.h"
#include "../Graphics/Model.h"
#include "../Shader.h"

class GameObject
{
public:
    explicit GameObject(Mesh* mesh)
    { 
      meshes.push_back(mesh);
    }

    explicit GameObject(const Model& model)
    {
        meshes = model.GetMeshes();
    }

    virtual ~GameObject() = default;

    virtual void Update(float deltaTime)
    {
    }

    virtual void Render(Shader& shader) const
    {
        if (meshes.empty())
            return;

        Mat4 model =
            Transform::Translate(position) *
            Transform::RotateZ(rotation.z) *
            Transform::RotateY(rotation.y) *
            Transform::RotateX(rotation.x) *
            Transform::Scale(scale);

        shader.SetMat4("uModel", model);
        shader.SetVec3("uObjectColor", color);

        for (Mesh* mesh : meshes)
        {
            if (mesh)
                mesh->Draw();
        }
    }

    Vec3 GetPosition()
    {
        return position;
    }

    Vec3 position = Vec3(0.0f, 0.0f, 0.0f);
    Vec3 rotation = Vec3(0.0f, 0.0f, 0.0f);
    Vec3 scale = Vec3(1.0f, 1.0f, 1.0f);

    Vec3 color = Vec3(1.0f, 1.0f, 1.0f);

protected:
    std::vector<Mesh*> meshes;
};
