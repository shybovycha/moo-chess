#pragma once

#include <array>
#include <deque>
#include <format>
#include <map>
#include <string>
#include <optional>
#include <ostream>
#include <iostream>
#include <vector>

enum class PieceColor {
    BLACK = 'b',
    WHITE = 'w'
};

enum class PieceType {
    NONE = '.',

    PAWN = 'P',
    ROOK = 'R',
    KNIGHT = 'N',
    BISHOP = 'B',
    QUEEN = 'Q',
    KING = 'K'
};

struct CastlingAvailability {
    bool WHITE_KING_SIDE;
    bool WHITE_QUEEN_SIDE;

    bool BLACK_KING_SIDE;
    bool BLACK_QUEEN_SIDE;
};

struct Position {
    int row;
    char col;

    void parse(const std::string& positionString) {
        col = positionString.at(0);
        row = static_cast<int>(positionString.at(1)) - static_cast<int>('1') + 1;
    }

    bool operator==(const Position& other) const = default;

    friend std::ostream& operator<<(std::ostream& os, const Position& pos) {
        return os << std::format("<{1}, {0}>", static_cast<char>(pos.col), pos.row);
    }
};

struct Piece {
    PieceType type;
    PieceColor color;
    Position position;
    bool hasMoved = false;
    bool justMadeDoubleMove = false;
};

struct Move {
    PieceType piece;

    Position from;
    Position to;

    std::optional<PieceType> promotion;

    bool isCapture;
    bool isCastling;

    bool operator==(const Move& other) const = default;
};

template<>
struct std::hash<PieceType> {
    std::size_t operator()(PieceType const& p) const noexcept {
        return std::hash<char>{}(static_cast<std::underlying_type<PieceType>::type>(p));
    }
};

template <>
struct std::formatter<PieceType> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const PieceType& piece, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "{0:c}", static_cast<std::underlying_type<PieceType>::type>(piece));
    }
};

template <>
struct std::formatter<Position> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const Position& position, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "{0:c}{1}", position.col, position.row);
    }
};

template <>
struct std::formatter<Move> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const Move& move, FormatContext& ctx) const {
        if (move.isCastling)
        {
            if (std::abs(move.to.col - move.from.col) > 2)
            {
                return std::format_to(ctx.out(), "O-O-O");
            }
            else
            {
                return std::format_to(ctx.out(), "O-O");
            }
        }

        if (move.isCapture)
        {
            // TODO: add shortening?
            return std::format_to(ctx.out(), "{0}{1}x{2}", move.piece, move.from, move.to);
        }

        // TODO: add distinguishing if two pieces can move to the same square
        return std::format_to(ctx.out(), "{0}{1}", move.piece, move.to);
    }
};

template<>
struct std::hash<Position> {
    std::size_t operator()(Position const& pos) const noexcept {
        std::size_t h1 = std::hash<int>{}(pos.row);
        std::size_t h2 = std::hash<char>{}(pos.col);
        return h1 ^ (h2 << 1);
    }
};

template<>
struct std::hash<Move> {
    std::size_t operator()(Move const& move) const noexcept {
        std::size_t h1 = std::hash<Position>{}(move.from);
        std::size_t h2 = std::hash<Position>{}(move.to);
        std::size_t h3 = std::hash<PieceType>{}(move.piece);
        std::size_t h4 = std::hash<std::optional<PieceType>>{}(move.promotion);
        std::size_t h5 = std::hash<bool>{}(move.isCapture);
        std::size_t h6 = std::hash<bool>{}(move.isCastling);
        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4) ^ (h6 << 5); // or use boost::hash_combine
    }
};

struct MoveComparator {
    bool operator()(const Move& lhs, const Move& rhs) const {
        return std::hash<Move>{}(lhs) < std::hash<Move>{}(rhs);
    }
};

class Board {
public:
    Board();

    Board(std::vector<Piece> pieces) : pieces(pieces) {}

    ~Board() = default;

    // static Board* parseFEN(const std::string& fenString);

public:

    const Piece* getPieceAt(const Position& pos) const;

    bool isPathClear(const Position& start, const Position& end) const;

    bool isKingInCheck(const Piece king) const;

    bool canCastle(const Piece king, const Position to) const;

    bool isValidMove(const Piece piece, const Position to, bool checkCastling = true) const;

    void applyMove(const Piece piece, const Position to);

public:
    std::vector<Piece> pieces;
};
