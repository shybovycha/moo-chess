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

TEST(FENParsingTest, DefaultSetup) {
    auto game = std::make_unique<Game>();

    EXPECT_EQ(game->pieceAt(1, 'a'), WHITE_ROOK);
    EXPECT_EQ(game->pieceAt(1, 'b'), WHITE_KNIGHT);
    EXPECT_EQ(game->pieceAt(1, 'c'), WHITE_BISHOP);
    EXPECT_EQ(game->pieceAt(1, 'd'), WHITE_QUEEN);
    EXPECT_EQ(game->pieceAt(1, 'e'), WHITE_KING);
    EXPECT_EQ(game->pieceAt(1, 'f'), WHITE_BISHOP);
    EXPECT_EQ(game->pieceAt(1, 'g'), WHITE_KNIGHT);
    EXPECT_EQ(game->pieceAt(1, 'h'), WHITE_ROOK);

    EXPECT_EQ(game->pieceAt(8, 'a'), BLACK_ROOK);
    EXPECT_EQ(game->pieceAt(8, 'b'), BLACK_KNIGHT);
    EXPECT_EQ(game->pieceAt(8, 'c'), BLACK_BISHOP);
    EXPECT_EQ(game->pieceAt(8, 'd'), BLACK_QUEEN);
    EXPECT_EQ(game->pieceAt(8, 'e'), BLACK_KING);
    EXPECT_EQ(game->pieceAt(8, 'f'), BLACK_BISHOP);
    EXPECT_EQ(game->pieceAt(8, 'g'), BLACK_KNIGHT);
    EXPECT_EQ(game->pieceAt(8, 'h'), BLACK_ROOK);

    EXPECT_EQ(game->pieceAt(2, 'a'), WHITE_PAWN);
    EXPECT_EQ(game->pieceAt(2, 'b'), WHITE_PAWN);
    EXPECT_EQ(game->pieceAt(2, 'c'), WHITE_PAWN);
    EXPECT_EQ(game->pieceAt(2, 'd'), WHITE_PAWN);
    EXPECT_EQ(game->pieceAt(2, 'e'), WHITE_PAWN);
    EXPECT_EQ(game->pieceAt(2, 'f'), WHITE_PAWN);
    EXPECT_EQ(game->pieceAt(2, 'g'), WHITE_PAWN);
    EXPECT_EQ(game->pieceAt(2, 'h'), WHITE_PAWN);

    EXPECT_EQ(game->pieceAt(7, 'a'), BLACK_PAWN);
    EXPECT_EQ(game->pieceAt(7, 'b'), BLACK_PAWN);
    EXPECT_EQ(game->pieceAt(7, 'c'), BLACK_PAWN);
    EXPECT_EQ(game->pieceAt(7, 'd'), BLACK_PAWN);
    EXPECT_EQ(game->pieceAt(7, 'e'), BLACK_PAWN);
    EXPECT_EQ(game->pieceAt(7, 'f'), BLACK_PAWN);
    EXPECT_EQ(game->pieceAt(7, 'g'), BLACK_PAWN);
    EXPECT_EQ(game->pieceAt(7, 'h'), BLACK_PAWN);

    EXPECT_EQ(game->pieceAt(3, 'a'), NONE);
    EXPECT_EQ(game->pieceAt(3, 'b'), NONE);
    EXPECT_EQ(game->pieceAt(3, 'c'), NONE);
    EXPECT_EQ(game->pieceAt(3, 'd'), NONE);
    EXPECT_EQ(game->pieceAt(3, 'e'), NONE);
    EXPECT_EQ(game->pieceAt(3, 'f'), NONE);
    EXPECT_EQ(game->pieceAt(3, 'g'), NONE);
    EXPECT_EQ(game->pieceAt(3, 'h'), NONE);

    EXPECT_EQ(game->pieceAt(4, 'a'), NONE);
    EXPECT_EQ(game->pieceAt(4, 'b'), NONE);
    EXPECT_EQ(game->pieceAt(4, 'c'), NONE);
    EXPECT_EQ(game->pieceAt(4, 'd'), NONE);
    EXPECT_EQ(game->pieceAt(4, 'e'), NONE);
    EXPECT_EQ(game->pieceAt(4, 'f'), NONE);
    EXPECT_EQ(game->pieceAt(4, 'g'), NONE);
    EXPECT_EQ(game->pieceAt(4, 'h'), NONE);

    EXPECT_EQ(game->pieceAt(5, 'a'), NONE);
    EXPECT_EQ(game->pieceAt(5, 'b'), NONE);
    EXPECT_EQ(game->pieceAt(5, 'c'), NONE);
    EXPECT_EQ(game->pieceAt(5, 'd'), NONE);
    EXPECT_EQ(game->pieceAt(5, 'e'), NONE);
    EXPECT_EQ(game->pieceAt(5, 'f'), NONE);
    EXPECT_EQ(game->pieceAt(5, 'g'), NONE);
    EXPECT_EQ(game->pieceAt(5, 'h'), NONE);

    EXPECT_EQ(game->pieceAt(6, 'a'), NONE);
    EXPECT_EQ(game->pieceAt(6, 'b'), NONE);
    EXPECT_EQ(game->pieceAt(6, 'c'), NONE);
    EXPECT_EQ(game->pieceAt(6, 'd'), NONE);
    EXPECT_EQ(game->pieceAt(6, 'e'), NONE);
    EXPECT_EQ(game->pieceAt(6, 'f'), NONE);
    EXPECT_EQ(game->pieceAt(6, 'g'), NONE);
    EXPECT_EQ(game->pieceAt(6, 'h'), NONE);

    EXPECT_EQ(game->currentPlayer, WHITE);

    EXPECT_EQ(game->castlingAvailability.BLACK_KING_SIDE, false);
    EXPECT_EQ(game->castlingAvailability.BLACK_QUEEN_SIDE, false);
    EXPECT_EQ(game->castlingAvailability.WHITE_KING_SIDE, false);
    EXPECT_EQ(game->castlingAvailability.WHITE_QUEEN_SIDE, false);
}

TEST(FENParsingTest, ParsingCastlingAvailability) {
    auto game = std::make_unique<Game>();

    game->parseFEN("r3k2r/pp1b1ppp/nqp1pn2/2bp4/N1P1P3/1P1B1N2/PB1PQPPP/R3K2R b kq - 0 8");

    EXPECT_EQ(game->castlingAvailability.BLACK_KING_SIDE, true);
    EXPECT_EQ(game->castlingAvailability.BLACK_QUEEN_SIDE, true);
    EXPECT_EQ(game->castlingAvailability.WHITE_KING_SIDE, false);
    EXPECT_EQ(game->castlingAvailability.WHITE_QUEEN_SIDE, false);
}

TEST(FENParsingTest, ParsingCastlingAvailability2) {
    auto game = std::make_unique<Game>();

    game->parseFEN("r3k2r/pp1b1ppp/nqp1pn2/2bp4/N1P1P3/1P1B1N2/PB1PQPPP/R3K2R b - - 0 8");

    EXPECT_EQ(game->castlingAvailability.BLACK_KING_SIDE, false);
    EXPECT_EQ(game->castlingAvailability.BLACK_QUEEN_SIDE, false);
    EXPECT_EQ(game->castlingAvailability.WHITE_KING_SIDE, false);
    EXPECT_EQ(game->castlingAvailability.WHITE_QUEEN_SIDE, false);
}

TEST(FENParsingTest, ParsingCastlingAvailability3) {
    auto game = std::make_unique<Game>();

    game->parseFEN("r3k2r/pp1b1ppp/nqp1pn2/2bp4/N1P1P3/1P1B1N2/PB1PQPPP/R3K2R b kK - 0 8");

    EXPECT_EQ(game->castlingAvailability.BLACK_KING_SIDE, true);
    EXPECT_EQ(game->castlingAvailability.BLACK_QUEEN_SIDE, false);
    EXPECT_EQ(game->castlingAvailability.WHITE_KING_SIDE, true);
    EXPECT_EQ(game->castlingAvailability.WHITE_QUEEN_SIDE, false);
}

TEST(FENParsingTest, ParsingCastlingAvailability4) {
    auto game = std::make_unique<Game>();

    game->parseFEN("r3k2r/pp1b1ppp/nqp1pn2/2bp4/N1P1P3/1P1B1N2/PB1PQPPP/R3K2R w Qq - 0 8");

    EXPECT_EQ(game->castlingAvailability.BLACK_KING_SIDE, false);
    EXPECT_EQ(game->castlingAvailability.BLACK_QUEEN_SIDE, true);
    EXPECT_EQ(game->castlingAvailability.WHITE_KING_SIDE, false);
    EXPECT_EQ(game->castlingAvailability.WHITE_QUEEN_SIDE, true);
}

TEST(FENParsingTest, ParsingCastlingAvailability5) {
    auto game = std::make_unique<Game>();

    game->parseFEN("r3k2r/pp1b1ppp/nqp1pn2/2bp4/N1P1P3/1P1B1N2/PB1PQPPP/R3K2R w KQkq - 0 8");

    EXPECT_EQ(game->castlingAvailability.BLACK_KING_SIDE, true);
    EXPECT_EQ(game->castlingAvailability.BLACK_QUEEN_SIDE, true);
    EXPECT_EQ(game->castlingAvailability.WHITE_KING_SIDE, true);
    EXPECT_EQ(game->castlingAvailability.WHITE_QUEEN_SIDE, true);
}

TEST(FENParsingTest, ParsingCurrentPlayerBlack) {
    auto game = std::make_unique<Game>();

    game->parseFEN("r3k2r/pp1b1ppp/nqp1pn2/2bp4/N1P1P3/1P1B1N2/PB1PQPPP/R3K2R b kq - 0 8");

    EXPECT_EQ(game->currentPlayer, BLACK);
}

TEST(FENParsingTest, ParsingCurrentPlayerWhite) {
    auto game = std::make_unique<Game>();

    game->parseFEN("r3k2r/pp1b1ppp/nqp1pn2/2bp4/N1P1P3/1P1B1N2/PB1PQPPP/R3K2R w Qq - 0 8");

    EXPECT_EQ(game->currentPlayer, WHITE);
}
