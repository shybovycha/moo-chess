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

TEST(MoveTest, ParsingMove) {
    auto game = std::make_unique<Game>();

    // auto move = game->parseMove("e4");

    // EXPECT_EQ(move, Move{ .piece = WHITE_PAWN, .from = Position{ .row = 2, .col = 'e' }, .to = Position{ .row = 4, .col = 'e' } });

    // EXPECT_EQ(game->serializeAsFEN(), "rbnqkbnr/pppp1ppp/8/4P3/8/8/PPPPPPPP/RBNQKBNR w - - 0 1");
}

TEST(MoveTest, ValidatingMove) {
    auto game = std::make_unique<Game>();

    // game->applyMove(game->parseMove("e4"));
    EXPECT_EQ(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 4, .col = 'e' } }), true);
}

TEST(MoveTest, ApplyingMove) {
    auto game = std::make_unique<Game>();

    // game->applyMove(game->parseMove("e4"));
    game->applyMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 4, .col = 'e' } });

    EXPECT_EQ(game->serializeAsFEN(), "rbnqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RBNQKBNR w - - 0 1");
}

int main() {
    RUN_ALL_TESTS();
}
