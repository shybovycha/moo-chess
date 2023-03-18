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

#include "Game.hpp"

TEST(FENTest, BoardParsing) {
    auto game = std::make_unique<Game>();

    game->parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    EXPECT_EQ(game->currentPlayer, WHITE);
    
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

    EXPECT_EQ(game->castlingAvailability.BLACK_KING_SIDE, true);
    EXPECT_EQ(game->castlingAvailability.BLACK_QUEEN_SIDE, true);
    EXPECT_EQ(game->castlingAvailability.WHITE_KING_SIDE, true);
    EXPECT_EQ(game->castlingAvailability.WHITE_QUEEN_SIDE, true);

    game->parseFEN("r3k2r/pp1b1ppp/nqp1pn2/2bp4/N1P1P3/1P1B1N2/PB1PQPPP/R3K2R b kq - 0 8");

    EXPECT_EQ(game->currentPlayer, BLACK);

    EXPECT_EQ(game->castlingAvailability.BLACK_KING_SIDE, true);
    EXPECT_EQ(game->castlingAvailability.BLACK_QUEEN_SIDE, true);
    EXPECT_EQ(game->castlingAvailability.WHITE_KING_SIDE, false);
    EXPECT_EQ(game->castlingAvailability.WHITE_QUEEN_SIDE, false);

    game->parseFEN("r3k2r/pp1b1ppp/nqp1pn2/2bp4/N1P1P3/1P1B1N2/PB1PQPPP/R3K2R b - - 0 8");

    EXPECT_EQ(game->currentPlayer, BLACK);

    EXPECT_EQ(game->castlingAvailability.BLACK_KING_SIDE, false);
    EXPECT_EQ(game->castlingAvailability.BLACK_QUEEN_SIDE, false);
    EXPECT_EQ(game->castlingAvailability.WHITE_KING_SIDE, false);
    EXPECT_EQ(game->castlingAvailability.WHITE_QUEEN_SIDE, false);

    game->parseFEN("r3k2r/pp1b1ppp/nqp1pn2/2bp4/N1P1P3/1P1B1N2/PB1PQPPP/R3K2R b kK - 0 8");

    EXPECT_EQ(game->currentPlayer, BLACK);

    EXPECT_EQ(game->castlingAvailability.BLACK_KING_SIDE, true);
    EXPECT_EQ(game->castlingAvailability.BLACK_QUEEN_SIDE, false);
    EXPECT_EQ(game->castlingAvailability.WHITE_KING_SIDE, true);
    EXPECT_EQ(game->castlingAvailability.WHITE_QUEEN_SIDE, false);

    game->parseFEN("r3k2r/pp1b1ppp/nqp1pn2/2bp4/N1P1P3/1P1B1N2/PB1PQPPP/R3K2R w Qq - 0 8");

    EXPECT_EQ(game->currentPlayer, WHITE);

    EXPECT_EQ(game->castlingAvailability.BLACK_KING_SIDE, false);
    EXPECT_EQ(game->castlingAvailability.BLACK_QUEEN_SIDE, true);
    EXPECT_EQ(game->castlingAvailability.WHITE_KING_SIDE, false);
    EXPECT_EQ(game->castlingAvailability.WHITE_QUEEN_SIDE, true);
}

TEST(SerializingBoardTest, SerializeFEN) {
    auto game = std::make_unique<Game>();

    game->movePiece(Position{ .row = 2, .col = 'e' }, Position{ .row = 4, .col = 'e' });

    EXPECT_EQ(game->serializeAsFEN(), "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w - - 0 1");
}

TEST(ParsingMoveTest, ParsingPawnMove) {
    auto game = std::make_unique<Game>();

    auto move = game->parseMove("e4");

    EXPECT_THAT(move, Optional(Field(&Move::isCastling, Eq(false))));
    EXPECT_THAT(move, Optional(Field(&Move::isCapture, Eq(false))));
    EXPECT_THAT(move, Optional(Field(&Move::piece, Eq(WHITE_PAWN))));
    EXPECT_THAT(move, Optional(Field(&Move::to, FieldsAre(Eq(4), Eq('e')))));
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

    auto move2 = game->parseMove("cxd5");

    ASSERT_NE(move2, std::nullopt);

    EXPECT_EQ(move2->isCastling, false);
    EXPECT_EQ(move2->isCapture, true);
    EXPECT_EQ(move2->piece, WHITE_PAWN);
    EXPECT_THAT(move2->to, FieldsAre(Eq(5), Eq('d')));
    EXPECT_THAT(move2->from, FieldsAre(Eq(4), Eq('c')));

    game->parseFEN("r3k2r/pp1b1ppp/nqp1pn2/2bp4/N1P1P3/1P1B1N2/PB1PQPPP/R3K2R b KQkq - 0 8");

    auto move3 = game->parseMove("dxe4");

    ASSERT_NE(move3, std::nullopt);

    EXPECT_EQ(move3->isCastling, false);
    EXPECT_EQ(move3->isCapture, true);
    EXPECT_EQ(move3->piece, BLACK_PAWN);
    EXPECT_THAT(move3->to, FieldsAre(Eq(4), Eq('e')));
    EXPECT_THAT(move3->from, FieldsAre(Eq(5), Eq('d')));

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

    auto move2 = game->parseMove("0-0");

    ASSERT_NE(move2, std::nullopt);

    EXPECT_EQ(move2->isCastling, true);
    EXPECT_EQ(move2->isCapture, false);
    EXPECT_EQ(move2->piece, WHITE_KING);
    EXPECT_THAT(move2->from, FieldsAre(Eq(1), Eq('e')));
    EXPECT_THAT(move2->to, FieldsAre(Eq(1), Eq('g')));

    game->parseFEN("r3k2r/pp1b1ppp/nqp1pn2/2bp4/N3P3/1P1B1N2/PBPPQPPP/R3K2R b KQkq - 0 8");

    auto move3 = game->parseMove("0-0");

    ASSERT_NE(move3, std::nullopt);

    EXPECT_THAT(move3->isCastling, Eq(true));
    EXPECT_THAT(move3->isCapture, Eq(false));
    EXPECT_THAT(move3->piece, Eq(BLACK_KING));
    EXPECT_THAT(move3->from, FieldsAre(Eq(8), Eq('e')));
    EXPECT_THAT(move3->to, FieldsAre(Eq(8), Eq('g')));

    auto move4 = game->parseMove("0-0-0");

    ASSERT_NE(move4, std::nullopt);

    EXPECT_THAT(move4->isCastling, Eq(true));
    EXPECT_THAT(move4->isCapture, Eq(false));
    EXPECT_THAT(move4->piece, Eq(BLACK_KING));
    EXPECT_THAT(move4->from, FieldsAre(Eq(8), Eq('e')));
    EXPECT_THAT(move4->to, FieldsAre(Eq(8), Eq('b')));
}

TEST(ValidatingMoveTest, PawnMovement) {
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

TEST(ApplyingMoveTest, PawnAdvancement) {
    auto game = std::make_unique<Game>();

    // game->applyMove(game->parseMove("e4"));
    game->applyMove(Move{ .piece = WHITE_PAWN, .from = Position{.row = 2, .col = 'e' }, .to = Position{.row = 4, .col = 'e' } });

    EXPECT_EQ(game->serializeAsFEN(), "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w - - 0 1");
}

int main() {
    testing::InitGoogleTest();

    RUN_ALL_TESTS();
}
