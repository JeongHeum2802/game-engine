#include "ZombieSpawner.h"

#include <algorithm>
#include <cmath>

ZombieSpawner::ZombieSpawner()
    : randomEngine(std::random_device{}())
{
}

std::optional<Vec3> ZombieSpawner::Update(
    float deltaTime,
    const Vec3& playerPosition,
    bool canSpawn
)
{
    if (deltaTime <= 0.0f)
        return std::nullopt;

    elapsedTime += deltaTime;

    // 3초에서 시작해 점차 0.65초까지 감소
    float spawnInterval = std::max(
        0.65f,
        3.0f - elapsedTime * 0.025f
    );

    // 최대 개수에 도달하면 타이머만 정지
    if (!canSpawn)
        return std::nullopt;

    spawnTimer += deltaTime;

    if (spawnTimer < spawnInterval)
        return std::nullopt;

    spawnTimer = 0.0f;

    return GenerateSpawnPosition(playerPosition);
}

std::optional<Vec3> ZombieSpawner::GenerateSpawnPosition(
    const Vec3& playerPosition
)
{
    constexpr float PI = 3.14159265358979323846f;

    std::uniform_real_distribution<float> angleDist(
        0.0f, 2.0f * PI
    );

    // 면적에 대해 균등하게 생성
    std::uniform_real_distribution<float> radiusSquaredDist(
        minSpawnRadius * minSpawnRadius,
        maxSpawnRadius * maxSpawnRadius
    );

    // 유효한 스폰 지점을 최대 64번 탐색
    for (int i = 0; i < 64; ++i)
    {
        float angle = angleDist(randomEngine);
        float radius = std::sqrt(
            radiusSquaredDist(randomEngine)
        );

        Vec3 position(
            playerPosition.x + std::cos(angle) * radius,
            0.5f,
            playerPosition.z + std::sin(angle) * radius
        );

        // 바닥을 벗어나지 않도록 확인
        if (position.x < mapMin ||
            position.x > mapMax ||
            position.z < mapMin ||
            position.z > mapMax)
        {
            continue;
        }

        return position;
    }

    // 유효한 위치를 찾지 못하면 스폰 생략
    return std::nullopt;
}
