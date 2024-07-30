#include "ChessClient.hpp"

ChessClient::ChessClient(const ApplicationConfig& config) : config(config) {}

MoveValidationResult ChessClient::validateMove(const std::string& clientId, const sf::Vector2i& from, const sf::Vector2i& to) {
    std::map<std::string, std::string> params = {
        { "client", clientId },
        { "from", std::to_string(static_cast<int>('a') + from.x) + std::to_string(from.y + 1) },
        { "to", std::to_string(static_cast<int>('a') + to.x) + std::to_string(to.y + 1) }
    };

    std::string response = sendRequest(config.server.moveValidationURI, params);

    int result = -1;

    std::istringstream(response) >> result;

    return static_cast<MoveValidationResult>(result);
}

StatusResponse ChessClient::queryServer(const std::string& clientId, sf::Vector2i* from, sf::Vector2i* to) {
    std::map<std::string, std::string> params = {
        { "client", clientId }
    };

    std::string response = sendRequest(config.server.queryingURI, params);

    auto stream = std::istringstream(response);
    int tmp = static_cast<int>(StatusResponse::UNKNOWN);

    stream >> tmp;

    StatusResponse result{ tmp };

    if (result == StatusResponse::PLAYING_TURN || result == StatusResponse::OPPONENT_CASTLING_KING_SIDE) {
        int y1 = -1, y2 = -1;
        char x1 = 0, x2 = 0;

        stream >> x1 >> y1 >> x2 >> y2;

        *from = sf::Vector2i((x1 - static_cast<int>('a')), y1 - 1);
        *to = sf::Vector2i((x2 - static_cast<int>('a')), y2 - 1);
    }

    return static_cast<StatusResponse>(result);
}

FindSideResponse ChessClient::findSide() {
    std::cout << "Sending request to find side...\n";

    std::string response = sendRequest(config.server.findSideURI);

    int tmp = static_cast<int>(FindSideResponse::UNKNOWN);

    std::istringstream(response) >> tmp;

    return static_cast<FindSideResponse>(tmp);

    /*if (tmp == static_cast<int>(FindSideResponse::WHITE)) {
        return PieceColor::WHITE;
    }
    else if (tmp == static_cast<int>(FindSideResponse::BLACK)) {
        return PieceColor::BLACK;
    }
    else {
        std::cerr << "Server error while choosing your side\n";

        return PieceColor::BLACK;
    }*/
}

//PromoteResponse ChessClient::promoteSelf(PieceColor color) {
//    if (color == PieceColor::WHITE) {
//        return promoteSelf("white");
//    }
//    else {
//        return promoteSelf("black");
//    }
//}

PromoteResponse ChessClient::promoteSelf(const PieceColor playerColor) {
    std::cout << "Sending request to create a game...\n";

    std::map<std::string, std::string> params = {
        { "side", playerColor == PieceColor::WHITE ? "white" : "black" }
    };

    auto response = sendRequest(config.server.searchURI, params);
    auto clientId = response.substr(1, response.length() - 1);

    if (response[0] == '!') {
        return PromoteResponse::black(clientId);
    }
    else {
        return PromoteResponse::white(clientId);
    }
}

bool ChessClient::waitForOpponent(const std::string& clientId) {
    std::map<std::string, std::string> params = {
        { "client", clientId }
    };

    std::string response = sendRequest(config.server.gameStartedURI, params);

    int result = -1;

    std::istringstream(response) >> result;

    return (result == 1);
}

std::string ChessClient::formatQueryString(std::map<std::string, std::string> params) {
    std::string result;

    for (auto const& [key, value] : params) {
        if (!result.empty()) {
            result += "&";
        }

        result += key + "=" + value;
    }

    return result;
}

std::string ChessClient::sendRequest(std::string url) {
    return sendRequest(url, "");
}

std::string ChessClient::sendRequest(std::string url, std::map<std::string, std::string> params) {
    return sendRequest(url, formatQueryString(params));
}

std::string ChessClient::sendRequest(std::string url, std::string body) {
    std::unique_ptr<sf::Http> http(new sf::Http());
    http->setHost(config.server.host);

    std::unique_ptr<sf::Http::Request> request(new sf::Http::Request);
    request->setMethod(sf::Http::Request::Post);
    request->setUri(url);

    request->setBody(body);
    request->setHttpVersion(1, 0);

    auto response = std::make_unique<sf::Http::Response>(http->sendRequest(*request));

    if (response->getStatus() != sf::Http::Response::Status::Ok) {
        throw BadResponse(response->getStatus());
    }

    return response->getBody();
}
