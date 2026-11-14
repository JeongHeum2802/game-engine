#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Game/Game.h"

static void FramebufferSizeCallback(GLFWwindow*, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{
    if (!glfwInit())
    {
        std::cerr << "GLFW initialization failed\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        1600, 900, "OpenGL Game", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cerr << "GLAD initialization failed\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSwapInterval(1);

    // Game(Shader, Model, Mesh 등)은 GL 컨텍스트가 살아 있을 때 소멸해야 함.
    {
        Game game(window);
        double lastTime = glfwGetTime();

        while (!glfwWindowShouldClose(window))
        {
            const double currentTime = glfwGetTime();
            const float deltaTime = static_cast<float>(currentTime - lastTime);
            lastTime = currentTime;

            glfwPollEvents();
            game.ProcessInput();
            game.Update(deltaTime, currentTime);
            game.Render();
            glfwSwapBuffers(window);
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
