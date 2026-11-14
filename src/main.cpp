#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>
#include <vector>
#include <limits>
#include <algorithm>

#include "Shader.h"

#include "Math/Vec3.h"
#include "Math/Vec4.h"
#include "Math/Mat4.h"
#include "Math/Transform.h"
#include "Math/Ray.h"

#include "Camera/Camera.h"
#include "Camera/FollowCamera.h"

#include "Scene/Light.h"

#include "Graphics/Primitive.h"

#include "Game/GameObject.h"
#include "Game/Player.h"
#include "Game/Weapon.h"
#include "Game/Collision.h"
#include "Game/Zombie.h"
#include "Game/ZombieSpawner.h"
#include "Game/BulletEffects.h"

#include "Audio/AudioManager.h"


// 창 크기 변경 콜백
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{
    // GLFW 초기화
    if (!glfwInit())
    {
        std::cerr << "GLFW initialization failed\n";
        return -1;
    }

    // OpenGL 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 윈도우 생성
    GLFWwindow* window = glfwCreateWindow(
        1600, 900, "OpenGL Game", nullptr, nullptr
    );

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // GLAD 초기화
    if (!gladLoadGLLoader(
        (GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "GLAD initialization failed\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    glfwSwapInterval(1);

    int width, height;

    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glfwSetFramebufferSizeCallback(
        window,
        framebuffer_size_callback
    );

    // 6. 외부 쉐이더 파일 로딩
    {
        Shader shader(
            "shaders/standard.vert",
            "shaders/standard.frag"
        );

        Shader effectShader(
          "shaders/effect.vert",
          "shaders/effect.frag"
        );

        FollowCamera camera;
        camera.SetOffset(Vec3(0.0f, 15.0f, 9.0f));

        auto cubeMesh = Primitive::CreateCube();

        // 씬에 바닥 추가
        GameObject ground(cubeMesh.get());

        ground.position = Vec3(0.0f, -0.25f, 0.0f);
        ground.scale = Vec3(40.0f, 0.5f, 40.0f);
        ground.color = Vec3(0.8f, 0.8f, 0.8f);

        // 씬에 플레이어 추가
        Model playerModel("assets/models/character.glb");
        Player player(playerModel);

        player.position = Vec3(0.0f, 0.5f, 0.0f);
        player.scale = Vec3(0.8f, 1.0f, 0.8f);
        player.color = Vec3(0.4f, 0.7f, 1.0f);

        // 빛 추가
        DirectionalLight sun;
        sun.direction = Vec3(0.5f, -1.0f, 0.0f);
        sun.color = Vec3(1.0f, 1.0f, 1.0f);
        sun.intensity = 0.9f;

        // 총알 이펙트 추가
        BulletEffects bulletEffects(cubeMesh.get());

        // 적 추가
        std::vector<std::unique_ptr<Zombie>> zombies;

        Model zombieModel("assets/models/zombie.glb");

        auto SpawnZombie = [&](const Vec3& spawnPosition)
        {
            auto zombie = std::make_unique<Zombie>(zombieModel);

            zombie->position = spawnPosition;
            zombie->scale = Vec3(1.0f, 1.0f, 1.0f);

            zombie->moveSpeed = 1.5f;
            zombie->attackRange = 1.0f;
            zombie->collisionRadius = 0.5f;

            zombies.push_back(std::move(zombie));
        };

        ZombieSpawner spawner;

        constexpr size_t MAX_ZOMBIES = 20;

        int killCount = 0;

        Weapon weapon;  

        AudioManager audio;
        audio.SetVolume(0.7f);

        double lastTime = glfwGetTime();
        
        // 메인 루프dww
        while (!glfwWindowShouldClose(window))
        {
            double currentTime = glfwGetTime();
            double deltaTime = currentTime - lastTime;
            lastTime = currentTime;

            // 화면 초기화
            glClearColor(0.69f, 0.8f, 0.9f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            player.moveDirection = Vec3(0.0f, 0.0f, 0.0f);

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                player.moveDirection.z -= 1.0f;

            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                player.moveDirection.z += 1.0f;

            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                player.moveDirection.x -= 1.0f;

            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                player.moveDirection.x += 1.0f;
            
            player.Update(deltaTime);

            for (auto& zombie : zombies)
            {
              zombie->SetTarget(player.GetPosition());
              zombie->Update(static_cast<float>(deltaTime));

              if (zombie->TryAttack(currentTime))
              {
                  player.TakeDamage(zombie->attackDamage);
                  audio.Play("player_hit.wav");
                  std::cout
                      << "Player HP: "
                      << player.GetHealth()
                      << " / "
                      << player.GetMaxHealth()
                      << '\n';

                  if (player.IsDead())
                  {
                      std::cout << "GAME OVER!\n";
                      audio.Play("game_over.wav");
                      break;
                  }
              }
            }

            camera.Update(player.GetPosition());

            // 3. 마우스 위치 획득
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            int windowWidth, windowHeight;
            int framebufferWidth, framebufferHeight;

            glfwGetWindowSize(
                window,
                &windowWidth,
                &windowHeight
            );

            glfwGetFramebufferSize(
                window,
                &framebufferWidth,
                &framebufferHeight
            );
            float aspect = 1.0f;

            if (framebufferHeight > 0)
            {
                aspect =
                    static_cast<float>(framebufferWidth) /
                    framebufferHeight;
            }

            if (windowWidth > 0 && windowHeight > 0 &&
                framebufferWidth > 0 && framebufferHeight > 0)
            {
                // 5. 화면 좌표 -> 월드 Ray
                Ray mouseRay =
                    camera.GetCamera().ScreenPointToRay(
                        mouseX,
                        mouseY,
                        windowWidth,
                        windowHeight,
                        aspect
                    );

                // 6. Player 중심 높이의 수평 평면과 교차
                Vec3 mouseWorldPosition;

                if (mouseRay.IntersectHorizontalPlane(
                    player.position.y,
                    mouseWorldPosition
                ))
                {
                    // 7. 마우스 방향으로 회전
                    player.AimAt(mouseWorldPosition);

                    // 마우스 왼쪽 버튼으로 연속 사격
                    if (!player.IsDead() &&
                      glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
                    {
                        if (weapon.TryFire(currentTime))
                        {
                            audio.Play("gunshot.wav");

                            Vec3 bulletDirection =
                                mouseWorldPosition - player.position;

                            bulletDirection.y = 0.0f;

                            if (bulletDirection.Length() > 0.0001f)
                            {
                                bulletDirection =
                                    bulletDirection.Normalized();

                                Zombie* hitZombie = nullptr;
                                float closestDistance = weapon.range;

                                for (auto& zombie : zombies)
                                {
                                    if (zombie->IsDead())
                                        continue;

                                    float hitDistance = 0.0f;

                                    bool hit = RaycastCircleXZ(
                                        player.position,
                                        bulletDirection,
                                        zombie->position,
                                        zombie->collisionRadius,
                                        weapon.range,
                                        hitDistance
                                    );

                                    if (hit && hitDistance <= closestDistance)
                                    {
                                        closestDistance = hitDistance;
                                        hitZombie = zombie.get();
                                    }
                                }

                                // 총알 이펙트 추가

                                const float muzzleOffset = 0.55f;

                                // 원래 Raycast는 플레이어 중심에서 시작하므로
                                // 시각 효과의 이동 거리를 보정

                                bool hit = hitZombie != nullptr;
                                float visualDistance = hit
                                    ? std::max(0.05f, closestDistance - muzzleOffset)
                                    : std::min(15.0f, weapon.range);

                                Vec3 muzzlePosition =
                                    player.position +
                                    bulletDirection * muzzleOffset;

                                // 총알을 캐릭터 높이에 맞춰 배치
                                muzzlePosition.y += 0.7f;

                                // 총구 섬광 + 움직이는 궤적 + 명중 섬광
                                bulletEffects.Spawn(
                                    muzzlePosition,
                                    bulletDirection,
                                    visualDistance,
                                    hit
                                );

                                // 가장 가까운 좀비에게만 데미지 적용
                                if (hitZombie != nullptr)
                                {
                                    hitZombie->TakeDamage(weapon.damage);

                                    std::cout
                                        << "Zombie HP: "
                                        << hitZombie->GetHealth()
                                        << '\n';

                                    if (hitZombie->IsDead())
                                    {
                                        audio.Play("zombie_death.wav");
                                        ++killCount;

                                    std::cout
                                        << "Zombie killed! Total kills: "
                                        << killCount
                                        << '\n';
                                    }
                                    else 
                                    {
                                        audio.Play("zombie_hit.wav");
                                    }
                                }
                            }
                        }
                    }
                }
            }

            bulletEffects.Update(static_cast<float>(deltaTime));

            zombies.erase(
                std::remove_if(
                    zombies.begin(),
                    zombies.end(),
                    [](const std::unique_ptr<Zombie>& zombie)
                    {
                        return zombie->IsDead();
                    }
                ),
                zombies.end()
            );

            // 2. 자동 스폰
            if (!player.IsDead())
            {
                bool canSpawn = zombies.size() < MAX_ZOMBIES;

                auto spawnPosition = spawner.Update(
                    static_cast<float>(deltaTime),
                    player.GetPosition(),
                    canSpawn
                );

                if (spawnPosition.has_value())
                {
                    SpawnZombie(spawnPosition.value());
                }
            }

            Mat4 view = camera.GetCamera().GetViewMatrix();
            Mat4 projection = camera.GetCamera().GetProjectionMatrix(aspect);

            // 셰이더 활성화
            shader.Use();

            shader.SetMat4("uView", view);
            shader.SetMat4("uProjection", projection);
            shader.SetVec3("uObjectColor",Vec3(1.0f, 1.0f, 1.0f));

            shader.SetDirectionalLight("uDirectionalLight",sun);
            
            for (const auto& zombie : zombies)
            {
                if (!zombie->IsDead())
                {
                    zombie->Render(shader);
                }
            }
            

            player.Render(shader);
            ground.Render(shader);

            effectShader.Use();

            effectShader.SetMat4("uView", view);
            effectShader.SetMat4("uProjection", projection);

            // 총격 이펙트 출력
            bulletEffects.Render(effectShader);

            // 화면 출력
            glfwSwapBuffers(window);

            // 이벤트 처리
            glfwPollEvents();
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
