#include "Zombie.h"

#include <algorithm>
#include <cmath>

Zombie::Zombie(Mesh* mesh)
    : GameObject(mesh)
{
    health = maxHealth;
}

Zombie::Zombie(const Model& model)
    : GameObject(model)
{
    health = maxHealth;
}

void Zombie::SetTarget(const Vec3& target)
{
    targetPosition = target;
}

void Zombie::Update(float deltaTime)
{
    if (IsDead())
        return;

    Vec3 direction = targetPosition - position;

    // 수평 XZ 평면에서만 이동
    direction.y = 0.0f;

    float distance = direction.Length();

    inAttackRange = distance <= attackRange;

    if (distance < 0.0001f)
        return;

    Vec3 normalizedDirection = direction / distance;

    // 공격 범위 밖이면 플레이어에게 접근
    if (!inAttackRange)
    {
        // 한 프레임에 공격 범위를 지나치지 않도록 제한
        float moveDistance = std::min(
            moveSpeed * deltaTime,
            distance - attackRange
        );

        position = position +
            normalizedDirection * moveDistance;
    }

    // 기본 전방이 +Z인 모델 기준
    rotation.y = std::atan2(
        normalizedDirection.x,
        normalizedDirection.z
    );
}

void Zombie::TakeDamage(float damage)
{
    if (IsDead() || damage <= 0.0f)
        return;

    health = std::max(0.0f, health - damage);
}

bool Zombie::IsDead() const
{
    return health <= 0.0f;
}

bool Zombie::IsInAttackRange() const
{
    return inAttackRange && !IsDead();
}

float Zombie::GetHealth() const
{
    return health;
}


bool Zombie::TryAttack(double currentTime)
{
    if (IsDead() || !IsInAttackRange())
        return false;

    if (attackInterval <= 0.0f)
        return false;

    // 공격 쿨다운 검사
    if (currentTime - lastAttackTime < attackInterval)
        return false;

    lastAttackTime = currentTime;

    return true;
}
