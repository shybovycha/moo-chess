#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <memory>
#include <exception>
#include <map>

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <tinyxml2.h>

enum Piece {
    NONE = 0,

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

enum class PieceColor {
    BLACK,
    WHITE
};
