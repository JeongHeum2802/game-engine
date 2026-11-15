#pragma once

#include <string>

#include "AnimatedModelResource.h"
#include "Animator.h"
#include "Shader.h"
#include "Math/Mat4.h"

class AnimatedCharacter
{
public:
    explicit AnimatedCharacter(
        const AnimatedModelResource& resource
    );

    // 애니메이션 선택
    bool Play(const std::string& animationName, bool loop = true);

    // 현재 애니메이션 처음부터 재생
    void Restart();

    // 애니메이션 시간 진행
    void Update(float deltaTime);

    // 현재 포즈로 캐릭터 렌더링
    void Render(Shader& shader, const Mat4& modelMatrix) const;

    // 1회 재생 애니메이션 종료 여부
    bool IsFinished() const;

private:
    // GPU 메시와 모델 데이터를 공유
    const AnimatedModelResource& resource;

    // 캐릭터마다 독립적인 Animator
    Animator animator;
};
