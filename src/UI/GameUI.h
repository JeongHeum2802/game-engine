#pragma once

#include <algorithm>
#include <cstdio>
#include <imgui.h>

// UI-only state. Gameplay code stays inside Game.
enum class GameScreen { Title, Playing, GameOver };
enum class UIAction { None, Start, Restart };

class GameUI
{
public:
    UIAction Render(GameScreen screen, float health, float maxHealth, int kills)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        const ImVec2 pos = viewport->Pos;
        const ImVec2 size = viewport->Size;

        if (screen == GameScreen::Playing)
        {
            DrawHUD(pos, size, health, maxHealth, kills);
            return UIAction::None;
        }

        // Dim the entire game view without making an interactive full-screen window.
        ImDrawList* background = ImGui::GetForegroundDrawList();
        background->AddRectFilled(
            pos,
            ImVec2(pos.x + size.x, pos.y + size.y),
            IM_COL32(0, 0, 0, 120));

        const bool title = screen == GameScreen::Title;
        ImGui::SetNextWindowPos(
            ImVec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f),
            ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(390.f, 245.f), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.93f);

        constexpr ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoSavedSettings;

        UIAction action = UIAction::None;
        if (ImGui::Begin("##GameOverlay", nullptr, flags))
        {
            ImGui::Dummy(ImVec2(0.f, 18.f));
            CenteredText(title ? "ZOMBIE SURVIVAL" : "GAME OVER");
            ImGui::Dummy(ImVec2(0.f, 15.f));
            if (title)
            {
                CenteredText("WASD - Move  |  Mouse - Aim / Shoot");
            }
            else
            {
                char score[64];
                std::snprintf(score, sizeof(score), "Zombies eliminated: %d", kills);
                CenteredText(score);
            }
            ImGui::Dummy(ImVec2(0.f, 18.f));

            const float width = 210.f;
            ImGui::SetCursorPosX((ImGui::GetWindowSize().x - width) * 0.5f);
            if (ImGui::Button(title ? "START GAME" : "RESTART", ImVec2(width, 44.f)))
                action = title ? UIAction::Start : UIAction::Restart;

            ImGui::Dummy(ImVec2(0.f, 6.f));
            CenteredText(title ? "Press ENTER to start" : "Press R to restart");
        }
        ImGui::End();
        return action;
    }

private:
    static void CenteredText(const char* text)
    {
        const float textWidth = ImGui::CalcTextSize(text).x;
        ImGui::SetCursorPosX(std::max(0.f, (ImGui::GetWindowSize().x - textWidth) * 0.5f));
        ImGui::TextUnformatted(text);
    }

    static void DrawHUD(const ImVec2& pos, const ImVec2& size,
                        float health, float maxHealth, int kills)
    {
        ImDrawList* draw = ImGui::GetForegroundDrawList();
        const float fraction = (maxHealth > 0.f)
            ? std::clamp(health / maxHealth, 0.f, 1.f)
            : 0.f;

        const ImVec2 barMin(pos.x + 24.f, pos.y + 44.f);
        const ImVec2 barMax(barMin.x + 230.f, barMin.y + 18.f);
        draw->AddText(ImVec2(barMin.x, barMin.y - 24.f), IM_COL32(255,255,255,255), "HEALTH");
        draw->AddRectFilled(barMin, barMax, IM_COL32(35, 36, 40, 235), 4.f);
        draw->AddRectFilled(barMin,
            ImVec2(barMin.x + 230.f * fraction, barMax.y),
            fraction > .3f ? IM_COL32(75, 205, 110, 255) : IM_COL32(220, 65, 65, 255), 4.f);
        draw->AddRect(barMin, barMax, IM_COL32(255,255,255,150), 4.f);

        char score[64];
        std::snprintf(score, sizeof(score), "KILLS  %d", kills);
        const ImVec2 textSize = ImGui::CalcTextSize(score);
        draw->AddText(ImVec2(pos.x + size.x - textSize.x - 26.f, pos.y + 20.f),
                      IM_COL32(255,255,255,255), score);
    }
};
