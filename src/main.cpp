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

// 창 크기 변경 콜백
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void quad(int& idx, float* vertices, Vec3 a, Vec3 b, Vec3 c, Vec3 d)
{
  vertices[idx++] = a.x; vertices[idx++] = a.y; vertices[idx++] = a.z;
  vertices[idx++] = b.x; vertices[idx++] = b.y; vertices[idx++] = b.z;
  vertices[idx++] = c.x; vertices[idx++] = c.y; vertices[idx++] = c.z;

  vertices[idx++] = a.x; vertices[idx++] = a.y; vertices[idx++] = a.z;
  vertices[idx++] = c.x; vertices[idx++] = c.y; vertices[idx++] = c.z;
  vertices[idx++] = d.x; vertices[idx++] = d.y; vertices[idx++] = d.z;
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

    // 1. 정육면체 정점 데이터
    float data[6 * 2 * 3 * 3];
    int idx = 0;

    Vec3 vertices[8] = {
        Vec3(-0.5f, -0.5f, -0.5f), // 0
        Vec3( 0.5f, -0.5f, -0.5f), // 1
        Vec3( 0.5f,  0.5f, -0.5f), // 2
        Vec3(-0.5f,  0.5f, -0.5f), // 3
        Vec3(-0.5f, -0.5f,  0.5f), // 4
        Vec3( 0.5f, -0.5f,  0.5f), // 5
        Vec3( 0.5f,  0.5f,  0.5f), // 6
        Vec3(-0.5f,  0.5f,  0.5f)  // 7
    };

    quad(idx, data, vertices[0], vertices[1], vertices[2], vertices[3]);
    quad(idx, data, vertices[4], vertices[0], vertices[3], vertices[7]);
    quad(idx, data, vertices[5], vertices[4], vertices[7], vertices[6]);
    quad(idx, data, vertices[1], vertices[5], vertices[6], vertices[2]);
    quad(idx, data, vertices[3], vertices[2], vertices[6], vertices[7]);
    quad(idx, data, vertices[4], vertices[5], vertices[1], vertices[0]);

    // 2. VAO, VBO 생성
    GLuint VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // 3. VAO 바인딩
    glBindVertexArray(VAO);

    // 4. VBO에 정점 데이터 업로드
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(data),
        data,
        GL_STATIC_DRAW
    );

    // 5. 정점 속성 설정
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    // 6. 외부 쉐이더 파일 로딩
    {
        Shader shader(
            "shaders/triangle.vert",
            "shaders/triangle.frag"
        );

        Camera camera;

        double lastTime = glfwGetTime();

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
            camera.SetPosition(Vec3(0, 5.0f, -3.0f));
            Mat4 view = camera.GetViewMatrix();
            Mat4 projection = camera.GetProjectionMatrix(aspect);

            // 셰이더 활성화
            shader.Use();

            shader.SetMat4("uModel", model);
            shader.SetMat4("uView", view);
            shader.SetMat4("uProjection", projection);


            // 정점 데이터 설정 활성화 및 그리기
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // 화면 출력
            glfwSwapBuffers(window);

            // 이벤트 처리
            glfwPollEvents();
        }
    }
    // 자원 해제
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
