#pragma once

#include "GameObject.h"
#include <memory>
#include <string>

#include "Graphics/Animation/AnimatedCharacter.h"

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

    // 좀비 생성 후 애니메이션 연결
    void SetupAnimation(
        const AnimatedModelResource& resource,
        const std::string& walkClip,
        const std::string& attackClip = ""
    );

    void UpdateAnimation(float deltaTime);

    void TriggerAttackAnimation();

    void RenderAnimation(
        Shader& shader,
        const Mat4& modelMatrix
    ) const;

    bool HasAnimation() const { return visual != nullptr; }

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

    std::unique_ptr<AnimatedCharacter> visual;

    std::string walkAnimation;
    std::string attackAnimation;

    bool isPlayingAttackAnimation = false;
};
