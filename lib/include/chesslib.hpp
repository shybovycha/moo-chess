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
    Board() {
        pieces = {
            Piece{ PieceType::ROOK,   PieceColor::WHITE, Position{ 1, 'a' } },
            Piece{ PieceType::KNIGHT, PieceColor::WHITE, Position{ 1, 'b' } },
            Piece{ PieceType::BISHOP, PieceColor::WHITE, Position{ 1, 'c' } },
            Piece{ PieceType::QUEEN,  PieceColor::WHITE, Position{ 1, 'd' } },
            Piece{ PieceType::KING,   PieceColor::WHITE, Position{ 1, 'e' } },
            Piece{ PieceType::BISHOP, PieceColor::WHITE, Position{ 1, 'f' } },
            Piece{ PieceType::KNIGHT, PieceColor::WHITE, Position{ 1, 'g' } },
            Piece{ PieceType::ROOK,   PieceColor::WHITE, Position{ 1, 'h' } },

            Piece{ PieceType::PAWN, PieceColor::WHITE, Position{ 2, 'a' } },
            Piece{ PieceType::PAWN, PieceColor::WHITE, Position{ 2, 'b' } },
            Piece{ PieceType::PAWN, PieceColor::WHITE, Position{ 2, 'c' } },
            Piece{ PieceType::PAWN, PieceColor::WHITE, Position{ 2, 'd' } },
            Piece{ PieceType::PAWN, PieceColor::WHITE, Position{ 2, 'e' } },
            Piece{ PieceType::PAWN, PieceColor::WHITE, Position{ 2, 'f' } },
            Piece{ PieceType::PAWN, PieceColor::WHITE, Position{ 2, 'g' } },
            Piece{ PieceType::PAWN, PieceColor::WHITE, Position{ 2, 'h' } },

            Piece{ PieceType::PAWN, PieceColor::BLACK, Position{ 7, 'a' } },
            Piece{ PieceType::PAWN, PieceColor::BLACK, Position{ 7, 'b' } },
            Piece{ PieceType::PAWN, PieceColor::BLACK, Position{ 7, 'c' } },
            Piece{ PieceType::PAWN, PieceColor::BLACK, Position{ 7, 'd' } },
            Piece{ PieceType::PAWN, PieceColor::BLACK, Position{ 7, 'e' } },
            Piece{ PieceType::PAWN, PieceColor::BLACK, Position{ 7, 'f' } },
            Piece{ PieceType::PAWN, PieceColor::BLACK, Position{ 7, 'g' } },
            Piece{ PieceType::PAWN, PieceColor::BLACK, Position{ 7, 'h' } },

            Piece{ PieceType::ROOK,   PieceColor::BLACK, Position{ 8, 'a' } },
            Piece{ PieceType::KNIGHT, PieceColor::BLACK, Position{ 8, 'b' } },
            Piece{ PieceType::BISHOP, PieceColor::BLACK, Position{ 8, 'c' } },
            Piece{ PieceType::QUEEN,  PieceColor::BLACK, Position{ 8, 'd' } },
            Piece{ PieceType::KING,   PieceColor::BLACK, Position{ 8, 'e' } },
            Piece{ PieceType::BISHOP, PieceColor::BLACK, Position{ 8, 'f' } },
            Piece{ PieceType::KNIGHT, PieceColor::BLACK, Position{ 8, 'g' } },
            Piece{ PieceType::ROOK,   PieceColor::BLACK, Position{ 8, 'h' } },
        };
    }

    ~Board() = default;

public:

    const Piece* getPieceAt(const Position& pos) const {
        for (const auto& piece : pieces) {
            if (piece.position == pos) {
                return &piece;
            }
        }

        return nullptr;
    }

    bool isPathClear(const Position& start, const Position& end) const {
        int dcol = end.col - start.col;
        int drow = end.row - start.row;

        int steps = std::max(std::abs(dcol), std::abs(drow));

        int colStep = (dcol != 0) ? dcol / std::abs(dcol) : 0;
        int rowStep = (drow != 0) ? drow / std::abs(drow) : 0;

        for (int i = 1; i < steps; ++i) {
            Position pos = { start.col + i * colStep, start.row + i * rowStep };

            if (getPieceAt(pos) != nullptr) {
                return false;
            }
        }

        return true;
    }

    bool isKingInCheck(const Piece king) const {
        for (const auto& piece : pieces) {
            if (piece.color != king.color && isValidMove(piece, king.position, false)) {
                return true;
            }
        }

        return false;
    }

    bool canCastle(const Piece king, const Position to) const {
        if (king.hasMoved || to.row != king.position.row || std::abs(to.col - king.position.col) != 2) {
            return false;
        }

        int direction = (to.col > king.position.col) ? 1 : -1;

        Position rookPos = { king.position.row, direction > 0 ? 8 : 1 };
        const Piece* rook = getPieceAt(rookPos);

        if (rook == nullptr || rook->type != PieceType::ROOK || rook->hasMoved) {
            return false;
        }

        for (int i = 1; i <= 2; ++i) {
            Position pos = { king.position.row, king.position.col + i * direction };

            if (!isPathClear(king.position, pos) || isKingInCheck(Piece{ king.type, king.color, pos, king.hasMoved })) {
                return false;
            }
        }

        return true;
    }

    bool isValidMove(const Piece piece, const Position to, bool checkCastling = true) const {
        if (to.row < 1 || to.row > 8 || to.col < 'a' || to.col > 'h') {
            return false;
        }

        if (to == piece.position) {
            return false;
        }

        const Piece* target = getPieceAt(to);

        if (target && target->color == piece.color) {
            return false;
        }

        int dcol = to.col - piece.position.col;
        int drow = to.row - piece.position.row;

        switch (piece.type) {
            case PieceType::KING:
                return (std::abs(dcol) <= 1 && std::abs(drow) <= 1) || (checkCastling && canCastle(piece, to));

            case PieceType::QUEEN:
                return (dcol == 0 || drow == 0 || std::abs(dcol) == std::abs(drow)) && isPathClear(piece.position, to);

            case PieceType::BISHOP:
                return (std::abs(dcol) == std::abs(drow)) && isPathClear(piece.position, to);

            case PieceType::ROOK:
                return (dcol == 0 || drow == 0) && isPathClear(piece.position, to);

            case PieceType::KNIGHT:
                return (std::abs(dcol) == 2 && std::abs(drow) == 1) || (std::abs(dcol) == 1 && std::abs(drow) == 2);

            case PieceType::PAWN: {
                int pawnForward = (piece.color == PieceColor::WHITE) ? 1 : -1;

                if (dcol == 0 && drow == pawnForward && !target) {
                    return true;
                }

                if (dcol == 0 && drow == 2 * pawnForward && !piece.hasMoved && !getPieceAt({ piece.position.row + pawnForward, piece.position.col }) && !target) {
                    return true;
                }

                if (std::abs(dcol) == 1 && drow == pawnForward) {
                    // regular capture
                    if (target) {
                        return true;
                    }

                    // en passant
                    const Piece* adjacent = getPieceAt({ piece.position.row, static_cast<char>(piece.position.col + dcol) });

                    return (adjacent && adjacent->type == PieceType::PAWN && adjacent->color != piece.color && adjacent->justMadeDoubleMove);
                }
            }

            default:
                return false;
        }
    }

    void applyMove(const Piece piece, const Position to) {
        if (!isValidMove(piece, to)) {
            return;
        }

        for (auto& p : pieces) {
            if (p.position != piece.position) {
                continue;
            }

            Position from = p.position;

            p.position = to;
            p.hasMoved = true;

            // castling
            if (p.type == PieceType::KING && std::abs(to.col - piece.position.col) == 2) {
                int direction = (to.col > piece.position.col) ? 1 : -1;
                Position oldRookPos = { piece.position.row, direction > 0 ? 8 : 1 };
                Position newRookPos = { piece.position.row, piece.position.col + direction };

                for (auto& rook : pieces) {
                    if (rook.position != oldRookPos) {
                        continue;
                    }

                    rook.position = newRookPos;
                    rook.hasMoved = true;
                    break;
                }
            }

            // pawn
            if (p.type == PieceType::PAWN) {
                if (std::abs(to.row - from.row) == 2) {
                    p.justMadeDoubleMove = true;
                }

                int pawnForward = (piece.color == PieceColor::WHITE) ? 1 : -1;

                if (std::abs(to.col - from.col) == 1 && to.row - from.row == pawnForward && !getPieceAt(to)) {
                    Position capturedPawnPosition = { to.row, from.col };

                    pieces.erase(
                        std::remove_if(
                            pieces.begin(),
                            pieces.end(),
                            [&capturedPawnPosition](const Piece& p) {
                                return p.position.col == capturedPawnPosition.col && p.position.row == capturedPawnPosition.row;
                            }
                        ),
                        pieces.end()
                    );
                }
            }

            break;
        }

        // remove captured piece
        pieces.erase(
            std::remove_if(
                pieces.begin(),
                pieces.end(),
                [&to](const Piece& p) { return p.position == to; }
            ),
            pieces.end()
        );
    }

public:
    std::vector<Piece> pieces;
};
