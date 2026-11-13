#pragma once

#include "GameObject.h"

class Zombie : public GameObject
{
public:
    explicit Zombie(Mesh* mesh);
    explicit Zombie(const Model& model);

    void Update(float deltaTime) override;

    void SetTarget(const Vec3& target);

    void TakeDamage(float damage);

    bool IsDead() const;
    bool IsInAttackRange() const;

    float GetHealth() const;

    bool TryAttack(double currentTime);



    float moveSpeed = 1.5f;
    float attackRange = 1.0f;
    float maxHealth = 100.0f;
    float attackDamage = 10.0f;
    float attackInterval = 1.0f;
    float collisionRadius = 0.5f;

private:
    Vec3 targetPosition = Vec3(0.0f, 0.0f, 0.0f);

    float health = 100.0f;
    bool inAttackRange = false;
    double lastAttackTime = -1.0e9;
};
