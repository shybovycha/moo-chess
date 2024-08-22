#include "MainMenuScene.hpp"

void MainMenuScene::load()
{
}

void MainMenuScene::render()
{
    ImGui::Begin("The game of chess", nullptr, ImGuiWindowFlags_NoCollapse);

    static std::vector<TimeMode> time_modes{
        {
            {1, 0, "Bullet"},
            {1, 1, "Bullet"},
            {2, 1, "Bullet"},
            {3, 0, "Blitz"},
            {3, 2, "Blitz"},
            {5, 0, "Blitz"},
            {10, 0, "Rapid"},
            {15, 10, "Rapid"},
            {30, 0, "Rapid"},
        }};

    if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Quick match"))
        {
            ImGui::PushFont(font_opensans_36px);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5));

            for (auto i = 0; i < time_modes.size(); ++i)
            {
                auto time_mode = time_modes[i];

                if (ImGui::Button(std::format("{0}:{1}", time_mode.time_limit_min, time_mode.time_increment_sec).c_str(), ImVec2(90, 90)))
                {
                    startQuickMatch(time_mode.time_limit_min, time_mode.time_increment_sec);
                }

                if ((i + 1) % 3 != 0)
                {
                    ImGui::SameLine();
                }
            }

            ImGui::PopStyleVar();
            ImGui::PopFont();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Lobby"))
        {
            ImGui::Text("TODO");

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}
