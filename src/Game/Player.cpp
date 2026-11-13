#include "Player.h"

#include <cmath>

void Player::Update(float deltaTime)
{
    // 대각선 이동 속도가 더 빨라지는 현상 방지
    if (moveDirection.Length() > 0.0f)
    {
        Vec3 direction = moveDirection.Normalized();

        position = position +
            direction * moveSpeed * deltaTime;
    }
}

void Player::AimAt(const Vec3& worldPosition)
{
    Vec3 direction = worldPosition - position;

    // Y-Up이므로 수평 회전만 허용
    direction.y = 0.0f;

    if (direction.Length() < 0.0001f)
        return;

    // 캐릭터 모델의 기본 전방이 +Z일 때
    rotation.y = std::atan2(
        direction.x,
        direction.z
    );
}