#include <string>

struct ServerConfig {
    std::string host;
    std::string gameStartedURI;
    std::string searchURI;
    std::string queryingURI;
    std::string moveValidationURI;
    std::string findSideURI;
};

struct ClientConfig {
    std::string dataFolderPath;
};

struct ApplicationConfig {
    ServerConfig server;
    ClientConfig game;
};
