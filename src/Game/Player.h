#pragma once

#include "GameObject.h"

class Player : public GameObject
{
public:
    explicit Player(Mesh* mesh) : GameObject(mesh) { }

    Player(const Model& model) : GameObject(model) { }

    void Update(float deltaTime) override;

    float moveSpeed = 3.0f;

    Vec3 moveDirection = Vec3(0.0f, 0.0f, 0.0f);

    void AimAt(const Vec3& worldPosition);

    void TakeDamage(float damage);
    bool IsDead() const;
    float GetHealth() const;
    float GetMaxHealth() const;

private:
    float maxHealth = 100.0f;
    float health = 100.0f;
};
