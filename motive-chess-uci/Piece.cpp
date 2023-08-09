#include "Piece.h"

// Use two bits for color to allow for 'unspecified/unknown' as that is what we get in the UCI "moves" block for position
const unsigned char WHITE = 0b01 << 3;
const unsigned char BLACK = 0b10 << 3;
const unsigned char NO_BW = 0b00 << 3; // NO COLOR

// All the pieces, without colors, and with a special value for 'no piece'
const unsigned char X = 0b000; // NO PIECE
const unsigned char K = 0b110;
const unsigned char Q = 0b101;
const unsigned char R = 0b100;
const unsigned char B = 0b011;
const unsigned char N = 0b010;
const unsigned char P = 0b001;

// Actual pieces
unsigned char const Piece::NX = NO_BW | X;
unsigned char const Piece::WK = WHITE | K;
unsigned char const Piece::WQ = WHITE | Q;
unsigned char const Piece::WR = WHITE | R;
unsigned char const Piece::WB = WHITE | B;
unsigned char const Piece::WN = WHITE | N;
unsigned char const Piece::WP = WHITE | P;
unsigned char const Piece::BK = BLACK | K;
unsigned char const Piece::BQ = BLACK | Q;
unsigned char const Piece::BR = BLACK | R;
unsigned char const Piece::BB = BLACK | B;
unsigned char const Piece::BN = BLACK | N;
unsigned char const Piece::BP = BLACK | P;
                                

/*
const Piece Piece::wk( Piece::Type::KING, Piece::Color::WHITE );
const Piece Piece::wq( Piece::Type::QUEEN, Piece::Color::WHITE );
const Piece Piece::wr( Piece::Type::ROOK, Piece::Color::WHITE );
const Piece Piece::wb( Piece::Type::BISHOP, Piece::Color::WHITE );
const Piece Piece::wn( Piece::Type::KNIGHT, Piece::Color::WHITE );
const Piece Piece::wp( Piece::Type::PAWN, Piece::Color::WHITE );
const Piece Piece::bk( Piece::Type::KING, Piece::Color::BLACK );
const Piece Piece::bq( Piece::Type::QUEEN, Piece::Color::BLACK );
const Piece Piece::br( Piece::Type::ROOK, Piece::Color::BLACK );
const Piece Piece::bb( Piece::Type::BISHOP, Piece::Color::BLACK );
const Piece Piece::bn( Piece::Type::KNIGHT, Piece::Color::BLACK );
const Piece Piece::bp( Piece::Type::PAWN, Piece::Color::BLACK );
const Piece Piece::nn( Piece::Type::NONE, Piece::Color::NONE );
*/