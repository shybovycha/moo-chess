#include <string>
#include <iostream>

#include "Game.hpp"

void showHelp(char** argv) {
    std::cout << "Usage: " << argv[0] << " [ARGS]\n\n";
    std::cout << "Possible argument values:\n";
    std::cout << "\t(white | black | random) - find an opponent for a game with me as [side]\n";
    std::cout << "\t(--version | -v) - show version string\n";
    std::cout << "\t(--help | -h) - show this message\n\n";
}

void showNoArgsError() {
    std::cout << "You have not specified any of these options => shutting down.\n\n";
}

void showInvalidArgError() {
    std::cout << "You have not specified any of known options => shutting down.\n\n";
}

void showVersionString() {
    std::cout << "This is a mooChess client, version 0.2 [ALPHA]\n\n";
}

int main(int argc, char** argv) {
    if (argc < 2) {
        showHelp(argv);
        showNoArgsError();

        return 1;
    }

    std::string argv1 = argv[1];

    if (argv1 == "--version" || argv1 == "-v") {
        showVersionString();

        return 0;
    } else if (argv1 == "--help" || argv1 == "-h") {
        showHelp(argv);

        return 0;
    } else if (argv1 == "black" || argv1 == "white" || argv1 == "random") {
        std::unique_ptr<Game> game = std::make_unique<Game>();

        game->start(argv1);
    } else {
        showHelp(argv);
        showInvalidArgError();

        return 1;
    }

    return 0;
}
