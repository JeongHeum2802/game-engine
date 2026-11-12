#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Shader.h"


#include "Math/Vec3.h"
#include "Math/Vec4.h"
#include "Math/Mat4.h"
#include "Math/Transform.h"

#include "Camera/Camera.h"
#include "Camera/FollowCamera.h"

#include "Graphics/Primitive.h"

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
        800, 600, "OpenGL Game", nullptr, nullptr
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
            "shaders/triangle.vert",
            "shaders/triangle.frag"
        );

        Camera camera;

        double lastTime = glfwGetTime();

        auto quad = Primitive::CreateQuad();
        auto cube = Primitive::CreateCube();
        auto sphere = Primitive::CreateSphere();

        // 메인 루프
        while (!glfwWindowShouldClose(window))
        {
            double currentTime = glfwGetTime();
            double deltaTime = currentTime - lastTime;
            lastTime = currentTime;

            // 화면 초기화
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            float aspect = static_cast<float>(width) / height;

            Mat4 model;
            model *= Transform::RotateY(static_cast<float>(currentTime));
            model *= Transform::Scale(1.3f, 1.0f, 1.0f);
            camera.SetPosition(Vec3(0, 3.0f, -3.0f));
            Mat4 view = camera.GetViewMatrix();
            Mat4 projection = camera.GetProjectionMatrix(aspect);

            // 셰이더 활성화
            shader.Use();

            shader.SetMat4("uModel", model);
            shader.SetMat4("uView", view);
            shader.SetMat4("uProjection", projection);

            //quad->Draw();
            //cube->Draw();
            sphere->Draw();

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
