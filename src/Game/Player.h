#pragma once

#include "GameObject.h"

class Player : public GameObject
{
public:
    explicit Player(Mesh* mesh)
        : GameObject(mesh)
    {
    }

    void Update(float deltaTime) override;

    float moveSpeed = 3.0f;

    // 매 프레임 입력을 통해 설정할 이동 방향
    Vec3 moveDirection = Vec3(0.0f, 0.0f, 0.0f);
};
