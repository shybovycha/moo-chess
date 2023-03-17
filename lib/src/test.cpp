#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "Game.hpp"

TEST(FENTest, BoardParsing) {
    auto game = std::make_unique<Game>();

    game->parseFEN("rbnqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RBNQKBNR w KQkq - 0 1");

    game->pieces[1][4] = NONE;
    game->pieces[3][4] = WHITE_PAWN;

    EXPECT_EQ(game->serializeAsFEN(), "rbnqkbnr/pppp1ppp/8/4P3/8/8/PPPPPPPP/RBNQKBNR w - - 0 1");
}

int main() {
    RUN_ALL_TESTS();
}
