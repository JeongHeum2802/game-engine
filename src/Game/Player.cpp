#include "Player.h"

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
