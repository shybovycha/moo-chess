#include <memory>
#include <string>

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

#include "chesslib/Game.hpp"

TEST(ValidatingPawnMoveTest, Advancement) {
    auto game = std::make_unique<Game>();

    EXPECT_TRUE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 3, .col = 'e' } }))
        << "e2 - e3 is allowed";

    EXPECT_TRUE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 4, .col = 'e' } }))
        << "e2 - e4 is allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 5, .col = 'e' } }))
        << "e2 - e5 is not allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 2, .col = 'a' }, .to = Position{.row = 4, .col = 'e' } }))
        << "a2 - e4 is not allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 2, .col = 'b' }, .to = Position{.row = 4, .col = 'e' } }))
        << "b2 - e4 is not allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 2, .col = 'c' }, .to = Position{.row = 4, .col = 'e' } }))
        << "c2 - e4 is not allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 2, .col = 'd' }, .to = Position{.row = 4, .col = 'e' } }))
        << "d2 - e4 is not allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 2, .col = 'f' }, .to = Position{.row = 4, .col = 'e' } }))
        << "f2 - e4 is not allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 2, .col = 'g' }, .to = Position{.row = 4, .col = 'e' } }))
        << "g2 - e4 is not allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 2, .col = 'h' }, .to = Position{.row = 4, .col = 'e' } }))
        << "h2 - e4 is not allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 2, .col = 'a' }, .to = Position{.row = 3, .col = 'e' } }))
        << "a2 - e3 is not allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 2, .col = 'b' }, .to = Position{.row = 3, .col = 'e' } }))
        << "b2 - e3 is not allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 2, .col = 'c' }, .to = Position{.row = 3, .col = 'e' } }))
        << "c2 - e3 is not allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 2, .col = 'd' }, .to = Position{.row = 3, .col = 'e' } }))
        << "d2 - e3 is not allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 2, .col = 'f' }, .to = Position{.row = 3, .col = 'e' } }))
        << "f2 - e3 is not allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 2, .col = 'g' }, .to = Position{.row = 3, .col = 'e' } }))
        << "g2 - e3 is not allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 2, .col = 'h' }, .to = Position{.row = 3, .col = 'e' } }))
        << "h2 - e3 is not allowed";
}

TEST(ValidatingPawnMoveTest, FirstMoveAdvancement) {
    auto game = std::make_unique<Game>();

    game->parseFEN("rnbqkbnr/1ppppppp/p7/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 3");

    EXPECT_TRUE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 4, .col = 'e' }, .to = Position{.row = 5, .col = 'e' } }))
        << "Can advance one rank after first 2-rank move";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 4, .col = 'e' }, .to = Position{.row = 6, .col = 'e' } }))
        << "Can not advance 2 ranks after first move";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 4, .col = 'e' }, .to = Position{.row = 5, .col = 'd' } }))
        << "Can not move diagonally to the left";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 4, .col = 'e' }, .to = Position{.row = 5, .col = 'f' } }))
        << "Can not move diagonally to the right";
}

TEST(ValidatingPawnMoveTest, FirstMoveAdvancement2) {
    auto game = std::make_unique<Game>();

    game->parseFEN("rnbqkbnr/pppp1ppp/8/8/4p3/P7/1PPPPPPP/RNBQKBNR w KQkq - 0 3");

    EXPECT_TRUE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 3, .col = 'e' } }))
        << "Can advance one rank on first move when 4th rank is blocked by an opponent";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 4, .col = 'e' } }))
        << "Can not advance 2 ranks on first move when 4th rank is blocked by an opponent";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 3, .col = 'd' } }))
        << "Can not advance diagonally to the left when 4th rank is blocked by an opponent";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 3, .col = 'f' } }))
        << "Can not advance diagonally to the right when 4th rank is blocked by an opponent";
}

TEST(ValidatingPawnMoveTest, FirstMoveAdvancement3) {
    auto game = std::make_unique<Game>();

    game->parseFEN("rnbqkbnr/pppp1ppp/8/8/P7/4p3/1PPPPPPP/RNBQKBNR w KQkq - 0 8");

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 3, .col = 'e' } }))
        << "Can not advance one rank if the 3rd rank is blocked by an opponent";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 4, .col = 'e' } }))
        << "Can not advance two ranks if the 3rd rank is blocked by an opponent";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 3, .col = 'd' } }))
        << "Can not advance diagonally to the left if the 3rd rank is blocked by an opponent";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 3, .col = 'f' } }))
        << "Can not advance diagonally to the right if the 3rd rank is blocked by an opponent";
}

TEST(ValidatingPawnMoveTest, PawnCapture) {
    auto game = std::make_unique<Game>();

    game->parseFEN("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 8");

    EXPECT_TRUE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 4, .col = 'e' }, .to = Position{.row = 5, .col = 'e' } }))
        << "Pawn can still move straight when capture is available";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 4, .col = 'e' }, .to = Position{.row = 6, .col = 'e' } }))
        << "Pawn can not move 2 ranks straight when capture is available";

    EXPECT_TRUE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 4, .col = 'e' }, .to = Position{.row = 5, .col = 'd' }, .isCapture = true }))
        << "Pawn can move diagonally to the left (capture) when capture to the left is available";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 4, .col = 'e' }, .to = Position{.row = 5, .col = 'f' }, .isCapture = true }))
        << "Pawn can not move diagonally to the right when capture to the left is available";
}

TEST(ValidatingPawnMoveTest, EnPassant) {
    auto game = std::make_unique<Game>();

    game->applyMove(*game->parseMove("e4"));
    game->applyMove(*game->parseMove("b6"));
    game->applyMove(*game->parseMove("e5"));
    game->applyMove(*game->parseMove("d5"));

    // essentially setting the board in the position of `rnbqkbnr/p1p1pppp/1p6/3pP3/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 1` with the last move by black, `d6`

    EXPECT_TRUE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 5, .col = 'e' }, .to = Position{.row = 6, .col = 'd' }, .isCapture = true }))
        << "Pawn can perform en passant when the last move is the piece to be captured and it advanced 2 ranks";

    EXPECT_TRUE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 5, .col = 'e' }, .to = Position{.row = 6, .col = 'e' } }))
        << "Pawn can ignore en passant and advance one rank straight";
}

TEST(ValidatingPawnMoveTest, Promotion) {
    auto game = std::make_unique<Game>();

    game->parseFEN("8/4P3/8/8/8/8/8/K6k w - - 0 1");

    EXPECT_TRUE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 7, .col = 'e' }, .to = Position{.row = 8, .col = 'e' }, .promotion = Piece::WHITE_QUEEN }))
        << "Pawn can be promoted to queen on a straight advancement";

    EXPECT_TRUE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 7, .col = 'e' }, .to = Position{.row = 8, .col = 'e' }, .promotion = Piece::WHITE_ROOK }))
        << "Pawn can be promoted to rook on a straight advancement";

    EXPECT_TRUE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 7, .col = 'e' }, .to = Position{.row = 8, .col = 'e' }, .promotion = Piece::WHITE_KNIGHT }))
        << "Pawn can be promoted to knight on a straight advancement";

    EXPECT_TRUE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 7, .col = 'e' }, .to = Position{.row = 8, .col = 'e' }, .promotion = Piece::WHITE_BISHOP }))
        << "Pawn can be promoted to bishop on a straight advancement";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 7, .col = 'e' }, .to = Position{.row = 8, .col = 'e' }, .promotion = Piece::WHITE_KING }))
        << "Pawn can not be promoted to king";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 7, .col = 'e' }, .to = Position{.row = 8, .col = 'e' }, .promotion = Piece::WHITE_PAWN }))
        << "Pawn can not be promoted to pawn";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 7, .col = 'e' }, .to = Position{.row = 8, .col = 'd' }, .promotion = Piece::WHITE_QUEEN }))
        << "Pawn can not be promoted diagonally to the left";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 7, .col = 'e' }, .to = Position{.row = 8, .col = 'f' }, .promotion = Piece::WHITE_QUEEN }))
        << "Pawn can not be promoted diagonally to the right";
}

TEST(ValidatingPawnMoveTest, Promotion2) {
    auto game = std::make_unique<Game>();

    game->parseFEN("3q4/4P3/8/8/8/8/8/K6k w - - 0 1");

    EXPECT_TRUE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 7, .col = 'e' }, .to = Position{.row = 8, .col = 'e' }, .promotion = Piece::WHITE_QUEEN, .isCapture = false }))
        << "Pawn can be promoted to queen on a straight advancement even if capture is available to the left";

    EXPECT_TRUE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 7, .col = 'e' }, .to = Position{.row = 8, .col = 'd' }, .promotion = Piece::WHITE_QUEEN, .isCapture = true }))
        << "Pawn can be promoted diagonally to the left only if capturing";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 7, .col = 'e' }, .to = Position{.row = 8, .col = 'f' }, .promotion = Piece::WHITE_QUEEN }))
        << "Pawn can not be promoted diagonally to the right if capture is available to the left";
}

TEST(ValidatingPawnMoveTest, Promotion3) {
    auto game = std::make_unique<Game>();

    game->parseFEN("3Q4/4P3/8/8/8/8/8/8 w KQkq - 0 1");

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 7, .col = 'e' }, .to = Position{.row = 8, .col = 'f' }, .promotion = Piece::WHITE_QUEEN }))
        << "Pawn can not be promoted diagonally to the right if blocked by the ally piece on the capture field to the left";
}

TEST(ValidatingPawnMoveTest, Promotion4) {
    auto game = std::make_unique<Game>();

    game->parseFEN("4q3/4P3/8/8/8/8/8/8 w KQkq - 0 1");

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 7, .col = 'e' }, .to = Position{.row = 8, .col = 'f' }, .promotion = Piece::WHITE_QUEEN }))
        << "Pawn can not be promoted if blocked by the opponent piece ahead";
}

TEST(ValidatingPawnMoveTest, Promotion5) {
    auto game = std::make_unique<Game>();

    game->parseFEN("4Q3/4P3/8/8/8/8/8/8 w KQkq - 0 1");

    EXPECT_FALSE(game->isValidMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 7, .col = 'e' }, .to = Position{.row = 8, .col = 'f' }, .promotion = Piece::WHITE_QUEEN }))
        << "Pawn can not be promoted if blocked by the ally piece ahead";
}
