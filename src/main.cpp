#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

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

#include "Graphics/Model.h"

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

        FollowCamera camera;
        camera.SetOffset(Vec3(0.0f, 15.0f, 7.5f));

        auto cubeMesh = Primitive::CreateCube();

        // 씬에 바닥 추가
        GameObject ground(cubeMesh.get());

        ground.position = Vec3(0.0f, -0.25f, 0.0f);
        ground.scale = Vec3(20.0f, 0.5f, 20.0f);
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

        // PointLight pointLight1;
        // pointLight1.position = Vec3(0.0f, 2.0f, 0.0f);
        // pointLight1.color = Vec3(1.0f, 1.0f, 1.0f);

        // 적 추가
        auto enemyMesh = Primitive::CreateSphere();

        GameObject enemy(enemyMesh.get());

        enemy.position = Vec3(3.0f, 0.65f, -4.0f);
        enemy.scale = Vec3(1.3f, 1.3f, 1.3f);
        enemy.color = Vec3(0.8f, 0.2f, 0.2f);

        // Sphere 기본 반지름 0.5 * Scale 1.3
        float enemyRadius = 0.65f;

        float enemyHealth = 100.0f;
        float hitFlashTimer = 0.0f;

        Weapon weapon;  

        double lastTime = glfwGetTime();
        
        // 메인 루프
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
            camera.Update(player.GetPosition());

            // 3. 마우스 위치 획득
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            float aspect = static_cast<float>(width) / height;

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

            if (windowWidth > 0 && windowHeight > 0 &&
                framebufferWidth > 0 && framebufferHeight > 0)
            {
                float aspect =
                    static_cast<float>(framebufferWidth) /
                    framebufferHeight;

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
                    if (glfwGetMouseButton(
                        window, GLFW_MOUSE_BUTTON_LEFT
                    ) == GLFW_PRESS)
                    {
                        if (weapon.TryFire(currentTime))
                        {
                            Vec3 bulletDirection =
                                mouseWorldPosition - player.position;

                            bulletDirection.y = 0.0f;

                            if (bulletDirection.Length() > 0.0001f)
                            {
                                bulletDirection =
                                    bulletDirection.Normalized();

                                float hitDistance = 0.0f;

                                if (enemyHealth > 0.0f &&
                                    RaycastCircleXZ(
                                        player.position,
                                        bulletDirection,
                                        enemy.position,
                                        enemyRadius,
                                        weapon.range,
                                        hitDistance
                                    ))
                                {
                                    enemyHealth -= weapon.damage;

                                    if (enemyHealth < 0.0f)
                                        enemyHealth = 0.0f;

                                    hitFlashTimer = 0.12f;

                                    std::cout
                                        << "Zombie HP: "
                                        << enemyHealth << '\n';
                                }
                            }
                        }
                    }
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
            // shader.SetInt("uPointLightCount", 1);

            // 첫 번째 Point Light
            // shader.SetPointLight(
            //     "uPointLights[0]",
            //     pointLight1
            // );

            hitFlashTimer -= static_cast<float>(deltaTime);

            if (hitFlashTimer > 0.0f)
            {
                enemy.color = Vec3(1.0f, 1.0f, 1.0f);
            }
            else
            {
                enemy.color = Vec3(0.8f, 0.2f, 0.2f);
            }

            player.Render(shader);
            ground.Render(shader);

            if (enemyHealth > 0.0f)
            {
                enemy.Render(shader);
            }

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
