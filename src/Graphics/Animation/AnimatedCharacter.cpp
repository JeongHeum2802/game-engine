
#include "AnimatedCharacter.h"

AnimatedCharacter::AnimatedCharacter(
    const AnimatedModelResource& resource
)
    : resource(resource),
      animator(resource.GetModel())
{
}

bool AnimatedCharacter::Play(
    const std::string& animationName,
    bool loop
)
{
    return animator.Play(animationName, loop);
}

void AnimatedCharacter::Restart()
{
    animator.Restart();
}

void AnimatedCharacter::Update(float deltaTime)
{
    animator.Update(deltaTime);
}

void AnimatedCharacter::Render(
    Shader& shader,
    const Mat4& modelMatrix
) const
{
    shader.Use();

    // 캐릭터의 월드 변환 행렬
    shader.SetMat4("uModel", modelMatrix);

    // 본 행렬 전달 및 Mesh 렌더링
    resource.Draw(
        shader,
        animator.GetFinalBoneMatrices()
    );
}

bool AnimatedCharacter::IsFinished() const
{
    return animator.IsFinished();
}
