#include "GameScene.hpp"

void GameScene::load()
{
    loadTextures();
}

void GameScene::startNewGame(PieceColor playerColor)
{
    currentPlayer = playerColor;
    flipBoard = currentPlayer == PieceColor::BLACK;

    game = Board();
}

void GameScene::loadTextures()
{
    piece_textures = {
        {{PieceType::BISHOP, PieceColor::BLACK}, IMG_LoadTexture(renderer, "assets/b_bishop.png")},
        {{PieceType::KING, PieceColor::BLACK}, IMG_LoadTexture(renderer, "assets/b_king.png")},
        {{PieceType::KNIGHT, PieceColor::BLACK}, IMG_LoadTexture(renderer, "assets/b_knight.png")},
        {{PieceType::PAWN, PieceColor::BLACK}, IMG_LoadTexture(renderer, "assets/b_pawn.png")},
        {{PieceType::QUEEN, PieceColor::BLACK}, IMG_LoadTexture(renderer, "assets/b_queen.png")},
        {{PieceType::ROOK, PieceColor::BLACK}, IMG_LoadTexture(renderer, "assets/b_rook.png")},
        {{PieceType::BISHOP, PieceColor::WHITE}, IMG_LoadTexture(renderer, "assets/w_bishop.png")},
        {{PieceType::KING, PieceColor::WHITE}, IMG_LoadTexture(renderer, "assets/w_king.png")},
        {{PieceType::KNIGHT, PieceColor::WHITE}, IMG_LoadTexture(renderer, "assets/w_knight.png")},
        {{PieceType::PAWN, PieceColor::WHITE}, IMG_LoadTexture(renderer, "assets/w_pawn.png")},
        {{PieceType::QUEEN, PieceColor::WHITE}, IMG_LoadTexture(renderer, "assets/w_queen.png")},
        {{PieceType::ROOK, PieceColor::WHITE}, IMG_LoadTexture(renderer, "assets/w_rook.png")}};

    auto kingCheckHighlightTexture = IMG_LoadTexture(renderer, "assets/check_king.png");

    white_kingCheckHighlightTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 60, 60);
    black_kingCheckHighlightTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 60, 60);

    SDL_SetTextureBlendMode(white_kingCheckHighlightTexture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, white_kingCheckHighlightTexture);
    SDL_RenderCopy(renderer, kingCheckHighlightTexture, nullptr, nullptr);
    SDL_RenderCopy(renderer, piece_textures[std::make_pair(PieceType::KING, PieceColor::WHITE)], nullptr, nullptr);

    SDL_SetTextureBlendMode(black_kingCheckHighlightTexture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, black_kingCheckHighlightTexture);
    SDL_RenderCopy(renderer, kingCheckHighlightTexture, nullptr, nullptr);
    SDL_RenderCopy(renderer, piece_textures[std::make_pair(PieceType::KING, PieceColor::BLACK)], nullptr, nullptr);

    SDL_SetRenderTarget(renderer, nullptr);

    SDL_DestroyTexture(kingCheckHighlightTexture);
}

void GameScene::renderMoveHistory()
{
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);

    ImGui::BeginChild("Move history", ImVec2(150, 260), ImGuiChildFlags_Border, ImGuiWindowFlags_None);

    if (ImGui::BeginTable("split", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings))
    {
        for (auto i = 0; i < moveHistory.size(); ++i)
        {
            const auto &move = moveHistory.at(i);

            if (i % 2 == 0)
            {
                ImGui::TableNextColumn();
                ImGui::Text("%d", i + 1);

                std::string whiteStr = std::format("{0}", move);
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
}

void GameScene::renderBoard()
{
    ImGui::SameLine();

    ImGui::BeginChild("Board", ImVec2(500, 500));

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
            renderSquare(row, col);

            if (col < 7)
            {
                ImGui::SameLine();
            }
            else
            {
                auto labelBackgroundColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)labelBackgroundColor);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)labelBackgroundColor);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)labelBackgroundColor);
                ImGui::Button(std::format("{0}", row + 1).c_str(), ImVec2(20, 60));
                ImGui::PopStyleColor(3);
            }

            if (!ImGui::GetDragDropPayload() && draggingPiece != std::nullopt)
            {
                selectedPiece = {};
                draggingPiece = {};
            }
        }
    }

    ImGui::NewLine();

    for (int col = 0; col < 8; ++col)
    {
        auto labelBackgroundColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)labelBackgroundColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)labelBackgroundColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)labelBackgroundColor);
        ImGui::Button(std::format("{0:c}", static_cast<char>('A' + col)).c_str(), ImVec2(60, 20));
        ImGui::PopStyleColor(3);
    }

    // gap between buttons
    ImGui::PopStyleVar(1);

    ImGui::EndChild();
}

void GameScene::renderSquare(int row, int col)
{
    Position square_position{row + 1, static_cast<char>('a' + col)};

    const Piece *piece = game->getPieceAt(square_position);

    auto text = std::format("{0}", square_position);

    ImGui::PushID(text.c_str());

    auto square_color = ((row + col) % 2 == 0)
                            ? ImColor(173 / 255.f, 138 / 255.f, 104 / 255.f)  // dark square
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

    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)square_color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)square_color);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)square_color);

    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(1.0f, 1.0f));

    if (piece)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

        auto tint = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

        if ((draggingPiece != std::nullopt && draggingPiece->position == square_position) || (selectedPiece != std::nullopt && selectedPiece->position == square_position))
        {
            tint = ImVec4(1.0f, 1.0f, 1.0f, 0.25f);
        }

        auto piece_texture = piece_textures[std::make_tuple(piece->type, piece->color)];

        if (piece->type == PieceType::KING && game->isKingInCheck(*piece))
        {
            if (piece->color == PieceColor::WHITE)
            {
                piece_texture = white_kingCheckHighlightTexture;
            }
            else
            {
                piece_texture = black_kingCheckHighlightTexture;
            }
        }

        if (ImGui::ImageButton(text.c_str(), piece_texture, ImVec2(60, 60), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), (ImVec4)square_color, tint))
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
                draggingPiece = {};
            }
            else // TODO: testing // if (piece->color == currentPlayer)
            {
                selectedPiece = Piece{piece->type, piece->color, piece->position, piece->hasMoved, piece->justMadeDoubleMove};
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
            draggingPiece = {};
        }
    }

    ImGui::PopStyleVar();

    ImGui::PopStyleColor(4);

    if (draggingPiece != std::nullopt)
    {
        ImGui::SetNextWindowPos(ImVec2(imgui_io->MousePos.x - 30.0f, imgui_io->MousePos.y - 30.0f));
        ImGui::SetNextWindowSize(ImVec2(60.0f, 60.0f));

        ImGui::Begin("##x_tooltip_x_00", nullptr, ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

        ImGui::PushStyleColor(ImGuiCol_PopupBg, (ImVec4)ImColor(0.0f, 0.0f, 0.0f, 1.0f));
        ImGui::Image(piece_textures[std::make_tuple(draggingPiece->type, draggingPiece->color)], ImVec2(60, 60), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
        ImGui::PopStyleColor();

        ImGui::PopStyleVar();

        ImGui::End();
    }

    // Our buttons are both drag sources and drag targets
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoPreviewTooltip) && piece) // TODO: testing // && piece->color == currentPlayer)
    {
        ImGui::SetDragDropPayload("DND_TARGET_POS", &square_position, sizeof(Position), ImGuiCond_Always);

        if (draggingPiece == std::nullopt)
        {
            draggingPiece = Piece{piece->type, piece->color, piece->position, piece->hasMoved, piece->justMadeDoubleMove};
        }

        ImGui::EndDragDropSource();
    }

    ImGui::PushStyleColor(ImGuiCol_DragDropTarget, ImVec4(0.f, 0.f, 0.f, 0.f));

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("DND_TARGET_POS"))
        {
            IM_ASSERT(payload->DataSize == sizeof(Position));

            Position from_pos = *(const Position *)payload->Data;

            const Piece *src_piece = game->getPieceAt(from_pos);

            if (src_piece)
            {
                if (game->isValidMove(*src_piece, square_position))
                {
                    moveHistory.push_back(game->moveToStr(*src_piece, square_position));
                    game->applyMove(*src_piece, square_position);
                }
                else
                {
                    std::println("{0}{1} is invalid", *src_piece, square_position);
                }

                selectedPiece = {};
                draggingPiece = {};
            }
        }

        ImGui::EndDragDropTarget();
    }

    ImGui::PopStyleColor();

    ImGui::PopID();

    if (piece && piece->type == PieceType::PAWN)
    {
        if ((piece->color == PieceColor::WHITE && row == 7) || (piece->color == PieceColor::BLACK && row == 0))
        {
            // draw promotion selector
            ImGui::Begin("Promote white pawn", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

            if (ImGui::ImageButton("promote to rook", piece_textures[std::make_tuple(PieceType::ROOK, piece->color)], ImVec2(60, 60), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f)))
            {
                game->removePieceAt(square_position);
                game->setPieceAt(Piece{PieceType::ROOK, piece->color, square_position, false, false}, square_position);
            }

            ImGui::SameLine();

            if (ImGui::ImageButton("promote to knight", piece_textures[std::make_tuple(PieceType::KNIGHT, piece->color)], ImVec2(60, 60), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f)))
            {
                game->removePieceAt(square_position);
                game->setPieceAt(Piece{PieceType::KNIGHT, piece->color, square_position, false, false}, square_position);
            }

            ImGui::SameLine();

            if (ImGui::ImageButton("promote to bishop", piece_textures[std::make_tuple(PieceType::BISHOP, piece->color)], ImVec2(60, 60), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f)))
            {
                game->removePieceAt(square_position);
                game->setPieceAt(Piece{PieceType::BISHOP, piece->color, square_position, false, false}, square_position);
            }

            ImGui::SameLine();

            if (ImGui::ImageButton("promote to queen", piece_textures[std::make_tuple(PieceType::QUEEN, piece->color)], ImVec2(60, 60), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f)))
            {
                game->removePieceAt(square_position);
                game->setPieceAt(Piece{PieceType::QUEEN, piece->color, square_position, false, false}, square_position);
            }

            ImGui::PopStyleVar();

            ImGui::End();
        }
    }
}

void GameScene::renderGameControls()
{
    ImGui::BeginChild("Game controls", ImVec2(200, 100));

    ImGui::Text("You play as %s", "black");

    if (ImGui::Button("Flip the board"))
    {
        flipBoard = !flipBoard;
    }

    if (ImGui::Button("Resign"))
    {
        resign();
    }

    ImGui::SameLine();

    if (ImGui::Button("Suggest draw"))
    {
        suggestDraw();
    }

    ImGui::EndChild();
}

void GameScene::render()
{
    ImGui::Begin("Game", nullptr, ImGuiWindowFlags_NoCollapse);

    // ImGui::ShowDemoWindow(nullptr);

    renderMoveHistory();

    renderBoard();

    renderGameControls();

    ImGui::End();
}
