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

    EXPECT_EQ(game->pieceAt(1, 'a'), Piece::WHITE_ROOK);
    EXPECT_EQ(game->pieceAt(1, 'b'), Piece::WHITE_KNIGHT);
    EXPECT_EQ(game->pieceAt(1, 'c'), Piece::WHITE_BISHOP);
    EXPECT_EQ(game->pieceAt(1, 'd'), Piece::WHITE_QUEEN);
    EXPECT_EQ(game->pieceAt(1, 'e'), Piece::WHITE_KING);
    EXPECT_EQ(game->pieceAt(1, 'f'), Piece::WHITE_BISHOP);
    EXPECT_EQ(game->pieceAt(1, 'g'), Piece::WHITE_KNIGHT);
    EXPECT_EQ(game->pieceAt(1, 'h'), Piece::WHITE_ROOK);

    EXPECT_EQ(game->pieceAt(8, 'a'), Piece::BLACK_ROOK);
    EXPECT_EQ(game->pieceAt(8, 'b'), Piece::BLACK_KNIGHT);
    EXPECT_EQ(game->pieceAt(8, 'c'), Piece::BLACK_BISHOP);
    EXPECT_EQ(game->pieceAt(8, 'd'), Piece::BLACK_QUEEN);
    EXPECT_EQ(game->pieceAt(8, 'e'), Piece::BLACK_KING);
    EXPECT_EQ(game->pieceAt(8, 'f'), Piece::BLACK_BISHOP);
    EXPECT_EQ(game->pieceAt(8, 'g'), Piece::BLACK_KNIGHT);
    EXPECT_EQ(game->pieceAt(8, 'h'), Piece::BLACK_ROOK);

    EXPECT_EQ(game->pieceAt(2, 'a'), Piece::WHITE_PAWN);
    EXPECT_EQ(game->pieceAt(2, 'b'), Piece::WHITE_PAWN);
    EXPECT_EQ(game->pieceAt(2, 'c'), Piece::WHITE_PAWN);
    EXPECT_EQ(game->pieceAt(2, 'd'), Piece::WHITE_PAWN);
    EXPECT_EQ(game->pieceAt(2, 'e'), Piece::WHITE_PAWN);
    EXPECT_EQ(game->pieceAt(2, 'f'), Piece::WHITE_PAWN);
    EXPECT_EQ(game->pieceAt(2, 'g'), Piece::WHITE_PAWN);
    EXPECT_EQ(game->pieceAt(2, 'h'), Piece::WHITE_PAWN);

    EXPECT_EQ(game->pieceAt(7, 'a'), Piece::BLACK_PAWN);
    EXPECT_EQ(game->pieceAt(7, 'b'), Piece::BLACK_PAWN);
    EXPECT_EQ(game->pieceAt(7, 'c'), Piece::BLACK_PAWN);
    EXPECT_EQ(game->pieceAt(7, 'd'), Piece::BLACK_PAWN);
    EXPECT_EQ(game->pieceAt(7, 'e'), Piece::BLACK_PAWN);
    EXPECT_EQ(game->pieceAt(7, 'f'), Piece::BLACK_PAWN);
    EXPECT_EQ(game->pieceAt(7, 'g'), Piece::BLACK_PAWN);
    EXPECT_EQ(game->pieceAt(7, 'h'), Piece::BLACK_PAWN);

    EXPECT_EQ(game->pieceAt(3, 'a'), Piece::NONE);
    EXPECT_EQ(game->pieceAt(3, 'b'), Piece::NONE);
    EXPECT_EQ(game->pieceAt(3, 'c'), Piece::NONE);
    EXPECT_EQ(game->pieceAt(3, 'd'), Piece::NONE);
    EXPECT_EQ(game->pieceAt(3, 'e'), Piece::NONE);
    EXPECT_EQ(game->pieceAt(3, 'f'), Piece::NONE);
    EXPECT_EQ(game->pieceAt(3, 'g'), Piece::NONE);
    EXPECT_EQ(game->pieceAt(3, 'h'), Piece::NONE);

    EXPECT_EQ(game->pieceAt(4, 'a'), Piece::NONE);
    EXPECT_EQ(game->pieceAt(4, 'b'), Piece::NONE);
    EXPECT_EQ(game->pieceAt(4, 'c'), Piece::NONE);
    EXPECT_EQ(game->pieceAt(4, 'd'), Piece::NONE);
    EXPECT_EQ(game->pieceAt(4, 'e'), Piece::NONE);
    EXPECT_EQ(game->pieceAt(4, 'f'), Piece::NONE);
    EXPECT_EQ(game->pieceAt(4, 'g'), Piece::NONE);
    EXPECT_EQ(game->pieceAt(4, 'h'), Piece::NONE);

    EXPECT_EQ(game->pieceAt(5, 'a'), Piece::NONE);
    EXPECT_EQ(game->pieceAt(5, 'b'), Piece::NONE);
    EXPECT_EQ(game->pieceAt(5, 'c'), Piece::NONE);
    EXPECT_EQ(game->pieceAt(5, 'd'), Piece::NONE);
    EXPECT_EQ(game->pieceAt(5, 'e'), Piece::NONE);
    EXPECT_EQ(game->pieceAt(5, 'f'), Piece::NONE);
    EXPECT_EQ(game->pieceAt(5, 'g'), Piece::NONE);
    EXPECT_EQ(game->pieceAt(5, 'h'), Piece::NONE);

    EXPECT_EQ(game->pieceAt(6, 'a'), Piece::NONE);
    EXPECT_EQ(game->pieceAt(6, 'b'), Piece::NONE);
    EXPECT_EQ(game->pieceAt(6, 'c'), Piece::NONE);
    EXPECT_EQ(game->pieceAt(6, 'd'), Piece::NONE);
    EXPECT_EQ(game->pieceAt(6, 'e'), Piece::NONE);
    EXPECT_EQ(game->pieceAt(6, 'f'), Piece::NONE);
    EXPECT_EQ(game->pieceAt(6, 'g'), Piece::NONE);
    EXPECT_EQ(game->pieceAt(6, 'h'), Piece::NONE);

    EXPECT_EQ(game->currentPlayer, PieceColor::WHITE);

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

    EXPECT_EQ(game->currentPlayer, PieceColor::BLACK);
}

TEST(FENParsingTest, ParsingCurrentPlayerWhite) {
    auto game = std::make_unique<Game>();

    game->parseFEN("r3k2r/pp1b1ppp/nqp1pn2/2bp4/N1P1P3/1P1B1N2/PB1PQPPP/R3K2R w Qq - 0 8");

    EXPECT_EQ(game->currentPlayer, PieceColor::WHITE);
}
