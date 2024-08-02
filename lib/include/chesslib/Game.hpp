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

enum class Piece {
    NONE = '.',

    WHITE_PAWN = 'P',
    WHITE_ROOK = 'R',
    WHITE_KNIGHT = 'N',
    WHITE_BISHOP = 'B',
    WHITE_QUEEN = 'Q',
    WHITE_KING = 'K',

    BLACK_PAWN = 'p',
    BLACK_ROOK = 'r',
    BLACK_KNIGHT = 'n',
    BLACK_BISHOP = 'b',
    BLACK_QUEEN = 'q',
    BLACK_KING = 'k'
};

struct CastlingAvailability {
    bool WHITE_KING_SIDE;
    bool WHITE_QUEEN_SIDE;

    bool BLACK_KING_SIDE;
    bool BLACK_QUEEN_SIDE;
};

struct Position {
    unsigned int row;
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

struct Move {
    Piece piece;

    Position from;
    Position to;

    std::optional<Piece> promotion;

    bool isCapture;
    bool isCastling;

    bool operator==(const Move& other) const = default;
};

template<>
struct std::hash<Piece> {
    std::size_t operator()(Piece const& p) const noexcept {
        return std::hash<char>{}(static_cast<std::underlying_type<Piece>::type>(p));
    }
};

template <>
struct std::formatter<Piece> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const Piece& piece, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "{0:c}", static_cast<std::underlying_type<Piece>::type>(piece));
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
        std::size_t h3 = std::hash<Piece>{}(move.piece);
        std::size_t h4 = std::hash<std::optional<Piece>>{}(move.promotion);
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

class PositionIterator {
public:
    PositionIterator(const Position from, const Position to) : dRow(0), dCol(0) {
        auto beginRow = std::min(from.row, to.row);
        auto beginCol = std::min(static_cast<unsigned int>(from.col), static_cast<unsigned int>(to.col));

        auto endRow = std::max(from.row, to.row);
        auto endCol = std::max(static_cast<unsigned int>(from.col), static_cast<unsigned int>(to.col));

        _begin = Position{ .row = beginRow, .col = static_cast<char>(beginCol) };
        _end = Position{ .row = endRow, .col = static_cast<char>(endCol) };

        _current = _begin;
    }

    Position operator*() const {
        return _current;
    }

    bool operator==(const PositionIterator& it) const {
        return _current == it._current;
    }

    // prefix
    PositionIterator& operator++() {
        _current.row += dRow;
        _current.col = static_cast<char>(_current.col + dCol);
        return *this;
    }

    // postfix
    PositionIterator operator++(int x) {
        PositionIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    bool hasNext() const {
        return _current.row != _end.row &&
            _current.col != _end.col &&
            _current.row >= 1 &&
            _current.row <= 8 &&
            _current.col >= 'a' &&
            _current.col <= 'h';
    }

protected:
    Position _begin;
    Position _end;
    Position _current;

    int dRow;
    int dCol;
};

class RookMoveIterator : public PositionIterator {
public:
    RookMoveIterator(const Position from, const Position to) : PositionIterator(from, to) {
        if (from.col == to.col) {
            dRow = (static_cast<int>(to.row) - static_cast<int>(from.row)) / std::abs(static_cast<int>(to.row) - static_cast<int>(from.row));
        }
        else if (from.row == to.row) {
            dCol = (to.col - from.col) / std::abs(to.col - from.col);
        }
        else {
            throw std::format("Can not create vertical iterator from ({0}, {1}) to ({2}, {3}) - both cols and rows differ, must be same on one dimension", from.row, from.col, to.row, to.col);
        }

        // initialize iterator advanced by one position to prevent unnecessary checks
        ++(*this);
    }
};

class BishopMoveIterator : public PositionIterator {
public:
    BishopMoveIterator(const Position from, const Position to) : PositionIterator(from, to) {
        if (from.col == to.col || from.row == to.row) {
            throw std::format("Can not create diagonal iterator from ({0}, {1}) to ({2}, {3}) - either cols or rows are same, must be different on both dimensions", from.row, from.col, to.row, to.col);
        }

        dCol = (to.col - from.col) / std::abs(to.col - from.col);
        dRow = (static_cast<int>(to.row) - static_cast<int>(from.row)) / std::abs(static_cast<int>(to.row) - static_cast<int>(from.row));

        // initialize iterator advanced by one position to prevent unnecessary checks
        ++(*this);
    }
};

class QueenMoveIterator : public PositionIterator {
public:
    QueenMoveIterator(const Position from, const Position to) : PositionIterator(from, to) {
        if (from.row == to.row) {
            dRow = (static_cast<int>(to.row) - static_cast<int>(from.row)) / std::abs(static_cast<int>(to.row) - static_cast<int>(from.row));
        }
        else if (from.col == to.col) {
            dCol = (to.col - from.col) / std::abs(to.col - from.col);
        }
        else {
            dRow = (static_cast<int>(to.row) - static_cast<int>(from.row)) / std::abs(static_cast<int>(to.row) - static_cast<int>(from.row));
            dCol = (to.col - from.col) / std::abs(to.col - from.col);
        }

        // initialize iterator advanced by one position to prevent unnecessary checks
        ++(*this);
    }
};

class Game {
public:
    Game();

    Game(PieceColor playerColor);

    ~Game() = default;

    void parseFEN(const std::string& FENstring);

    std::string serializeAsFEN() const;

    std::optional<Move> parseMove(const std::string& moveString) const;

    std::string serializeMove(const Move move) const;

    void applyMove(const Move move);

    bool isValidMove(const Move move) const;

public:
    Piece parsePiece(char pieceSymbol) const;

    Piece pieceAt(int row, char col) const;
    Piece pieceAt(const Position pos) const;

    void setPieceAt(int row, char col, const Piece piece);
    void setPieceAt(const Position pos, const Piece piece);

    void movePiece(Position from, Position to);

    bool opponentPieceAt(const Position pos) const;
    bool opponentPieceAt(const Position pos, const PieceColor currentPlayerColor) const;

    bool allyPieceAt(const Position pos) const;
    bool allyPieceAt(const Position pos, const PieceColor currentPlayerColor) const;

    bool canOpponentMoveTo(const Position pos) const;

    bool isValidMove(const Move move, std::map<Move, bool, MoveComparator>& cache, const PieceColor currentPlayerColor) const;

    bool canOpponentMoveTo(const Position pos, std::map<Move, bool, MoveComparator>& cache, const PieceColor currentPlayerColor) const;

public:
    std::array<std::array<Piece, 8>, 8> pieces;
    PieceColor currentPlayer;
    CastlingAvailability castlingAvailability;
    std::deque<Move> moveHistory;
};
