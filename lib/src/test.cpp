#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "Game.hpp"

int main() {
    auto game = std::make_unique<Game>();

    game->parseFEN("rbnqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RBNQKBNR w KQkq -");

    game->pieces[1][4] = NONE;
    game->pieces[3][4] = WHITE_PAWN;

    std::cout << game->serializeAsFEN();
}
