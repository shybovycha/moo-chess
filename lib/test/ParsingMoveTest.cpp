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

#include "Game.hpp"

TEST(ParsingMoveTest, ParsingPawnAdvancement) {
    auto game = std::make_unique<Game>();

    auto move = game->parseMove("e4");

    ASSERT_NE(move, std::nullopt);

    EXPECT_EQ(move->isCastling, false);
    EXPECT_EQ(move->isCapture, false);
    EXPECT_EQ(move->piece, WHITE_PAWN);
    EXPECT_THAT(move->from, FieldsAre(Eq(2), Eq('e')));
    EXPECT_THAT(move->to, FieldsAre(Eq(4), Eq('e')));
}

TEST(ParsingMoveTest, ParsingPawnAdvancement2) {
    auto game = std::make_unique<Game>();

    auto move2 = game->parseMove("e3");

    ASSERT_NE(move2, std::nullopt);

    EXPECT_EQ(move2->isCastling, false);
    EXPECT_EQ(move2->isCapture, false);
    EXPECT_EQ(move2->piece, WHITE_PAWN);
    EXPECT_THAT(move2->from, FieldsAre(Eq(2), Eq('e')));
    EXPECT_THAT(move2->to, FieldsAre(Eq(3), Eq('e')));
}

TEST(ParsingMoveTest, ParsingPawnCapture) {
    auto game = std::make_unique<Game>();

    game->parseFEN("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 8");

    auto move = game->parseMove("exd5");

    EXPECT_EQ(move->isCastling, false);
    EXPECT_EQ(move->isCapture, true);
    EXPECT_EQ(move->piece, WHITE_PAWN);
    EXPECT_THAT(move->from, FieldsAre(Eq(4), Eq('e')));
    EXPECT_THAT(move->to, FieldsAre(Eq(5), Eq('d')));
}

TEST(ParsingMoveTest, ParsingEnPassant) {
    auto game = std::make_unique<Game>();

    game->applyMove(*game->parseMove("e4"));
    game->applyMove(*game->parseMove("b6"));
    game->applyMove(*game->parseMove("e5"));
    game->applyMove(*game->parseMove("d5"));

    // essentially setting the board in the position of `rnbqkbnr/p1p1pppp/1p6/3pP3/8/8/PPPP1PPP/RNBQKBNR` with the last move by black, `d6`

    auto move = game->parseMove("exd6");

    EXPECT_EQ(move->isCastling, false);
    EXPECT_EQ(move->isCapture, true);
    EXPECT_EQ(move->piece, WHITE_PAWN);
    EXPECT_THAT(move->from, FieldsAre(Eq(5), Eq('e')));
    EXPECT_THAT(move->to, FieldsAre(Eq(6), Eq('d')));

    EXPECT_TRUE(game->isValidMove(*move));
}

TEST(ParsingMoveTest, DisambiguishingPawnMove) {
    auto game = std::make_unique<Game>();

    game->parseFEN("r3k2r/pp1b1ppp/nqp1pn2/2bp4/N1P1P3/1P1B1N2/PB1PQPPP/R3K2R w KQkq - 0 8");

    auto move1 = game->parseMove("exd5");

    ASSERT_NE(move1, std::nullopt);

    EXPECT_EQ(move1->isCastling, false);
    EXPECT_EQ(move1->isCapture, true);
    EXPECT_EQ(move1->piece, WHITE_PAWN);
    EXPECT_THAT(move1->to, FieldsAre(Eq(5), Eq('d')));
    EXPECT_THAT(move1->from, FieldsAre(Eq(4), Eq('e')));
}

TEST(ParsingMoveTest, DisambiguishingPawnMove2) {
    auto game = std::make_unique<Game>();

    game->parseFEN("r3k2r/pp1b1ppp/nqp1pn2/2bp4/N1P1P3/1P1B1N2/PB1PQPPP/R3K2R w KQkq - 0 8");

    auto move2 = game->parseMove("cxd5");

    ASSERT_NE(move2, std::nullopt);

    EXPECT_EQ(move2->isCastling, false);
    EXPECT_EQ(move2->isCapture, true);
    EXPECT_EQ(move2->piece, WHITE_PAWN);
    EXPECT_THAT(move2->to, FieldsAre(Eq(5), Eq('d')));
    EXPECT_THAT(move2->from, FieldsAre(Eq(4), Eq('c')));
}

TEST(ParsingMoveTest, DisambiguishingPawnMove3) {
    auto game = std::make_unique<Game>();

    game->parseFEN("r3k2r/pp1b1ppp/nqp1pn2/2bp4/N1P1P3/1P1B1N2/PB1PQPPP/R3K2R b KQkq - 0 8");

    auto move3 = game->parseMove("dxe4");

    ASSERT_NE(move3, std::nullopt);

    EXPECT_EQ(move3->isCastling, false);
    EXPECT_EQ(move3->isCapture, true);
    EXPECT_EQ(move3->piece, BLACK_PAWN);
    EXPECT_THAT(move3->to, FieldsAre(Eq(4), Eq('e')));
    EXPECT_THAT(move3->from, FieldsAre(Eq(5), Eq('d')));
}

TEST(ParsingMoveTest, DisambiguishingPawnMove4) {
    auto game = std::make_unique<Game>();

    game->parseFEN("r3k2r/pp1b1ppp/nqp1pn2/2bp4/N1P1P3/1P1B1N2/PB1PQPPP/R3K2R b KQkq - 0 8");

    auto move4 = game->parseMove("dxc4");

    ASSERT_NE(move4, std::nullopt);

    EXPECT_EQ(move4->isCastling, false);
    EXPECT_EQ(move4->isCapture, true);
    EXPECT_EQ(move4->piece, BLACK_PAWN);
    EXPECT_THAT(move4->to, FieldsAre(Eq(4), Eq('c')));
    EXPECT_THAT(move4->from, FieldsAre(Eq(5), Eq('d')));
}

TEST(ParsingMoveTest, ParsingCastling) {
    auto game = std::make_unique<Game>();

    game->parseFEN("rnbqkb1r/pp2pppp/2p2n2/3p4/4P3/3B1N2/PPPP1PPP/RNBQK2R w KQkq - 0 3");

    auto move1 = game->parseMove("O-O");

    ASSERT_NE(move1, std::nullopt);

    EXPECT_EQ(move1->isCastling, true);
    EXPECT_EQ(move1->isCapture, false);
    EXPECT_EQ(move1->piece, WHITE_KING);
    EXPECT_THAT(move1->from, FieldsAre(Eq(1), Eq('e')));
    EXPECT_THAT(move1->to, FieldsAre(Eq(1), Eq('g')));
}

TEST(ParsingMoveTest, ParsingCastling2) {
    auto game = std::make_unique<Game>();

    game->parseFEN("rnbqkb1r/pp2pppp/2p2n2/3p4/4P3/3B1N2/PPPP1PPP/RNBQK2R w KQkq - 0 3");

    auto move2 = game->parseMove("0-0");

    ASSERT_NE(move2, std::nullopt);

    EXPECT_EQ(move2->isCastling, true);
    EXPECT_EQ(move2->isCapture, false);
    EXPECT_EQ(move2->piece, WHITE_KING);
    EXPECT_THAT(move2->from, FieldsAre(Eq(1), Eq('e')));
    EXPECT_THAT(move2->to, FieldsAre(Eq(1), Eq('g')));
}


TEST(ParsingMoveTest, ParsingCastling3) {
    auto game = std::make_unique<Game>();

    game->parseFEN("r3k2r/pp1b1ppp/nqp1pn2/2bp4/N3P3/1P1B1N2/PBPPQPPP/R3K2R b KQkq - 0 8");

    auto move3 = game->parseMove("0-0");

    ASSERT_NE(move3, std::nullopt);

    EXPECT_EQ(move3->isCastling, true);
    EXPECT_EQ(move3->isCapture, false);
    EXPECT_EQ(move3->piece, BLACK_KING);
    EXPECT_THAT(move3->from, FieldsAre(Eq(8), Eq('e')));
    EXPECT_THAT(move3->to, FieldsAre(Eq(8), Eq('g')));
}

TEST(ParsingMoveTest, ParsingCastling4) {
    auto game = std::make_unique<Game>();

    game->parseFEN("r3k2r/pp1b1ppp/nqp1pn2/2bp4/N3P3/1P1B1N2/PBPPQPPP/R3K2R b KQkq - 0 8");

    auto move4 = game->parseMove("0-0-0");

    ASSERT_NE(move4, std::nullopt);

    EXPECT_EQ(move4->isCastling, true);
    EXPECT_EQ(move4->isCapture, false);
    EXPECT_EQ(move4->piece, BLACK_KING);
    EXPECT_THAT(move4->from, FieldsAre(Eq(8), Eq('e')));
    EXPECT_THAT(move4->to, FieldsAre(Eq(8), Eq('b')));
}
