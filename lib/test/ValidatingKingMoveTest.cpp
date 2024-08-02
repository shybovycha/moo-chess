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

TEST(ValidatingKingMoveTest, Castling) {
    auto game = std::make_unique<Game>();

    game->parseFEN("rnbqkb1r/pp2pppp/2p2n2/3p4/4P3/3B1N2/PPPP1PPP/RNBQK2R w KQkq - 0 3");

    auto move1 = game->parseMove("O-O");

    ASSERT_NE(move1, std::nullopt)
        << "Notation of `O-O` is valid";

    EXPECT_TRUE(game->isValidMove(*move1))
        << "Short castling for white noted as `O-O` is valid";
}

TEST(ValidatingKingMoveTest, Castling2) {
    auto game = std::make_unique<Game>();

    game->parseFEN("rnbqk2r/pp2bppp/2p1pn2/3pP3/8/2PB1N2/PP1P1PPP/RNBQK2R b KQkq - 0 4");

    auto move2 = game->parseMove("0-0");

    ASSERT_NE(move2, std::nullopt)
        << "Notation of `0-0` is valid";

    EXPECT_TRUE(game->isValidMove(*move2))
        << "Short castling for white noted as `0-0` is valid";
}

TEST(ValidatingKingMoveTest, Castling3) {
    auto game = std::make_unique<Game>();

    game->parseFEN("rnbqk2r/pp2bppp/2p1pn2/3pP3/8/2PB1N2/PP1P1PPP/RNBQK2R b KQkq - 0 4");

    auto move3 = game->parseMove("O-O");

    ASSERT_NE(move3, std::nullopt)
        << "Notation of `O-O` is valid";

    EXPECT_TRUE(game->isValidMove(*move3))
        << "Short castling for black noted as `O-O` is valid";
}

TEST(ValidatingKingMoveTest, Castling4) {
    auto game = std::make_unique<Game>();

    game->parseFEN("rnbqk2r/pp2bppp/2p1pn2/3pP3/8/2PB1N2/PP1P1PPP/RNBQK2R b KQkq - 0 4");

    auto move4 = game->parseMove("0-0");

    ASSERT_NE(move4, std::nullopt)
        << "Notation of `0-0` is valid";

    EXPECT_TRUE(game->isValidMove(*move4))
        << "Short castling for black noted as `0-0` is valid";
}

TEST(ValidatingKingMoveTest, Castling5) {
    auto game = std::make_unique<Game>();

    game->parseFEN("r2qk2r/pp1b1ppp/n1p1pn2/2bp4/N3P3/1P1B1N2/PBPPQPPP/R3K2R w KQkq - 0 8");

    auto move5 = game->parseMove("0-0");

    ASSERT_NE(move5, std::nullopt)
        << "Notation of `0-0` is valid";

    EXPECT_TRUE(game->isValidMove(*move5))
        << "Short castling for white noted as `0-0` is valid";
}

TEST(ValidatingKingMoveTest, Castling6) {
    auto game = std::make_unique<Game>();

    game->parseFEN("rnbqk2r/pp2bppp/2p1pn2/3pP3/8/2PB1N2/PP1P1PPP/RNBQK2R b KQkq - 0 4");

    auto move6 = game->parseMove("0-0-0");

    ASSERT_EQ(move6, std::nullopt)
        << "Long castling is invalid move";
}

TEST(ValidatingKingMoveTest, Castling7) {
    auto game = std::make_unique<Game>();

    game->parseFEN("r2qk2r/pp1b1ppp/n1p1pn2/2bp4/4P3/1PNB1N2/PBPPQPPP/R3K2R b KQkq - 0 7");

    auto move7 = game->parseMove("0-0");

    ASSERT_NE(move7, std::nullopt)
        << "Notation of `0-0` is valid";

    EXPECT_TRUE(game->isValidMove(*move7))
        << "Short castling for black noted as `0-0` is a valid move";
}

TEST(ValidatingKingMoveTest, Castling8) {
    auto game = std::make_unique<Game>();

    game->parseFEN("r2qk2r/pp1b1ppp/n1p1pn2/2bp4/4P3/1PNB1N2/PBPPQPPP/R3K2R b KQkq - 0 7");

    auto move8 = game->parseMove("0-0-0");

    ASSERT_EQ(move8, std::nullopt)
        << "Long castling is an invalid move";
}

TEST(ValidatingKingMoveTest, Castling9) {
    auto game = std::make_unique<Game>();

    game->parseFEN("r2qk2r/pp1b1ppp/n1p1pn2/2bp4/4P3/1PNB1N2/PBPPQPPP/R3K2R b - - 0 7");

    auto move9 = game->parseMove("0-0");

    ASSERT_EQ(move9, std::nullopt)
        << "Short castling is not available, hence an invalid move";
}

TEST(ValidatingKingMoveTest, Castling10) {
    auto game = std::make_unique<Game>();

    game->parseFEN("r2qk2r/pp1b1ppp/n1p1pn2/2bp4/4P3/1PNB1N2/PBPPQPPP/R3K2R b - - 0 7");

    auto move10 = game->parseMove("0-0-0");

    ASSERT_EQ(move10, std::nullopt)
        << "Long castling is not available, hence an invalid move";
}

TEST(ValidatingKingMoveTest, Advancement) {
    auto game = std::make_unique<Game>();

    game->parseFEN("8/8/8/8/4K3/8/8/8 w KQkq - 0 1");

    EXPECT_EQ(game->parseMove("O-O"), std::nullopt)
        << "On an empty field, short castling is not valid";

    EXPECT_EQ(game->parseMove("O-O-O"), std::nullopt)
        << "On an empty field, long castling is not valid";
}

TEST(ValidatingKingMoveTest, Advancement1) {
    auto game = std::make_unique<Game>();

    game->parseFEN("8/8/8/8/4K3/8/8/8 w KQkq - 0 1");

    auto move1 = game->parseMove("Kd5");

    ASSERT_NE(move1, std::nullopt)
        << "Kd5 is valid notation";

    EXPECT_TRUE(game->isValidMove(*move1))
        << "K e4 - K d5 is a valid move";
}

TEST(ValidatingKingMoveTest, Advancement2) {
    auto game = std::make_unique<Game>();

    game->parseFEN("8/8/8/8/4K3/8/8/8 w KQkq - 0 1");

    auto move2 = game->parseMove("Ke5");

    ASSERT_NE(move2, std::nullopt)
        << "Ke5 is valid notation";

    EXPECT_TRUE(game->isValidMove(*move2))
        << "K e4 - K e5 is a valid move";
}

TEST(ValidatingKingMoveTest, Advancement3) {
    auto game = std::make_unique<Game>();

    game->parseFEN("8/8/8/8/4K3/8/8/8 w KQkq - 0 1");

    auto move3 = game->parseMove("Kf5");

    ASSERT_NE(move3, std::nullopt)
        << "Kf5 is valid notation";

    EXPECT_TRUE(game->isValidMove(*move3))
        << "K e4 - K f5 is a valid move";
}

TEST(ValidatingKingMoveTest, Advancement4) {
    auto game = std::make_unique<Game>();

    game->parseFEN("8/8/8/8/4K3/8/8/8 w KQkq - 0 1");

    auto move4 = game->parseMove("Kd4");

    ASSERT_NE(move4, std::nullopt)
        << "Kd4 is valid notation";

    EXPECT_TRUE(game->isValidMove(*move4))
        << "K e4 - K d4 is a valid move";
}

TEST(ValidatingKingMoveTest, Advancement5) {
    auto game = std::make_unique<Game>();

    game->parseFEN("8/8/8/8/4K3/8/8/8 w KQkq - 0 1");

    auto move5 = game->parseMove("Kf4");

    ASSERT_NE(move5, std::nullopt)
        << "Kf4 is valid notation";

    EXPECT_TRUE(game->isValidMove(*move5))
        << "K e4 - K f4 is a valid move";
}

TEST(ValidatingKingMoveTest, Advancement7) {
    auto game = std::make_unique<Game>();

    game->parseFEN("8/8/8/8/4K3/8/8/8 w KQkq - 0 1");

    auto move7 = game->parseMove("Kd3");

    ASSERT_NE(move7, std::nullopt)
        << "Kd3 is valid notation";

    EXPECT_TRUE(game->isValidMove(*move7))
        << "K e4 - K d3 is a valid move";
}

TEST(ValidatingKingMoveTest, Advancement8) {
    auto game = std::make_unique<Game>();

    game->parseFEN("8/8/8/8/4K3/8/8/8 w KQkq - 0 1");

    auto move8 = game->parseMove("Ke3");

    ASSERT_NE(move8, std::nullopt)
        << "Ke3 is valid notation";

    EXPECT_TRUE(game->isValidMove(*move8))
        << "K e4 - K e3 is a valid move";
}

TEST(ValidatingKingMoveTest, Advancement9) {
    auto game = std::make_unique<Game>();

    game->parseFEN("8/8/8/8/4K3/8/8/8 w KQkq - 0 1");

    auto move9 = game->parseMove("Kf3");

    ASSERT_NE(move9, std::nullopt)
        << "Kf3 is valid notation";

    EXPECT_TRUE(game->isValidMove(*move9))
        << "K e4 - K f3 is a valid move";
}

TEST(ValidatingKingMoveTest, Advancement10) {
    auto game = std::make_unique<Game>();

    game->parseFEN("rnbqkbnr/ppp1p1pp/3p1p2/8/8/4PP2/PPPP2PP/RNBQKBNR w KQkq - 0 1");

    auto move10 = game->parseMove("Ke2");

    ASSERT_NE(move10, std::nullopt)
        << "Ke2 is valid notation";

    EXPECT_TRUE(game->isValidMove(*move10))
        << "K e1 - K e2 is a valid move";
}

TEST(ValidatingKingMoveTest, Advancement11) {
    auto game = std::make_unique<Game>();

    game->parseFEN("rnbqkbnr/ppp1p1pp/3p1p2/8/8/4PP2/PPPP2PP/RNBQKBNR w KQkq - 0 1");

    auto move11 = game->parseMove("Kf2");

    ASSERT_NE(move11, std::nullopt)
        << "Kf2 is valid notation";

    EXPECT_TRUE(game->isValidMove(*move11))
        << "K e1 - K f2 is a valid move";
}

TEST(ValidatingKingMoveTest, Advancement12) {
    auto game = std::make_unique<Game>();

    game->parseFEN("rnbqkbnr/ppp1p1pp/3p1p2/8/8/4PP2/PPPP2PP/RNBQKBNR w KQkq - 0 1");

    auto move12 = game->parseMove("Ke2");

    ASSERT_NE(move12, std::nullopt)
        << "Ke2 is valid notation";

    EXPECT_FALSE(game->isValidMove(*move12))
        << "K e1 - K e2 is not a valid move";
}

TEST(ValidatingKingMoveTest, AdvancementUnderAttack) {
    auto game = std::make_unique<Game>();

    game->parseFEN("8/8/8/8/8/2k5/4K3/8 w - - 0 1");

    EXPECT_EQ(game->parseMove("O-O"), std::nullopt)
        << "Short castling is not valid if king is not on its original rank";

    EXPECT_EQ(game->parseMove("O-O-O"), std::nullopt)
        << "Long castling is not valid if king is not on its original rank";
}

TEST(ValidatingKingMoveTest, AdvancementUnderAttack1) {
    auto game = std::make_unique<Game>();

    game->parseFEN("8/8/8/8/8/2k5/4K3/8 w - - 0 1");

    auto move1 = game->parseMove("Kd1");

    ASSERT_TRUE(game->isValidMove(*move1))
        << "Kd1 is a valid move since d1 is not under attack";
}

TEST(ValidatingKingMoveTest, AdvancementUnderAttack2) {
    auto game = std::make_unique<Game>();

    game->parseFEN("8/8/8/8/8/2k5/4K3/8 w - - 0 1");

    auto move2 = game->parseMove("Kd2");

    ASSERT_EQ(move2, std::nullopt);

    /*ASSERT_FALSE(game->isValidMove(*move2))
        << "Kd2 is not a valid move since d2 is under attack by black king";*/
}

TEST(ValidatingKingMoveTest, AdvancementUnderAttack3) {
    auto game = std::make_unique<Game>();

    game->parseFEN("8/8/8/8/8/2k5/4K3/8 w - - 0 1");

    auto move3 = game->parseMove("Kd3");

    ASSERT_EQ(move3, std::nullopt);

    /*ASSERT_FALSE(game->isValidMove(*move3))
        << "Kd3 is not a valid move since d3 is under attack by black king";*/
}

TEST(ValidatingKingMoveTest, AdvancementUnderAttack4) {
    auto game = std::make_unique<Game>();

    game->parseFEN("8/8/8/8/8/2k5/4K3/8 w - - 0 1");

    auto move4 = game->parseMove("Ke3");

    ASSERT_NE(move4, std::nullopt)
        << "Ke3 is valid notation";

    EXPECT_TRUE(game->isValidMove(*move4))
        << "Ke3 is a valid move";
}

TEST(ValidatingKingMoveTest, AdvancementUnderAttack5) {
    auto game = std::make_unique<Game>();

    game->parseFEN("8/8/8/8/8/2k5/4K3/8 w - - 0 1");

    auto move5 = game->parseMove("Kf3");

    ASSERT_NE(move5, std::nullopt)
        << "Kf3 is valid notation";

    EXPECT_TRUE(game->isValidMove(*move5))
        << "Kf3 is a valid move";
}

TEST(ValidatingKingMoveTest, AdvancementUnderAttack6) {
    auto game = std::make_unique<Game>();

    game->parseFEN("8/8/8/8/8/2k5/4K3/8 w - - 0 1");

    auto move6 = game->parseMove("Kf2");

    ASSERT_NE(move6, std::nullopt)
        << "Kf2 is valid notation";

    EXPECT_TRUE(game->isValidMove(*move6))
        << "Kf2 is a valid move";
}

TEST(ValidatingKingMoveTest, AdvancementUnderAttack7) {
    auto game = std::make_unique<Game>();

    game->parseFEN("8/8/8/8/8/2k5/4K3/8 w - - 0 1");

    auto move7 = game->parseMove("Kf1");

    ASSERT_NE(move7, std::nullopt)
        << "Kf1 is valid notation";

    EXPECT_TRUE(game->isValidMove(*move7))
        << "Kf1 is a valid move";
}

TEST(ValidatingKingMoveTest, AdvancementUnderAttack8) {
    auto game = std::make_unique<Game>();

    game->parseFEN("8/8/8/8/8/2k5/4K3/8 b - - 0 1");

    EXPECT_EQ(game->parseMove("O-O"), std::nullopt)
        << "Short castling is not valid if king is not on its original rank";

    EXPECT_EQ(game->parseMove("O-O-O"), std::nullopt)
        << "Long castling is not valid if king is not on its original rank";
}

TEST(ValidatingKingMoveTest, AdvancementUnderAttack9) {
    auto game = std::make_unique<Game>();

    game->parseFEN("8/8/8/8/8/2k5/4K3/8 b - - 0 1");

    auto move8 = game->parseMove("Kd2");

    ASSERT_EQ(move8, std::nullopt)
        << "Kd2 is not a valid move since d2 is under attack by white king";
}

TEST(ValidatingKingMoveTest, AdvancementUnderAttack10) {
    auto game = std::make_unique<Game>();

    game->parseFEN("8/8/8/8/8/2k5/4K3/8 b - - 0 1");

    auto move9 = game->parseMove("Kd3");

    ASSERT_EQ(move9, std::nullopt)
        << "Kd3 is not a valid move since d3 is under attack by white king";
}

TEST(ValidatingKingMoveTest, AdvancementUnderAttack11) {
    auto game = std::make_unique<Game>();

    game->parseFEN("8/8/8/8/8/2k5/4K3/8 b - - 0 1");

    auto move10 = game->parseMove("Kd4");

    ASSERT_NE(move10, std::nullopt)
        << "Kd4 is valid notation";

    EXPECT_TRUE(game->isValidMove(*move10))
        << "Kd4 is a valid move";
}

TEST(ApplyingMoveTest, PawnAdvancement) {
    auto game = std::make_unique<Game>();

    // game->applyMove(game->parseMove("e4"));
    game->applyMove(Move{ .piece = Piece::WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 4, .col = 'e' } });

    EXPECT_EQ(game->serializeAsFEN(), "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b - - 0 1")
        << "Serialized board after applying a e4 move";
}
