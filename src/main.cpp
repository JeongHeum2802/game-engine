#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

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

    glViewport(0, 0, 800, 600);

    // 메인 루프
    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)
            == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        // 화면 초기화
        glClearColor(0.1f, 0.15f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 화면 출력
        glfwSwapBuffers(window);

        // 이벤트 처리
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
