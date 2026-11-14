#include "Game/Game.h"

#include <algorithm>
#include <iostream>

#include <glad/glad.h>
#include "Graphics/Primitive.h"
#include "Game/Collision.h"
#include "Math/Ray.h"

Game::Game(GLFWwindow* window)
    : window(window),
      shader("shaders/standard.vert", "shaders/standard.frag"),
      effectShader("shaders/effect.vert", "shaders/effect.frag"),
      cubeMesh(Primitive::CreateCube()),
      ground(cubeMesh.get()),
      playerModel("assets/models/character.glb"),
      player(playerModel),
      zombieModel("assets/models/zombie.glb"),
      bulletEffects(cubeMesh.get())
{
    camera.SetOffset(Vec3(0.0f, 15.0f, 9.0f));

    ground.position = Vec3(0.0f, -0.25f, 0.0f);
    ground.scale = Vec3(40.0f, 0.5f, 40.0f);
    ground.color = Vec3(0.8f, 0.8f, 0.8f);

    player.position = Vec3(0.0f, 0.5f, 0.0f);
    player.scale = Vec3(0.8f, 1.0f, 0.8f);
    player.color = Vec3(0.4f, 0.7f, 1.0f); // 원본 색상을 원하면 (1,1,1)

    sun.direction = Vec3(0.5f, -1.0f, 0.0f);
    sun.color = Vec3(1.0f, 1.0f, 1.0f);
    sun.intensity = 0.9f;

    audio.SetVolume(0.7f);
    camera.Update(player.GetPosition());
}

void Game::ProcessInput()
{
    player.moveDirection = Vec3(0.0f, 0.0f, 0.0f);

    if (player.IsDead())
        return;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        player.moveDirection.z -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        player.moveDirection.z += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        player.moveDirection.x -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        player.moveDirection.x += 1.0f;
}

void Game::Update(float deltaTime, double currentTime)
{
    if (!player.IsDead())
    {
        player.Update(deltaTime);
        UpdateZombies(deltaTime, currentTime);
    }

    camera.Update(player.GetPosition());

    if (!player.IsDead())
        UpdateAimAndShooting(currentTime);

    RemoveDeadZombies();

    if (!player.IsDead())
        UpdateSpawning(deltaTime);

    // 게임 오버 후에도 이미 생성된 이펙트는 사라질 때까지 갱신.
    bulletEffects.Update(deltaTime);
}

void Game::UpdateZombies(float deltaTime, double currentTime)
{
    for (auto& zombie : zombies)
    {
        zombie->SetTarget(player.GetPosition());
        zombie->Update(deltaTime);

        if (!zombie->TryAttack(currentTime))
            continue;

        player.TakeDamage(zombie->attackDamage);
        audio.Play("player_hit.wav");

        std::cout << "Player HP: " << player.GetHealth()
                  << " / " << player.GetMaxHealth() << '\n';

        if (player.IsDead())
        {
            audio.Play("game_over.wav");
            std::cout << "GAME OVER!\n";
            break;
        }
    }
}

void Game::UpdateAimAndShooting(double currentTime)
{
    double mouseX = 0.0;
    double mouseY = 0.0;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    int windowWidth = 0;
    int windowHeight = 0;
    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

    if (windowWidth <= 0 || windowHeight <= 0 ||
        framebufferWidth <= 0 || framebufferHeight <= 0)
        return;

    aspect = static_cast<float>(framebufferWidth) /
             static_cast<float>(framebufferHeight);

    Ray mouseRay = camera.GetCamera().ScreenPointToRay(
        mouseX, mouseY, windowWidth, windowHeight, aspect);

    Vec3 aimPosition;
    if (!mouseRay.IntersectHorizontalPlane(player.position.y, aimPosition))
        return;

    player.AimAt(aimPosition);

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        FireShot(aimPosition, currentTime);
}

void Game::FireShot(const Vec3& aimPosition, double currentTime)
{
    Vec3 direction = aimPosition - player.position;
    direction.y = 0.0f;

    if (direction.Length() <= 0.0001f)
        return;

    if (!weapon.TryFire(currentTime))
        return;

    direction = direction.Normalized();
    audio.Play("gunshot.wav");

    Zombie* hitZombie = nullptr;
    float closestDistance = weapon.range;

    for (auto& zombie : zombies)
    {
        if (zombie->IsDead())
            continue;

        float hitDistance = 0.0f;
        if (RaycastCircleXZ(
                player.position, direction, zombie->position,
                zombie->collisionRadius, weapon.range, hitDistance) &&
            hitDistance <= closestDistance)
        {
            closestDistance = hitDistance;
            hitZombie = zombie.get();
        }
    }

    constexpr float muzzleOffset = 0.55f;
    Vec3 muzzlePosition = player.position + direction * muzzleOffset;
    muzzlePosition.y += 0.7f;

    const float visualDistance = hitZombie
        ? std::max(0.05f, closestDistance - muzzleOffset)
        : std::min(15.0f, weapon.range);

    bulletEffects.Spawn(
        muzzlePosition, direction, visualDistance, hitZombie != nullptr);

    if (!hitZombie)
        return;

    hitZombie->TakeDamage(weapon.damage);
    std::cout << "Zombie HP: " << hitZombie->GetHealth() << '\n';

    if (hitZombie->IsDead())
    {
        ++killCount;
        audio.Play("zombie_death.wav");
        std::cout << "Zombie killed! Total kills: " << killCount << '\n';
    }
    else
    {
        audio.Play("zombie_hit.wav");
    }
}

void Game::RemoveDeadZombies()
{
    zombies.erase(
        std::remove_if(zombies.begin(), zombies.end(),
            [](const std::unique_ptr<Zombie>& zombie)
            {
                return zombie->IsDead();
            }),
        zombies.end());
}

void Game::UpdateSpawning(float deltaTime)
{
    const bool canSpawn = zombies.size() < MAX_ZOMBIES;
    const auto spawnPosition = spawner.Update(
        deltaTime, player.GetPosition(), canSpawn);

    if (spawnPosition.has_value())
        SpawnZombie(*spawnPosition);
}

void Game::SpawnZombie(const Vec3& position)
{
    auto zombie = std::make_unique<Zombie>(zombieModel);
    zombie->position = position;
    zombie->scale = Vec3(1.0f, 1.0f, 1.0f);
    zombie->color = Vec3(1.0f, 1.0f, 1.0f);
    zombie->moveSpeed = 1.5f;
    zombie->attackRange = 1.0f;
    zombie->collisionRadius = 0.5f;
    zombies.push_back(std::move(zombie));
}

void Game::Render()
{
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    if (width <= 0 || height <= 0)
        return;

    aspect = static_cast<float>(width) / static_cast<float>(height);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.69f, 0.8f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const Mat4 view = camera.GetCamera().GetViewMatrix();
    const Mat4 projection = camera.GetCamera().GetProjectionMatrix(aspect);

    shader.Use();
    shader.SetMat4("uView", view);
    shader.SetMat4("uProjection", projection);
    shader.SetDirectionalLight("uDirectionalLight", sun);

    ground.Render(shader);
    player.Render(shader);

    for (const auto& zombie : zombies)
        zombie->Render(shader);

    effectShader.Use();
    effectShader.SetMat4("uView", view);
    effectShader.SetMat4("uProjection", projection);
    bulletEffects.Render(effectShader);
}
