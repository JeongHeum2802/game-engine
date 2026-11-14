#pragma once

#include <algorithm>
#include <cmath>
#include <vector>

#include "Game/GameObject.h"
#include "Graphics/Mesh.h"
#include "Shader.h"

class BulletEffects
{
public:
    explicit BulletEffects(Mesh* cubeMesh)
        : cubeMesh(cubeMesh)
    {
    }

    // 발사 순간 한 번 호출
    void Spawn(
        const Vec3& origin,
        const Vec3& direction,
        float distance,
        bool hit
    )
    {
        if (distance <= 0.0f)
            return;

        Shot shot;
        shot.origin = origin;
        shot.direction = direction; // 정규화된 XZ 방향
        shot.distance = distance;
        shot.hit = hit;

        shots.push_back(shot);
    }

    // 매 프레임 호출
    void Update(float deltaTime)
    {
        for (auto& shot : shots)
        {
            shot.age += deltaTime;
        }

        shots.erase(
            std::remove_if(
                shots.begin(),
                shots.end(),
                [](const Shot& shot)
                {
                    // 탄환 도착 이후 피격 효과까지 유지
                    float travelTime =
                        shot.distance / BULLET_SPEED;

                    return shot.age >
                        travelTime + IMPACT_DURATION;
                }
            ),
            shots.end()
        );
    }

    // 3D 월드 위에 이펙트 출력
    void Render(Shader& shader)
    {
        for (const auto& shot : shots)
        {
            const float travelTime =
                shot.distance / BULLET_SPEED;

            // 1. 총구 섬광
            if (shot.age < FLASH_DURATION)
            {
                float ratio =
                    1.0f - shot.age / FLASH_DURATION;

                DrawCube(
                    shader,
                    shot.origin,
                    Vec3(
                        0.28f * ratio,
                        0.28f * ratio,
                        0.28f * ratio
                    ),
                    0.0f,
                    Vec3(1.0f, 0.85f, 0.2f)
                );
            }

            // 2. 움직이는 탄환 궤적
            if (shot.age <= travelTime)
            {
                float frontDistance = std::min(
                    shot.age * BULLET_SPEED,
                    shot.distance
                );

                float backDistance = std::max(
                    0.0f,
                    frontDistance - TRACER_LENGTH
                );

                float length =
                    frontDistance - backDistance;

                if (length > 0.001f)
                {
                    float centerDistance =
                        (frontDistance + backDistance)
                        * 0.5f;

                    Vec3 center =
                        shot.origin +
                        shot.direction * centerDistance;

                    float yaw = std::atan2(
                        shot.direction.x,
                        shot.direction.z
                    );

                    DrawCube(
                        shader,
                        center,
                        Vec3(0.07f, 0.07f, length),
                        yaw,
                        Vec3(1.0f, 0.75f, 0.12f)
                    );
                }
            }

            // 3. 명중 시 충돌 섬광
            if (shot.hit &&
                shot.age >= travelTime &&
                shot.age <
                    travelTime + IMPACT_DURATION)
            {
                float ratio = 1.0f -
                    (shot.age - travelTime)
                    / IMPACT_DURATION;

                Vec3 impactPosition =
                    shot.origin +
                    shot.direction * shot.distance;

                DrawCube(
                    shader,
                    impactPosition,
                    Vec3(
                        0.35f * ratio,
                        0.35f * ratio,
                        0.35f * ratio
                    ),
                    0.0f,
                    Vec3(1.0f, 0.3f, 0.08f)
                );
            }
        }
    }

private:
    struct Shot
    {
        Vec3 origin;
        Vec3 direction;

        float distance = 0.0f;
        float age = 0.0f;

        bool hit = false;
    };

    // 시각 효과용 상수
    static constexpr float BULLET_SPEED = 45.0f;
    static constexpr float TRACER_LENGTH = 0.75f;
    static constexpr float FLASH_DURATION = 0.07f;
    static constexpr float IMPACT_DURATION = 0.12f;

    Mesh* cubeMesh = nullptr;

    std::vector<Shot> shots;

    void DrawCube(
        Shader& shader,
        const Vec3& position,
        const Vec3& scale,
        float yaw,
        const Vec3& color
    )
    {
        GameObject effect(cubeMesh);

        effect.position = position;
        effect.scale = scale;
        effect.rotation.y = yaw;
        effect.color = color;

        effect.Render(shader);
    }
};
