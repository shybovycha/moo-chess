#pragma once

#include "common.hpp"
#include "Config.hpp"

class BadResponse : public std::exception {
public:
    BadResponse(int status) : status(status) {}

    virtual const char* what() const throw() {
        std::ostringstream msg;

        msg << "Bad HTTP response status: " << status;

        return msg.str().c_str();
    }

protected:
    int status;
};

enum class PieceColor {
    BLACK,
    WHITE
};

class PromoteResponse {
public:
    static PromoteResponse black(std::string clientId) {
        return PromoteResponse(clientId, PieceColor::BLACK);
    }

    static PromoteResponse white(std::string clientId) {
        return PromoteResponse(clientId, PieceColor::WHITE);
    }

    std::string getClientId() const {
        return clientId;
    }

    PieceColor getColor() const {
        return color;
    }

private:
    PromoteResponse(std::string clientId, PieceColor color) : clientId(clientId), color(color) {}

    std::string clientId;
    PieceColor color;
};


enum class MoveValidationResult {
    UNKNOWN = -1,
    INVALID_MOVE = 0, // invalid move
    NORMAL_MOVE = 1, // normal move - move piece from [from] squasre to the [to] square
    CASTLING_KING_SIDE = 4, // castling (swap rook and king for 3 squares)
    CASTLING_QUEEN_SIDE = 5, // castling (swap rook and king for 2 squares)
    EN_PASSANT = 8, // en passant (remove neighbour pawn and place player's one as it captured other when it moves one square less)
    PROMOTE_PAWN_TO_QUEEN = 16, // promote pawn to queen
    PROMOTE_PAWN_TO_BISHOP = 17, // promote pawn to bishop
    PROMOTE_PAWN_TO_KNIGHT = 18, // promote pawn to knight
    PROMOTE_PAWN_TO_ROOK = 19, // promote pawn to rook
    CHECK = 32, // check
    CHECKMATE = 64 // checkmate
};

enum class StatusResponse {
    UNKNOWN = -1,
    NO_UPDATES = 0, // no new moves or no response
    WAITING_FOR_OPPONENT = 1, // opponent's turn
    PLAYING_TURN = 2, // user's turn
    UNDER_CHECK = 3, // opponent's check
    UNDER_CHECKMATE = 4, // opponent's checkmate
    CHECK = 5, // user's ckeck
    CHECKMATE = 6, // user's checkmate
    OPPONENT_CASTLING_KING_SIDE = 7, // opponent's castling short side
    OPPONENT_CASTLING_QUEEN_SIDE = 8, // opponent's castling long side
    START_AS_BLACK = 12, // set up game with whites on the top
    START_AS_WHITE = 17 // set up game with whites on the bottom
};

enum class FindSideResponse {
    UNKNOWN = -1,
    WHITE = 2,
    BLACK = 7
};

enum class GameState {
    WAITING_FOR_OPPONENT = 0, // opponent's turn or any other lock mode
    MAKING_TURN = 1, // my turn
    UNDER_CHECK = 2, // check for me
    UNDER_CHECKMATE = 4 // checkmate for me
};

class ChessClient {
public:
    ChessClient(const ApplicationConfig& config);

    MoveValidationResult validateMove(const std::string& clientId, const sf::Vector2i& from, const sf::Vector2i& to);

    StatusResponse queryServer(const std::string& clientId, sf::Vector2i* from, sf::Vector2i* to);

    FindSideResponse findSide();

    PromoteResponse promoteSelf(const std::string& side);

    bool waitForOpponent(const std::string& clientId);

private:
    std::string formatQueryString(std::map<std::string, std::string> params);

    std::string sendRequest(std::string url);

    std::string sendRequest(std::string url, std::map<std::string, std::string> params);

    std::string sendRequest(std::string url, std::string body);

private:
    ApplicationConfig config;
};
