#include <iostream>
#include <format>
#include <string>

#include <SDL.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

enum class ApplicationState {
    UNKNOWN = 0,
    NO_CURRENT_GAME,
    CONFIGURE_NEW_GAME,
    CONFIGURE_GAME_SEARCH,
    SEARCHING_FOR_GAME,
    PLAYING,
    GAME_OVER,
    QUIT,
};

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
       std::cerr << std::format("Error: {0}\n", SDL_GetError());
       return -1;
    }

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("SDL2+SDL_Renderer example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);

    if (window == nullptr)
    {
        std::cerr << std::format("Error: SDL_CreateWindow(): {0}\n", SDL_GetError());
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    if (renderer == nullptr)
    {
        SDL_Log("Error creating SDL_Renderer!");
        return 0;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); // (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    ImFont* font_opensans = io.Fonts->AddFontFromFileTTF("assets/OpenSans-Light.ttf", 18.0f);

    if (font_opensans == nullptr)
    {
        std::cerr << std::format("Could not load font\n");
    }

    ApplicationState state = ApplicationState::NO_CURRENT_GAME;

    while (state != ApplicationState::QUIT)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            if (event.type == SDL_QUIT)
            {
                state = ApplicationState::QUIT;
            }

            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
            {
                state = ApplicationState::QUIT;
            }
        }

        // main loop
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        if (state == ApplicationState::NO_CURRENT_GAME)
        {

            ImGui::Begin("The game of chess", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

            // static bool demo_open = true;
            // ImGui::ShowDemoWindow(&demo_open);

            ImGui::PushItemWidth(-100.f);
            if (ImGui::Button("Find a game"))
            {
                state = ApplicationState::CONFIGURE_GAME_SEARCH;
            }

            if (ImGui::Button("Create a new game"))
            {
                state = ApplicationState::CONFIGURE_NEW_GAME;
            }

            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(240.f / 255.f, 123.f / 255.f, 93.f / 255.f));

            if (ImGui::Button("Quit"))
            {
                state = ApplicationState::QUIT;
            }

            ImGui::PopStyleColor();

            ImGui::PopItemWidth();

            ImGui::End();
        }

        if (state == ApplicationState::CONFIGURE_GAME_SEARCH)
        {
            ImGui::Begin("Find a game", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

            // static int elo = 1000;
            // ImGui::InputInt("Your ELO", &elo);

            static char str0[128] = "";
            ImGui::InputTextWithHint("", "Your name", str0, IM_ARRAYSIZE(str0));

            ImGui::Text("Find a game to play as");
            static int player_color_idx = 0;
            ImGui::RadioButton("Black", &player_color_idx, 0); ImGui::SameLine();
            ImGui::RadioButton("White", &player_color_idx, 1); ImGui::SameLine();
            ImGui::RadioButton("Random", &player_color_idx, 2);

            if (ImGui::Button("Create")) {}

            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(240.f / 255.f, 123.f / 255.f, 93.f / 255.f));

            if (ImGui::Button("Cancel"))
            {
                state = ApplicationState::NO_CURRENT_GAME;
            }

            ImGui::PopStyleColor();

            ImGui::End();
        }

        if (state == ApplicationState::CONFIGURE_NEW_GAME)
        {
            ImGui::Begin("Create a game", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

            static char player_name[128] = "";
            ImGui::InputTextWithHint("", "Your name", player_name, IM_ARRAYSIZE(player_name));

            // static int time_limit = 5;
            // ImGui::InputInt("Time limit (min)", &time_limit);

            ImGui::Text("Create a game to play as");
            static int player_color_idx = 0;
            ImGui::RadioButton("Black", &player_color_idx, 0); ImGui::SameLine();
            ImGui::RadioButton("White", &player_color_idx, 1); ImGui::SameLine();
            ImGui::RadioButton("Random", &player_color_idx, 2);

            if (ImGui::Button("Create")) {}

            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(240.f / 255.f, 123.f / 255.f, 93.f / 255.f));

            if (ImGui::Button("Cancel"))
            {
                state = ApplicationState::NO_CURRENT_GAME;
            }

            ImGui::PopStyleColor();

            ImGui::End();
        }

        static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        ImGui::Render();
        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
