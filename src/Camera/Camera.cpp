#include "Camera.h"

#include <stdexcept>

Ray Camera::ScreenPointToRay(
    double mouseX,
    double mouseY,
    int screenWidth,
    int screenHeight,
    float aspect
) const
{
    if (screenWidth <= 0 || screenHeight <= 0)
        throw std::invalid_argument("Invalid screen size");

    // 1. 화면 좌표 -> NDC
    float ndcX =
        2.0f * static_cast<float>(mouseX) /
        screenWidth - 1.0f;

    float ndcY =
        1.0f - 2.0f * static_cast<float>(mouseY) /
        screenHeight;

    // 2. 현재 View / Projection 행렬
    Mat4 view = GetViewMatrix();
    Mat4 projection = GetProjectionMatrix(aspect);

    // 3. View 행렬에서 카메라 좌표축 추출
    // LookAt으로 만든 오른손 직교 View 행렬 기준
    Vec3 right(
        view(0, 0),
        view(0, 1),
        view(0, 2)
    );

    Vec3 up(
        view(1, 0),
        view(1, 1),
        view(1, 2)
    );

    Vec3 forward(
        -view(2, 0),
        -view(2, 1),
        -view(2, 2)
    );

    // 4. 카메라 공간 Ray를 월드 방향으로 변환
    Vec3 direction =
        forward +
        right * (ndcX / projection(0, 0)) +
        up * (ndcY / projection(1, 1));

    Ray ray;

    // Camera의 실제 위치 멤버를 사용
    ray.origin = position;
    ray.direction = direction.Normalized();

    return ray;
}
