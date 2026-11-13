#pragma once

class Weapon
{
public:
    float damage = 25.0f;
    float fireRate = 5.0f;    // 초당 5발
    float range = 30.0f;      // 최대 사거리

    bool TryFire(double currentTime)
    {
        if (fireRate <= 0.0f)
            return false;

        double interval = 1.0 / fireRate;

        if (currentTime - lastFireTime < interval)
            return false;

        lastFireTime = currentTime;
        return true;
    }

private:
    double lastFireTime = -1.0e9;
};
