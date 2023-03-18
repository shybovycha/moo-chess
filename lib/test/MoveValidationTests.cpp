#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::AllOf;
using ::testing::Eq;
using ::testing::Field;
using ::testing::FieldsAre;
using ::testing::Optional;
using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::Ne;

#include "../src/Game.hpp"

TEST(ValidatingMoveTest, PawnAdvancement) {
    auto game = std::make_unique<Game>();

    EXPECT_TRUE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 3, .col = 'e' } }));
    EXPECT_TRUE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 4, .col = 'e' } }));

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 5, .col = 'e' } }));

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'a' }, .to = Position{.row = 4, .col = 'e' } }));
    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'b' }, .to = Position{.row = 4, .col = 'e' } }));
    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'c' }, .to = Position{.row = 4, .col = 'e' } }));
    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'd' }, .to = Position{.row = 4, .col = 'e' } }));
    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'f' }, .to = Position{.row = 4, .col = 'e' } }));
    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'g' }, .to = Position{.row = 4, .col = 'e' } }));
    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'h' }, .to = Position{.row = 4, .col = 'e' } }));

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'a' }, .to = Position{.row = 3, .col = 'e' } }));
    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'b' }, .to = Position{.row = 3, .col = 'e' } }));
    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'c' }, .to = Position{.row = 3, .col = 'e' } }));
    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'd' }, .to = Position{.row = 3, .col = 'e' } }));
    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'f' }, .to = Position{.row = 3, .col = 'e' } }));
    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'g' }, .to = Position{.row = 3, .col = 'e' } }));
    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'h' }, .to = Position{.row = 3, .col = 'e' } }));

    game->parseFEN("rnbqkbnr/1ppppppp/p7/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 8");

    EXPECT_TRUE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 4, .col = 'e' }, .to = Position{.row = 5, .col = 'e' } }));

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 4, .col = 'e' }, .to = Position{.row = 6, .col = 'e' } }));
    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 4, .col = 'e' }, .to = Position{.row = 5, .col = 'd' } }));
    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 4, .col = 'e' }, .to = Position{.row = 5, .col = 'f' } }));

    game->parseFEN("rnbqkbnr/pppp1ppp/8/8/4p3/P7/1PPPPPPP/RNBQKBNR w KQkq - 0 8");

    EXPECT_TRUE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 3, .col = 'e' } }));

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 4, .col = 'e' } }));
    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 3, .col = 'd' } }));
    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 3, .col = 'f' } }));
}

TEST(ValidatingMoveTest, PawnCapture) {
    auto game = std::make_unique<Game>();

    game->parseFEN("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 8");

    EXPECT_TRUE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 4, .col = 'e' }, .to = Position{.row = 5, .col = 'e' } }));

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 4, .col = 'e' }, .to = Position{.row = 6, .col = 'e' } }));

    EXPECT_TRUE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 4, .col = 'e' }, .to = Position{.row = 5, .col = 'd' }, .isCapture = true }));
    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 4, .col = 'e' }, .to = Position{.row = 5, .col = 'f' }, .isCapture = true }));
}

TEST(ValidatingMoveTest, EnPassant) {
    auto game = std::make_unique<Game>();

    game->applyMove(*game->parseMove("e4"));
    game->applyMove(*game->parseMove("b6"));
    game->applyMove(*game->parseMove("e5"));
    game->applyMove(*game->parseMove("d5"));

    // essentially setting the board in the position of `rnbqkbnr/p1p1pppp/1p6/3pP3/8/8/PPPP1PPP/RNBQKBNR` with the last move by black, `d6`

    EXPECT_TRUE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 5, .col = 'e' }, .to = Position{.row = 6, .col = 'e' } }));

    EXPECT_TRUE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 5, .col = 'e' }, .to = Position{.row = 6, .col = 'd' }, .isCapture = true }));
}

TEST(ValidatingMoveTest, Castling) {
    auto game = std::make_unique<Game>();

    game->parseFEN("rnbqkb1r/pp2pppp/2p2n2/3p4/4P3/3B1N2/PPPP1PPP/RNBQK2R w KQkq - 0 3");

    auto move1 = game->parseMove("O-O");

    ASSERT_NE(move1, std::nullopt);
    EXPECT_TRUE(game->isValidMove(*move1));

    auto move2 = game->parseMove("0-0");

    ASSERT_NE(move2, std::nullopt);
    EXPECT_TRUE(game->isValidMove(*move2));

    game->parseFEN("rnbqk2r/pp2bppp/2p1pn2/3pP3/8/2PB1N2/PP1P1PPP/RNBQK2R b KQkq - 0 4");

    auto move3 = game->parseMove("O-O");

    ASSERT_NE(move3, std::nullopt);
    EXPECT_TRUE(game->isValidMove(*move3));

    auto move4 = game->parseMove("0-0");

    ASSERT_NE(move4, std::nullopt);
    EXPECT_TRUE(game->isValidMove(*move4));

    game->parseFEN("r2qk2r/pp1b1ppp/n1p1pn2/2bp4/N3P3/1P1B1N2/PBPPQPPP/R3K2R w KQkq - 0 8");

    auto move5 = game->parseMove("0-0");

    ASSERT_NE(move5, std::nullopt);
    EXPECT_TRUE(game->isValidMove(*move5));

    auto move6 = game->parseMove("0-0-0");

    ASSERT_NE(move6, std::nullopt);
    EXPECT_TRUE(game->isValidMove(*move6));

    game->parseFEN("r2qk2r/pp1b1ppp/n1p1pn2/2bp4/4P3/1PNB1N2/PBPPQPPP/R3K2R b KQkq - 0 7");

    auto move7 = game->parseMove("0-0");

    ASSERT_NE(move7, std::nullopt);
    EXPECT_TRUE(game->isValidMove(*move7));

    auto move8 = game->parseMove("0-0-0");
    
    ASSERT_EQ(move8, std::nullopt);
}

TEST(ValidatingMoveTest, KingMovement) {
    auto game = std::make_unique<Game>();

    game->parseFEN("8/8/8/8/4K3/8/8/8 w KQkq - 0 1");

    EXPECT_EQ(game->parseMove("O-O"), std::nullopt);
    EXPECT_EQ(game->parseMove("O-O-O"), std::nullopt);

    auto move1 = game->parseMove("Kd5");

    ASSERT_NE(move1, std::nullopt);
    EXPECT_TRUE(game->isValidMove(*move1));
    
    auto move2 = game->parseMove("Ke5");

    ASSERT_NE(move2, std::nullopt);
    EXPECT_TRUE(game->isValidMove(*move2));

    auto move3 = game->parseMove("Kf5");

    ASSERT_NE(move3, std::nullopt);
    EXPECT_TRUE(game->isValidMove(*move3));

    auto move4 = game->parseMove("Kd4");

    ASSERT_NE(move4, std::nullopt);
    EXPECT_TRUE(game->isValidMove(*move4));

    auto move5 = game->parseMove("Kf4");

    ASSERT_NE(move5, std::nullopt);
    EXPECT_TRUE(game->isValidMove(*move5));

    auto move7 = game->parseMove("Kd3");

    ASSERT_NE(move7, std::nullopt);
    EXPECT_TRUE(game->isValidMove(*move7));

    auto move8 = game->parseMove("Ke3");

    ASSERT_NE(move8, std::nullopt);
    EXPECT_TRUE(game->isValidMove(*move8));

    auto move9 = game->parseMove("Kf3");

    ASSERT_NE(move9, std::nullopt);
    EXPECT_TRUE(game->isValidMove(*move9));
}

TEST(ApplyingMoveTest, PawnAdvancement) {
    auto game = std::make_unique<Game>();

    // game->applyMove(game->parseMove("e4"));
    game->applyMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 4, .col = 'e' } });

    EXPECT_EQ(game->serializeAsFEN(), "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b - - 0 1");
}
