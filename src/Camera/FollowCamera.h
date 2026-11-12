#include "Camera.h"
#include "../Math/Vec3.h"

class FollowCamera
{
private:
    Camera camera;
    Vec3 offset {0.0f, 2.0f, 5.0f};

public:
    void Update(const Vec3& targetPosition)
    {
        camera.SetPosition(targetPosition + offset);
        camera.SetTarget(targetPosition);
    }

    Camera& GetCamera()
    {
        return camera;
    }

    void SetOffset(const Vec3& newOffset)
    {
        offset = newOffset;
    }
};