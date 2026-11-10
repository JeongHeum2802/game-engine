#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Shader.h"

#include "Math/Vec3.h"
#include "Math/Vec4.h"
#include "Math/Mat4.h"
#include "Math/Transform.h"

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

    int width, height;

    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glfwSetFramebufferSizeCallback(
        window,
        framebuffer_size_callback
    );

    // 1. 삼각형 정점 데이터
    float vertices[] = {
        -0.3f, -0.3f, 0.0f,
        0.3f, -0.3f, 0.0f,
        0.0f, 0.5f, 0.0f
    };

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
        sizeof(vertices),
        vertices,
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


        double lastTime = glfwGetTime();

        // 메인 루프
        while (!glfwWindowShouldClose(window))
        {
            double currentTime = glfwGetTime();
            double deltaTime = currentTime - lastTime;
            lastTime = currentTime;

            // 화면 초기화
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // 셰이더 활성화
            shader.Use();

            Mat4 model;
          
            model *= Transform::RotateZ(static_cast<float>(glfwGetTime()));
            model *= Transform::Translate(0.5f, 0.0f, 0.0f);
            model *= Transform::Scale(0.5f, 0.5f, 0.5f);
            
            // 모델 행렬을 셰이더에 전달
            shader.SetMat4("uModel", model);

            // 정점 데이터 설정 활성화
            glBindVertexArray(VAO);

            // 그리기
            glDrawArrays(GL_TRIANGLES, 0, 3);

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
