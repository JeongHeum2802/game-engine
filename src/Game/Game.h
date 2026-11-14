#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "Math/Vec3.h"
#include "Camera/FollowCamera.h"
#include "Scene/Light.h"
#include "Graphics/Mesh.h"
#include "Graphics/Model.h"
#include "Graphics/Animation/AnimatedModel.h"
#include "Graphics/Animation/AnimatedMesh.h"
#include "Graphics/Animation/Animator.h"
#include "Game/GameObject.h"
#include "Game/Player.h"
#include "Game/Zombie.h"
#include "Game/ZombieSpawner.h"
#include "Game/Weapon.h"
#include "Game/BulletEffects.h"
#include "Audio/AudioManager.h"

class Game
{
public:
    explicit Game(GLFWwindow* window);

    void ProcessInput();
    void Update(float deltaTime, double currentTime);
    void Render();

private:
    void UpdateZombies(float deltaTime, double currentTime);
    void UpdateAimAndShooting(double currentTime);
    void FireShot(const Vec3& aimPosition, double currentTime);
    void RemoveDeadZombies();
    void UpdateSpawning(float deltaTime);
    void SpawnZombie(const Vec3& position);

    GLFWwindow* window = nullptr; // 소유하지 않음

    // GL 리소스는 glfwDestroyWindow()보다 먼저 소멸해야 함.
    Shader shader;
    Shader effectShader;
    FollowCamera camera;

    // Mesh/Model을 참조하는 GameObject보다 먼저 선언해 수명을 보장.
    std::unique_ptr<Mesh> cubeMesh;
    GameObject ground;

    Model playerModel;
    Player player;

    Model zombieModel;
    std::vector<std::unique_ptr<Zombie>> zombies;

    ZombieSpawner spawner;
    Weapon weapon;
    AudioManager audio;
    BulletEffects bulletEffects;
    DirectionalLight sun;

    static constexpr std::size_t MAX_ZOMBIES = 20;
    int killCount = 0;
    float aspect = 16.0f / 9.0f;

    std::unique_ptr<Shader> skinnedShader;

    std::unique_ptr<AnimatedModel> animatedPlayerModel;
    std::unique_ptr<Animator> playerAnimator;

    std::vector<std::unique_ptr<AnimatedMesh>>
        animatedPlayerMeshes;

    enum class PlayerAnimationState
    {
        Idle,
        Walk,
        Run,
        Shoot
    };

    PlayerAnimationState animationState =
        PlayerAnimationState::Idle;

    // 이번 프레임에 실제 총격이 발생했는지
    bool shotTriggered = false;

    void UpdatePlayerAnimation(float deltaTime);
    void RenderAnimatedPlayer(
        const Mat4& view,
        const Mat4& projection
    );
};
