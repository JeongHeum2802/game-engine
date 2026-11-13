#pragma once

#include <optional>
#include <random>

#include "Math/Vec3.h"

class ZombieSpawner
{
public:
    ZombieSpawner();

    // 생성 시간이 되었으면 위치 반환
    std::optional<Vec3> Update(
        float deltaTime,
        const Vec3& playerPosition,
        bool canSpawn
    );

private:
    std::optional<Vec3> GenerateSpawnPosition(
        const Vec3& playerPosition
    );

    std::mt19937 randomEngine;

    float elapsedTime = 0.0f;
    float spawnTimer = 0.0f;

    float minSpawnRadius = 8.0f;
    float maxSpawnRadius = 12.0f;

    // 20x20 바닥 내부에 생성
    float mapMin = -19.5f;
    float mapMax = 19.5f;
};
