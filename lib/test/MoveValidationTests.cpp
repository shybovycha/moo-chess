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

TEST(ValidatingPawnMoveTest, Advancement) {
    auto game = std::make_unique<Game>();

    EXPECT_TRUE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 3, .col = 'e' } }))
        << "e2 - e3 is allowed";

    EXPECT_TRUE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 4, .col = 'e' } }))
        << "e2 - e4 is allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 5, .col = 'e' } }))
        << "e2 - e5 is not allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'a' }, .to = Position{.row = 4, .col = 'e' } }))
        << "a2 - e4 is not allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'b' }, .to = Position{.row = 4, .col = 'e' } }))
        << "b2 - e4 is not allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'c' }, .to = Position{.row = 4, .col = 'e' } }))
        << "c2 - e4 is not allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'd' }, .to = Position{.row = 4, .col = 'e' } }))
        << "d2 - e4 is not allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'f' }, .to = Position{.row = 4, .col = 'e' } }))
        << "f2 - e4 is not allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'g' }, .to = Position{.row = 4, .col = 'e' } }))
        << "g2 - e4 is not allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'h' }, .to = Position{.row = 4, .col = 'e' } }))
        << "h2 - e4 is not allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'a' }, .to = Position{.row = 3, .col = 'e' } }))
        << "a2 - e3 is not allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'b' }, .to = Position{.row = 3, .col = 'e' } }))
        << "b2 - e3 is not allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'c' }, .to = Position{.row = 3, .col = 'e' } }))
        << "c2 - e3 is not allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'd' }, .to = Position{.row = 3, .col = 'e' } }))
        << "d2 - e3 is not allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'f' }, .to = Position{.row = 3, .col = 'e' } }))
        << "f2 - e3 is not allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'g' }, .to = Position{.row = 3, .col = 'e' } }))
        << "g2 - e3 is not allowed";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'h' }, .to = Position{.row = 3, .col = 'e' } }))
        << "h2 - e3 is not allowed";
}

TEST(ValidatingPawnMoveTest, FirstMoveAdvancement) {
    auto game = std::make_unique<Game>();

    game->parseFEN("rnbqkbnr/1ppppppp/p7/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 3");

    EXPECT_TRUE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 4, .col = 'e' }, .to = Position{.row = 5, .col = 'e' } }))
        << "Can advance one rank after first 2-rank move";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 4, .col = 'e' }, .to = Position{.row = 6, .col = 'e' } }))
        << "Can not advance 2 ranks after first move";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 4, .col = 'e' }, .to = Position{.row = 5, .col = 'd' } }))
        << "Can not move diagonally to the left";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 4, .col = 'e' }, .to = Position{.row = 5, .col = 'f' } }))
        << "Can not move diagonally to the right";

    game->parseFEN("rnbqkbnr/pppp1ppp/8/8/4p3/P7/1PPPPPPP/RNBQKBNR w KQkq - 0 3");

    EXPECT_TRUE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 3, .col = 'e' } }))
        << "Can advance one rank on first move when 4th rank is blocked by an opponent";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 4, .col = 'e' } }))
        << "Can not advance 2 ranks on first move when 4th rank is blocked by an opponent";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 3, .col = 'd' } }))
        << "Can not advance diagonally to the left when 4th rank is blocked by an opponent";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 3, .col = 'f' } }))
        << "Can not advance diagonally to the right when 4th rank is blocked by an opponent";

    game->parseFEN("rnbqkbnr/pppp1ppp/8/8/P7/4p3/1PPPPPPP/RNBQKBNR w KQkq - 0 8");

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 3, .col = 'e' } }))
        << "Can not advance one rank if the 3rd rank is blocked by an opponent";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 4, .col = 'e' } }))
        << "Can not advance two ranks if the 3rd rank is blocked by an opponent";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 3, .col = 'd' } }))
        << "Can not advance diagonally to the left if the 3rd rank is blocked by an opponent";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 3, .col = 'f' } }))
        << "Can not advance diagonally to the right if the 3rd rank is blocked by an opponent";
}

TEST(ValidatingPawnMoveTest, PawnCapture) {
    auto game = std::make_unique<Game>();

    game->parseFEN("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 8");

    EXPECT_TRUE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 4, .col = 'e' }, .to = Position{.row = 5, .col = 'e' } }))
        << "Pawn can still move straight when capture is available";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 4, .col = 'e' }, .to = Position{.row = 6, .col = 'e' } }))
        << "Pawn can not move 2 ranks straight when capture is available";

    EXPECT_TRUE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 4, .col = 'e' }, .to = Position{.row = 5, .col = 'd' }, .isCapture = true }))
        << "Pawn can move diagonally to the left (capture) when capture to the left is available";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 4, .col = 'e' }, .to = Position{.row = 5, .col = 'f' }, .isCapture = true }))
        << "Pawn can not move diagonally to the right when capture to the left is available";
}

TEST(ValidatingPawnMoveTest, EnPassant) {
    auto game = std::make_unique<Game>();

    game->applyMove(*game->parseMove("e4"));
    game->applyMove(*game->parseMove("b6"));
    game->applyMove(*game->parseMove("e5"));
    game->applyMove(*game->parseMove("d5"));

    // essentially setting the board in the position of `rnbqkbnr/p1p1pppp/1p6/3pP3/8/8/PPPP1PPP/RNBQKBNR` with the last move by black, `d6`

    EXPECT_TRUE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 5, .col = 'e' }, .to = Position{.row = 6, .col = 'e' } }))
        << "Pawn can perform en passant when the last move is the piece to be captured and it advanced 2 ranks";

    EXPECT_TRUE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 5, .col = 'e' }, .to = Position{.row = 6, .col = 'd' }, .isCapture = true }))
        << "Pawn can ignore en passant and advance one rank straight";
}

TEST(ValidatingPawnMoveTest, Promotion) {
    auto game = std::make_unique<Game>();

    game->parseFEN("8/4P3/8/8/8/8/8/8 w KQkq - 0 1");

    EXPECT_TRUE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 7, .col = 'e' }, .to = Position{.row = 6, .col = 'e' }, .promotion = WHITE_QUEEN }))
        << "Pawn can be promoted to queen on a straight advancement";

    EXPECT_TRUE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 7, .col = 'e' }, .to = Position{.row = 6, .col = 'e' }, .promotion = WHITE_ROOK }))
        << "Pawn can be promoted to rook on a straight advancement";

    EXPECT_TRUE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 7, .col = 'e' }, .to = Position{.row = 6, .col = 'e' }, .promotion = WHITE_KNIGHT }))
        << "Pawn can be promoted to knight on a straight advancement";

    EXPECT_TRUE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 7, .col = 'e' }, .to = Position{.row = 6, .col = 'e' }, .promotion = WHITE_BISHOP }))
        << "Pawn can be promoted to bishop on a straight advancement";

    EXPECT_TRUE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 7, .col = 'e' }, .to = Position{.row = 6, .col = 'e' }, .promotion = WHITE_KING }))
        << "Pawn can not be promoted to king";

    EXPECT_TRUE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 7, .col = 'e' }, .to = Position{.row = 6, .col = 'e' }, .promotion = WHITE_PAWN }))
        << "Pawn can not be promoted to pawn";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 7, .col = 'e' }, .to = Position{.row = 8, .col = 'd' }, .promotion = WHITE_QUEEN }))
        << "Pawn can not be promoted diagonally to the left";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 7, .col = 'e' }, .to = Position{.row = 8, .col = 'f' }, .promotion = WHITE_QUEEN }))
        << "Pawn can not be promoted diagonally to the right";

    game->parseFEN("3q4/4P3/8/8/8/8/8/8 w KQkq - 0 1");

    EXPECT_TRUE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 7, .col = 'e' }, .to = Position{.row = 6, .col = 'e' }, .promotion = WHITE_QUEEN, .isCapture = true }))
        << "Pawn can be promoted to queen on a straight advancement even if capture is available to the left";

    EXPECT_TRUE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 7, .col = 'e' }, .to = Position{.row = 8, .col = 'd' }, .promotion = WHITE_QUEEN, .isCapture = true }))
        << "Pawn can be promoted diagonally to the left only if capturing";

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 7, .col = 'e' }, .to = Position{.row = 8, .col = 'f' }, .promotion = WHITE_QUEEN }))
        << "Pawn can not be promoted diagonally to the right if capture is available to the left";

    game->parseFEN("3Q4/4P3/8/8/8/8/8/8 w KQkq - 0 1");

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 7, .col = 'e' }, .to = Position{.row = 8, .col = 'f' }, .promotion = WHITE_QUEEN }))
        << "Pawn can not be promoted diagonally to the right if blocked by the ally piece on the capture field to the left";

    game->parseFEN("4q3/4P3/8/8/8/8/8/8 w KQkq - 0 1");

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 7, .col = 'e' }, .to = Position{.row = 8, .col = 'f' }, .promotion = WHITE_QUEEN }))
        << "Pawn can not be promoted if blocked by the opponent piece ahead";

    game->parseFEN("4Q3/4P3/8/8/8/8/8/8 w KQkq - 0 1");

    EXPECT_FALSE(game->isValidMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 7, .col = 'e' }, .to = Position{.row = 8, .col = 'f' }, .promotion = WHITE_QUEEN }))
        << "Pawn can not be promoted if blocked by the ally piece ahead";
}

TEST(ValidatingKingMoveTest, Castling) {
    auto game = std::make_unique<Game>();

    game->parseFEN("rnbqkb1r/pp2pppp/2p2n2/3p4/4P3/3B1N2/PPPP1PPP/RNBQK2R w KQkq - 0 3");

    auto move1 = game->parseMove("O-O");

    ASSERT_NE(move1, std::nullopt)
        << "Notation of `O-O` is valid";

    EXPECT_TRUE(game->isValidMove(*move1))
        << "Short castling for white noted as `O-O` is valid";

    auto move2 = game->parseMove("0-0");

    ASSERT_NE(move2, std::nullopt)
        << "Notation of `0-0` is valid";

    EXPECT_TRUE(game->isValidMove(*move2))
        << "Short castling for white noted as `0-0` is valid";

    game->parseFEN("rnbqk2r/pp2bppp/2p1pn2/3pP3/8/2PB1N2/PP1P1PPP/RNBQK2R b KQkq - 0 4");

    auto move3 = game->parseMove("O-O");

    ASSERT_NE(move3, std::nullopt)
        << "Notation of `O-O` is valid";

    EXPECT_TRUE(game->isValidMove(*move3))
        << "Short castling for black noted as `O-O` is valid";

    auto move4 = game->parseMove("0-0");

    ASSERT_NE(move4, std::nullopt)
        << "Notation of `0-0` is valid";

    EXPECT_TRUE(game->isValidMove(*move4))
        << "Short castling for black noted as `0-0` is valid";

    game->parseFEN("r2qk2r/pp1b1ppp/n1p1pn2/2bp4/N3P3/1P1B1N2/PBPPQPPP/R3K2R w KQkq - 0 8");

    auto move5 = game->parseMove("0-0");

    ASSERT_NE(move5, std::nullopt)
        << "Notation of `0-0` is valid";

    EXPECT_TRUE(game->isValidMove(*move5))
        << "Short castling for white noted as `0-0` is valid";

    auto move6 = game->parseMove("0-0-0");

    ASSERT_NE(move6, std::nullopt)
        << "Notation of `0-0-0` is valid";

    EXPECT_TRUE(game->isValidMove(*move6))
        << "Long castling for white noted as `0-0-0` is valid";

    game->parseFEN("r2qk2r/pp1b1ppp/n1p1pn2/2bp4/4P3/1PNB1N2/PBPPQPPP/R3K2R b KQkq - 0 7");

    auto move7 = game->parseMove("0-0");

    ASSERT_NE(move7, std::nullopt)
        << "Notation of `0-0` is valid";

    EXPECT_TRUE(game->isValidMove(*move7))
        << "Short castling for black noted as `0-0` is a valid move";

    auto move8 = game->parseMove("0-0-0");
    
    ASSERT_EQ(move8, std::nullopt)
        << "Long castling is an invalid move";

    game->parseFEN("r2qk2r/pp1b1ppp/n1p1pn2/2bp4/4P3/1PNB1N2/PBPPQPPP/R3K2R b - - 0 7");

    auto move9 = game->parseMove("0-0");

    ASSERT_EQ(move9, std::nullopt)
        << "Short castling is not available, hence an invalid move";

    auto move10 = game->parseMove("0-0-0");

    ASSERT_EQ(move8, std::nullopt)
        << "Long castling is not available, hence an invalid move";
}

TEST(ValidatingKingMoveTest, Advancement) {
    auto game = std::make_unique<Game>();

    game->parseFEN("8/8/8/8/4K3/8/8/8 w KQkq - 0 1");

    EXPECT_EQ(game->parseMove("O-O"), std::nullopt)
        << "On an empty field, short castling is not valid";

    EXPECT_EQ(game->parseMove("O-O-O"), std::nullopt)
        << "On an empty field, long castling is not valid";

    auto move1 = game->parseMove("Kd5");

    ASSERT_NE(move1, std::nullopt)
        << "Kd5 is valid notation";

    EXPECT_TRUE(game->isValidMove(*move1))
        << "K e4 - K d5 is a valid move";
    
    auto move2 = game->parseMove("Ke5");

    ASSERT_NE(move2, std::nullopt)
        << "Ke5 is valid notation";

    EXPECT_TRUE(game->isValidMove(*move2))
        << "K e4 - K e5 is a valid move";

    auto move3 = game->parseMove("Kf5");

    ASSERT_NE(move3, std::nullopt)
        << "Kf5 is valid notation";

    EXPECT_TRUE(game->isValidMove(*move3))
        << "K e4 - K f5 is a valid move";

    auto move4 = game->parseMove("Kd4");

    ASSERT_NE(move4, std::nullopt)
        << "Kd4 is valid notation";

    EXPECT_TRUE(game->isValidMove(*move4))
        << "K e4 - K d4 is a valid move";

    auto move5 = game->parseMove("Kf4");

    ASSERT_NE(move5, std::nullopt)
        << "Kf4 is valid notation";

    EXPECT_TRUE(game->isValidMove(*move5))
        << "K e4 - K f4 is a valid move";

    auto move7 = game->parseMove("Kd3");

    ASSERT_NE(move7, std::nullopt)
        << "Kd3 is valid notation";

    EXPECT_TRUE(game->isValidMove(*move7))
        << "K e4 - K d3 is a valid move";

    auto move8 = game->parseMove("Ke3");

    ASSERT_NE(move8, std::nullopt)
        << "Ke3 is valid notation";

    EXPECT_TRUE(game->isValidMove(*move8))
        << "K e4 - K e3 is a valid move";

    auto move9 = game->parseMove("Kf3");

    ASSERT_NE(move9, std::nullopt)
        << "Kf3 is valid notation";

    EXPECT_TRUE(game->isValidMove(*move9))
        << "K e4 - K f3 is a valid move";
}

TEST(ValidatingKingMoveTest, AdvancementUnderAttack) {
    auto game = std::make_unique<Game>();

    game->parseFEN("8/8/8/8/8/2k5/4K3/8 w - - 0 1");

    EXPECT_EQ(game->parseMove("O-O"), std::nullopt)
        << "Short castling is not valid if king is not on its original rank";

    EXPECT_EQ(game->parseMove("O-O-O"), std::nullopt)
        << "Long castling is not valid if king is not on its original rank";

    auto move1 = game->parseMove("Kd1");
    
    ASSERT_EQ(move1, std::nullopt)
        << "Kd1 is not a valid move since d1 is under attack by black king";

    auto move2 = game->parseMove("Kd2");

    ASSERT_EQ(move2, std::nullopt)
        << "Kd2 is not a valid move since d2 is under attack by black king";

    auto move3 = game->parseMove("Kd3");

    ASSERT_EQ(move3, std::nullopt)
        << "Kd3 is not a valid move since d3 is under attack by black king";

    auto move4 = game->parseMove("Ke3");

    ASSERT_NE(move4, std::nullopt)
        << "Ke3 is valid notation";

    EXPECT_TRUE(game->isValidMove(*move4))
        << "Ke3 is a valid move";

    auto move5 = game->parseMove("Kf3");

    ASSERT_NE(move5, std::nullopt)
        << "Kf3 is valid notation";

    EXPECT_TRUE(game->isValidMove(*move5))
        << "Kf3 is a valid move";

    auto move6 = game->parseMove("Kf2");

    ASSERT_NE(move6, std::nullopt)
        << "Kf2 is valid notation";

    EXPECT_TRUE(game->isValidMove(*move6))
        << "Kf2 is a valid move";

    auto move7 = game->parseMove("Kf1");

    ASSERT_NE(move7, std::nullopt)
        << "Kf1 is valid notation";

    EXPECT_TRUE(game->isValidMove(*move7))
        << "Kf1 is a valid move";

    // ------------------------------------------------------------------------

    game->parseFEN("8/8/8/8/8/2k5/4K3/8 b - - 0 1");

    EXPECT_EQ(game->parseMove("O-O"), std::nullopt)
        << "Short castling is not valid if king is not on its original rank";

    EXPECT_EQ(game->parseMove("O-O-O"), std::nullopt)
        << "Long castling is not valid if king is not on its original rank";

    auto move8 = game->parseMove("Kd2");

    ASSERT_EQ(move8, std::nullopt)
        << "Kd2 is not a valid move since d2 is under attack by white king";

    auto move9 = game->parseMove("Kd3");

    ASSERT_EQ(move9, std::nullopt)
        << "Kd3 is not a valid move since d3 is under attack by white king";

    auto move10 = game->parseMove("Kd4");

    ASSERT_NE(move10, std::nullopt)
        << "Kd4 is valid notation";

    EXPECT_TRUE(game->isValidMove(*move10))
        << "Kd4 is a valid move";
}

TEST(ApplyingMoveTest, PawnAdvancement) {
    auto game = std::make_unique<Game>();

    // game->applyMove(game->parseMove("e4"));
    game->applyMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 4, .col = 'e' } });

    EXPECT_EQ(game->serializeAsFEN(), "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b - - 0 1")
        << "Serialized board after applying a e4 move";
}
