#include "CastlingRights.h"

const std::string CastlingRights::castlingFENStrings[ 16 ] =
{
    "-",    // 0b00000000 - none
    "q",    // 0b00000001 - 
    "k",    // 0b00000010 - Q
    "kq",    // 0b00000011 - KQ
    "Q",    // 0b00000100 - 
    "Qq",    // 0b00000101
    "Qk",    // 0b00000110
    "Qkq",    // 0b00000111
    "K",    // 0b00001000
    "Kq",    // 0b00001001
    "Kk",    // 0b00001010
    "Kkq",    // 0b00001011
    "KQ",    // 0b00001100
    "KQq",    // 0b00001101
    "KQk",    // 0b00001110
    "KQkq",    // 0b00001111
};
