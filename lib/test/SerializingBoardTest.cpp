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

TEST(SerializingBoardTest, SerializeFEN) {
    auto game = std::make_unique<Game>();

    game->movePiece(Position{ .row = 2, .col = 'e' }, Position{ .row = 4, .col = 'e' });

    EXPECT_EQ(game->serializeAsFEN(), "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w - - 0 1");
}
