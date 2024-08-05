#include <iostream>
#include <format>
#include <string>
#include <print>

#include <SDL.h>

#include <SDL_image.h>

#include "imgui.h"
#include "imgui_freetype.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include "chesslib.hpp"

enum class ApplicationState
{
    UNKNOWN = 0,
    NO_CURRENT_GAME,
    CONFIGURE_NEW_GAME,
    CONFIGURE_GAME_SEARCH,
    SEARCHING_FOR_GAME,
    PLAYING,
    GAME_OVER,
    QUIT,
};

int main(int argc, char** argv)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
       std::println(stderr, "Error: {0}", SDL_GetError());
       return -1;
    }

    SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, "permonitorv2");
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("mooChess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);

    if (window == nullptr)
    {
        std::println(stderr, "Error: SDL_CreateWindow(): {0}", SDL_GetError());
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

    ImFontConfig config;
    config.FontDataOwnedByAtlas = false;
    config.RasterizerMultiply = 1.2f;
    config.GlyphOffset.y = 1.0f;
    config.PixelSnapH = true;

    // double font size, half the scaling = Multi-sample Anti-Aliasing
    io.FontGlobalScale = 0.5f;

    // Only if using FreeType with ImGui
#ifdef IMGUI_ENABLE_FREETYPE
    config.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_ForceAutoHint;
#endif

    ImFont* font_opensans = io.Fonts->AddFontFromFileTTF("assets/OpenSans-Light.ttf", 36.0f, &config);

    if (font_opensans == nullptr)
    {
        std::println(stderr, "Could not load font");
    }

    ApplicationState state = ApplicationState::NO_CURRENT_GAME;

    std::optional<Board> game;

    std::map<std::tuple<PieceType, PieceColor>, SDL_Texture*> piece_textures = {
        { { PieceType::BISHOP, PieceColor::BLACK }, IMG_LoadTexture(renderer, "assets/b_bishop.png") },
        { { PieceType::KING, PieceColor::BLACK }, IMG_LoadTexture(renderer, "assets/b_king.png") },
        { { PieceType::KNIGHT, PieceColor::BLACK }, IMG_LoadTexture(renderer, "assets/b_knight.png") },
        { { PieceType::PAWN, PieceColor::BLACK }, IMG_LoadTexture(renderer, "assets/b_pawn.png") },
        { { PieceType::QUEEN, PieceColor::BLACK }, IMG_LoadTexture(renderer, "assets/b_queen.png") },
        { { PieceType::ROOK, PieceColor::BLACK }, IMG_LoadTexture(renderer, "assets/b_rook.png") },
        { { PieceType::BISHOP, PieceColor::WHITE }, IMG_LoadTexture(renderer, "assets/w_bishop.png") },
        { { PieceType::KING, PieceColor::WHITE }, IMG_LoadTexture(renderer, "assets/w_king.png") },
        { { PieceType::KNIGHT, PieceColor::WHITE }, IMG_LoadTexture(renderer, "assets/w_knight.png") },
        { { PieceType::PAWN, PieceColor::WHITE }, IMG_LoadTexture(renderer, "assets/w_pawn.png") },
        { { PieceType::QUEEN, PieceColor::WHITE }, IMG_LoadTexture(renderer, "assets/w_queen.png") },
        { { PieceType::ROOK, PieceColor::WHITE }, IMG_LoadTexture(renderer, "assets/w_rook.png") }
    };

    bool flipBoard = false;
    PieceColor currentPlayer = PieceColor::BLACK;
    std::optional<Piece> draggingPiece = {};
    std::optional<Piece> selectedPiece = {};
    std::vector<std::string> moveHistory;

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

            ImGui::Begin("The game of chess", nullptr, ImGuiWindowFlags_NoCollapse);

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
            ImGui::Begin("Find a game", nullptr, ImGuiWindowFlags_NoCollapse);

            // static int elo = 1000;
            // ImGui::InputInt("Your ELO", &elo);

            static char str0[128] = "";
            ImGui::InputTextWithHint("", "Your name", str0, IM_ARRAYSIZE(str0));

            ImGui::Text("Find a game to play as");
            static int player_color_idx = 0;
            ImGui::RadioButton("Black", &player_color_idx, 0); ImGui::SameLine();
            ImGui::RadioButton("White", &player_color_idx, 1); ImGui::SameLine();
            ImGui::RadioButton("Random", &player_color_idx, 2);

            if (ImGui::Button("Create"))
            {
                // TODO: add server call
                
                switch (player_color_idx)
                {
                case 0:
                    currentPlayer = PieceColor::BLACK;
                    break;

                case 1:
                    currentPlayer = PieceColor::WHITE;
                    break;

                default:
                    currentPlayer = std::rand() % 2 ? PieceColor::BLACK : PieceColor::WHITE;
                }

                game = Board();

                state = ApplicationState::PLAYING;

                flipBoard = currentPlayer == PieceColor::BLACK;
            }

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
            ImGui::Begin("Create a game", nullptr, ImGuiWindowFlags_NoCollapse);

            static char player_name[128] = "";
            ImGui::InputTextWithHint("", "Your name", player_name, IM_ARRAYSIZE(player_name));

            // static int time_limit = 5;
            // ImGui::InputInt("Time limit (min)", &time_limit);

            ImGui::Text("Create a game to play as");
            static int player_color_idx = 0;
            ImGui::RadioButton("Black", &player_color_idx, 0); ImGui::SameLine();
            ImGui::RadioButton("White", &player_color_idx, 1); ImGui::SameLine();
            ImGui::RadioButton("Random", &player_color_idx, 2);

            if (ImGui::Button("Create"))
            {
                // TODO: add server call
                switch (player_color_idx)
                {
                case 0:
                    currentPlayer = PieceColor::BLACK;
                    break;

                case 1:
                    currentPlayer = PieceColor::WHITE;
                    break;

                default:
                    currentPlayer = std::rand() % 2 ? PieceColor::BLACK : PieceColor::WHITE;
                }

                game = Board();

                state = ApplicationState::PLAYING;

                flipBoard = currentPlayer == PieceColor::BLACK;

                game = Board();

                state = ApplicationState::PLAYING;
            }

            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(240.f / 255.f, 123.f / 255.f, 93.f / 255.f));

            if (ImGui::Button("Cancel"))
            {
                state = ApplicationState::NO_CURRENT_GAME;
            }

            ImGui::PopStyleColor();

            ImGui::End();
        }

        if (state == ApplicationState::PLAYING)
        {
            ImGui::Begin("Game", nullptr, ImGuiWindowFlags_NoCollapse);

            ImGui::ShowDemoWindow(nullptr);

            ImGui::BeginChild("Game options", ImVec2(200, 100));

            ImGui::Text(std::format("You play as {0}", "black").c_str());

            if (ImGui::Button("Flip the board"))
            {
                flipBoard = !flipBoard;
            }

            if (ImGui::Button("Resign"))
            {
                state = ApplicationState::NO_CURRENT_GAME;
            }

            ImGui::SameLine();

            if (ImGui::Button("Suggest draw"))
            {
                state = ApplicationState::NO_CURRENT_GAME;
            }

            ImGui::EndChild();

            // ----------
            
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);

            ImGui::BeginChild("Move history", ImVec2(150, 260), ImGuiChildFlags_Border, ImGuiWindowFlags_None);

            if (ImGui::BeginTable("split", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings))
            {
                for (auto i = 0; i < moveHistory.size(); ++i)
                {
                    const auto& move = moveHistory.at(i);

                    if (i % 2 == 0)
                    {
                        std::string whiteStr = std::format("{0}. {1}", i + 1, move);
                        ImGui::TableNextColumn();
                        ImGui::Button(whiteStr.c_str(), ImVec2(-FLT_MIN, 0.0f));
                    }
                    else
                    {
                        std::string blackStr = std::format("{0}", move);
                        ImGui::TableNextColumn();
                        ImGui::Button(blackStr.c_str(), ImVec2(-FLT_MIN, 0.0f));
                    }
                }

                ImGui::EndTable();
            }

            ImGui::EndChild();
            ImGui::PopStyleVar();
            
            // ----------

            ImGui::SameLine();

            ImGui::BeginChild("Board", ImVec2(500, 500));

            // ImGui::Text(std::format("You play as {0}", "black"));

            // gap between buttons
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

            for (auto m_row = 7; m_row >= 0; m_row--)
            {
                int row = m_row;

                if (flipBoard)
                {
                    row = 7 - m_row;
                }

                for (auto col = 0; col < 8; col++)
                {
                    Position square_position{ row + 1, static_cast<char>('a' + col) };

                    const Piece* piece = game->getPieceAt(square_position);

                    auto text = std::format("{0}", square_position);

                    ImGui::PushID(text.c_str());

                    auto square_color = ((row + col) % 2 == 0)
                        ? ImColor(173 / 255.f, 138 / 255.f, 104 / 255.f) // dark square
                        : ImColor(237 / 255.f, 219 / 255.f, 185 / 255.f); // light square

                    if ((draggingPiece != std::nullopt && game->isValidMove(*draggingPiece, square_position)) || (selectedPiece != std::nullopt && game->isValidMove(*selectedPiece, square_position)))
                    {
                        if ((row + col) % 2 == 0)
                        {
                            square_color = ImColor(137 / 255.f, 140 / 255.f, 104 / 255.f, 0.8f); // dark square highlight
                        }
                        else
                        {
                            square_color = ImColor(173 / 255.f, 178 / 255.f, 104 / 255.f, 0.8f); // light square highlight
                        }
                    }

                    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4) square_color);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4) square_color);
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4) square_color);

                    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(1.0f, 1.0f));

                    if (piece)
                    {
                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

                        auto tint = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

                        if ((draggingPiece != std::nullopt && draggingPiece->position == square_position) || (selectedPiece != std::nullopt && selectedPiece->position == square_position))
                        {
                            tint = ImVec4(1.0f, 1.0f, 1.0f, 0.25f);
                        }

                        if (ImGui::ImageButton(text.c_str(), piece_textures[std::make_tuple(piece->type, piece->color)], ImVec2(60, 60), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), (ImVec4) square_color, tint))
                        {
                            // TODO: check current player turn
                            if (draggingPiece != std::nullopt)
                            {
                                draggingPiece = {};
                            }

                            if (selectedPiece != std::nullopt)
                            {
                                if (selectedPiece->position != square_position)
                                {
                                    if (game->isValidMove(*selectedPiece, square_position))
                                    {
                                        moveHistory.push_back(game->moveToStr(*selectedPiece, square_position));
                                        game->applyMove(*selectedPiece, square_position);
                                    }
                                }

                                selectedPiece = {};
                            }
                            else if (piece->color == currentPlayer)
                            {
                                selectedPiece = Piece{ piece->type, piece->color, piece->position, piece->hasMoved, piece->justMadeDoubleMove };
                            }
                        }

                        ImGui::PopStyleVar();
                    }
                    else
                    {
                        if (ImGui::Button("", ImVec2(60, 60)))
                        {
                            if (selectedPiece != std::nullopt && game->isValidMove(*selectedPiece, square_position))
                            {
                                moveHistory.push_back(game->moveToStr(*selectedPiece, square_position));
                                game->applyMove(*selectedPiece, square_position);
                            }

                            selectedPiece = {};
                        }
                    }

                    ImGui::PopStyleVar();

                    ImGui::PopStyleColor(3);

                    // Our buttons are both drag sources and drag targets
                    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoPreviewTooltip) && piece->color == currentPlayer)
                    {
                        ImGui::SetDragDropPayload("DND_TARGET_POS", &square_position, sizeof(Position), ImGuiCond_FirstUseEver);

                        ImGui::SetNextWindowPos(ImVec2(io.MousePos.x - 30.0f, io.MousePos.y - 30.0f));

                        ImGui::Begin("##x_tooltip_x_00", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

                        if (piece)
                        {
                            if (draggingPiece == std::nullopt)
                            {
                                draggingPiece = Piece{ piece->type, piece->color, piece->position, piece->hasMoved, piece->justMadeDoubleMove };
                            }

                            ImGui::PushStyleColor(ImGuiCol_PopupBg, (ImVec4) ImColor(0.0f, 0.0f, 0.0f, 1.0f));
                            ImGui::Image(piece_textures[std::make_tuple(piece->type, piece->color)], ImVec2(60, 60), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
                            ImGui::PopStyleColor();
                        }

                        ImGui::PopStyleVar();

                        ImGui::End();

                        ImGui::EndDragDropSource();
                    }

                    if (ImGui::BeginDragDropTarget())
                    {
                        // the number of frames is messed up because of non-standard board layout (in terms of UI elements), so this will always be untagged payload
                        if (const ImGuiPayload* payload = ImGui::GetDragDropPayload())
                        {
                            // TODO: add strcmp for payload->DataType, "DND_TARGET_POS"

                            IM_ASSERT(payload->DataSize == sizeof(Position));
                            Position from_pos = *(const Position*)payload->Data;

                            std::println(">> drop");

                            if (game->isValidMove(*game->getPieceAt(from_pos), square_position))
                            {
                                moveHistory.push_back(game->moveToStr(*game->getPieceAt(from_pos), square_position));
                                game->applyMove(*game->getPieceAt(from_pos), square_position);
                            }
                            else
                            {
                                std::println("{0}{1} is invalid", *game->getPieceAt(from_pos), square_position);
                            }

                            draggingPiece = {};
                        }

                        ImGui::EndDragDropTarget();
                    }

                    ImGui::PopID();

                    if (col < 7)
                    {
                        ImGui::SameLine();
                    }
                    else
                    {
                        auto labelBackgroundColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
                        ImGui::SameLine();
                        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4) labelBackgroundColor);
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4) labelBackgroundColor);
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4) labelBackgroundColor);
                        ImGui::Button(std::format("{0}", row + 1).c_str(), ImVec2(20, 60));
                        ImGui::PopStyleColor(3);
                    }

                    if (!ImGui::GetDragDropPayload() && draggingPiece != std::nullopt)
                    {
                        draggingPiece = {};
                    }
                }
            }

            ImGui::NewLine();

            for (int col = 0; col < 8; ++col)
            {
                auto labelBackgroundColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4) labelBackgroundColor);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4) labelBackgroundColor);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4) labelBackgroundColor);
                ImGui::Button(std::format("{0:c}", static_cast<char>('A' + col)).c_str(), ImVec2(60, 20));
                ImGui::PopStyleColor(3);
            }

            // gap between buttons
            ImGui::PopStyleVar(1);

            ImGui::EndChild();

            ImGui::End();
        }

        static ImVec4 clear_color = { 0.45f, 0.55f, 0.60f, 1.00f };

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
