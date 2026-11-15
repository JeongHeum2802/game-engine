#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "UI/GameUI.h"

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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Game(Shader, Model, Mesh 등)은 GL 컨텍스트가 살아 있을 때 소멸해야 함.
    {
        GameUI ui;
        GameScreen screen = GameScreen::Title;
        std::unique_ptr<Game> game = std::make_unique<Game>(window);
      
        double lastTime = glfwGetTime();

        while (!glfwWindowShouldClose(window))
        {
            const double currentTime = glfwGetTime();
            const float deltaTime = static_cast<float>(currentTime - lastTime);
            lastTime = currentTime;

            glfwPollEvents();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            if (screen == GameScreen::Playing)
            {
                game->ProcessInput();
                game->Update(deltaTime, currentTime);
                if (game->IsGameOver())
                    screen = GameScreen::GameOver;
            }

            game->Render();

            UIAction action = ui.Render(
            screen, game->GetPlayerHealth(), 100.f, game->GetKillCount());

            // Simple keyboard shortcuts (avoid key-repeat trigger):
            if (screen == GameScreen::Title && ImGui::IsKeyPressed(ImGuiKey_Enter))
                action = UIAction::Start;
            if (screen == GameScreen::GameOver && ImGui::IsKeyPressed(ImGuiKey_R))
                action = UIAction::Restart;

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);

            if (action == UIAction::Start)
                screen = GameScreen::Playing;
            else if (action == UIAction::Restart)
            {
                // Reconstructing Game resets player, zombies, spawn timer,
                // weapon timers, kill count, animation state, etc. at once.
                game.reset();
                game = std::make_unique<Game>(window);
                screen = GameScreen::Playing;
                lastTime = glfwGetTime();
            }
        }
        game.reset();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
