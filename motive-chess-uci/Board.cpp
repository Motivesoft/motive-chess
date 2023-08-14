#include "Board.h"

#include "Bitboard.h"

Board Board::makeMove( const Move& move )
{
    Board board( *this );

    board.applyMove( move );

    return board;
}

void Board::applyMove( const Move& move )
{
    LOG_TRACE << "Apply move: " << move.toString();

    if ( move.isNullMove() )
    {
        // To be honest, not sure what to do here - return an unchanged board, or an updated one with no move made
        // but other attributes updated as though a move had been made and it was now the other side's go
        LOG_TRACE << "Ignoring null move";
        return;
    }

    // Store this for later tests
    unsigned char movingPiece = pieceAt( move.getFrom() ); // Will not be Piece::NOTHING
    unsigned char capturedPiece = pieceAt( move.getTo() ); // May be Piece::NOTHING

    // Make the main part of the move and then check for other actions

    movePiece( move.getFrom(), move.getTo() );

    // Handle castling - check the piece that has just moved and work it out from there
    if ( movingPiece == Piece::WKING )
    {
        if ( move.getFrom() == Board::E1 )
        {
            if ( move.getTo() == Board::C1 )
            {
                movePiece( Board::A1, Board::D1 );

                LOG_TRACE << "White castling queen side";
            }
            else if ( move.getTo() == Board::G1 )
            {
                movePiece( Board::H1, Board::F1 );

                LOG_TRACE << "White castling king side";
            }
        }

        // White king has moved - no more castling
        castlingRights.removeWhiteCastlingRights();
    }
    else if ( movingPiece == Piece::BKING )
    {
        if ( move.getFrom() == Board::E8 )
        {
            if ( move.getTo() == Board::C8 )
            {
                movePiece( Board::A8, Board::D8 );

                LOG_TRACE << "Black castling queen side";
            }
            else if ( move.getTo() == Board::G8 )
            {
                movePiece( Board::H8, Board::F8 );

                LOG_TRACE << "Black castling king side";
            }
        }

        // Black king has moved - no more castling
        castlingRights.removeBlackCastlingRights();
    }
    else if ( movingPiece == Piece::WROOK )
    {
        // A white rook has moved - work out which and disable its ability to castle
        if ( move.getFrom() == Board::H1 )
        {
            castlingRights.removeWhiteKingsideCastlingRights();
        }
        else if ( move.getFrom() == Board::A1 )
        {
            castlingRights.removeWhiteQueensideCastlingRights();
        }
    }
    else if ( movingPiece == Piece::BROOK )
    {
        // A black rook has moved - work out which and disable its ability to castle
        if ( move.getFrom() == Board::H8 )
        {
            castlingRights.removeBlackKingsideCastlingRights();
        }
        else if ( move.getFrom() == Board::A8 )
        {
            castlingRights.removeBlackQueensideCastlingRights();
        }
    }

    LOG_TRACE << "Castling set to " << castlingRights.toString();

    // Promotions

    if ( move.isPromotion() )
    {
        setPiece( move.getTo(), move.getPromotionPiece() );

        LOG_TRACE << "Handling promotion to " << Piece::toFENString( move.getPromotionPiece() );
    }

    // En-passant

    if ( move.getTo() == enPassantIndex )
    {
        if ( Piece::isPawn( movingPiece ) )
        {
            LOG_TRACE << "Handling en-passant capture at " << Utilities::indexToSquare( enPassantIndex );

            // An en-passant capture is happening. Remove the enemy pawn

            unsigned short file = Utilities::indexToFile( enPassantIndex );
            unsigned short rank = Utilities::indexToRank( enPassantIndex );

            if ( rank == RANK_3 )
            {
                removePiece( file, RANK_4 );
            }
            else if( rank == RANK_6 )
            {
                removePiece( file, RANK_5 );
            }
        }
    }

    // Swap whose move it is
    activeColor = Piece::oppositeColor( activeColor );
    LOG_TRACE << "Active color now " << Piece::toColorString( activeColor );

    // Clear this but then determine whether this new move sets it again
    enPassantIndex = Utilities::getOffboardLocation();

    if ( Piece::isPawn( movingPiece ) )
    {
        unsigned short file = Utilities::indexToFile( move.getFrom() );

        if ( Utilities::indexToRank( move.getFrom() ) == RANK_2 && Utilities::indexToRank( move.getTo() ) == RANK_4 )
        {
            enPassantIndex = Utilities::squareToIndex( file, RANK_3 );
        }
        else if ( Utilities::indexToRank( move.getFrom() ) == RANK_7 && Utilities::indexToRank( move.getTo() ) == RANK_5 )
        {
            enPassantIndex = Utilities::squareToIndex( file, RANK_6 );
        }

        LOG_TRACE << "En-passant square: " << ( Utilities::isOffboard( enPassantIndex ) ? "none" : Utilities::indexToSquare( enPassantIndex ) );
    }
    else
    {
        LOG_TRACE << "En-passant square cleared";
    }

    // Halfmove increment? Only if not a capture or pawn move
    if ( Piece::isEmpty( capturedPiece ) && !Piece::isPawn( movingPiece ) )
    {
        halfmoveClock++;
        
        LOG_TRACE << "Adding one to halfmove clock. Now " << halfmoveClock;
    }
    else
    {
        halfmoveClock = 0;

        LOG_TRACE << "Reset halfmove clock to zero";
    }

    // Increment move number
    if ( Piece::isWhite( activeColor ) )
    {
        fullmoveNumber++;

        LOG_TRACE << "Full move incrementing to " << fullmoveNumber;
    }

    // TODO Tuning - does this call the copy constructor too often and should we move to pointers?

    Utilities::dumpBoard( pieces );
}

bool Board::positionMatch( const Board& board ) const
{
    for ( int loop = 0; loop < 64; loop++ )
    {
        if ( pieceAt( loop ) != board.pieceAt( loop ) )
        {
            return false;
        }
    }

    if ( activeColor != board.activeColor )
    {
        return false;
    }

    if ( castlingRights != board.castlingRights )
    {
        return false;
    }

    if ( enPassantIndex != board.enPassantIndex )
    {
        return false;
    }

    if ( halfmoveClock != board.halfmoveClock )
    {
        return false;
    }

    if ( fullmoveNumber != board.fullmoveNumber )
    {
        return false;
    }

    return true;
}

bool Board::isSamePosition( const Board& board ) const
{
    for ( int loop = 0; loop < 64; loop++ )
    {
        if ( pieceAt( loop ) != board.pieceAt( loop ) )
        {
            return false;
        }
    }

    if ( activeColor != board.activeColor )
    {
        return false;
    }

    if ( castlingRights != board.castlingRights )
    {
        return false;
    }

    return true;
}

bool Board::isRefutation( const Move& move ) const
{
    // TODO consider whether any other checks need to go in here
    return Piece::isKing( pieceAt( move.getTo() ) );
}

std::vector<Move> Board::getPseudoLegalMoves()
{
    std::vector<Move> moves;

    // Right then. Now how do we do this...

    // Let's make some bitboards
    unsigned long long whitePawns = makePieceBitboard( Piece::WPAWN );
    unsigned long long whiteKnights = makePieceBitboard( Piece::WKNIGHT );
    unsigned long long whiteBishops = makePieceBitboard( Piece::WBISHOP );
    unsigned long long whiteRooks = makePieceBitboard( Piece::WROOK );
    unsigned long long whiteQueens = makePieceBitboard( Piece::WQUEEN );
    unsigned long long whiteKing = makePieceBitboard( Piece::WKING );
    unsigned long long blackPawns = makePieceBitboard( Piece::BPAWN );
    unsigned long long blackKnights = makePieceBitboard( Piece::BKNIGHT );
    unsigned long long blackBishops = makePieceBitboard( Piece::BBISHOP );
    unsigned long long blackRooks = makePieceBitboard( Piece::BROOK );
    unsigned long long blackQueens = makePieceBitboard( Piece::BQUEEN );
    unsigned long long blackKing = makePieceBitboard( Piece::BKING );

    unsigned long long whitePieces = whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKing;
    unsigned long long blackPieces = blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKing;
    unsigned long long emptySquares = ~( whitePieces | blackPieces );
    unsigned long long whiteOrEmpty = whitePieces | emptySquares;
    unsigned long long blackOrEmpty = blackPieces | emptySquares;

    Utilities::dumpBitboard( whitePieces | blackPieces );

    unsigned long long mask = 1;

    // There must be a better way to do this - rotation maybe?
    if ( Piece::isWhite( activeColor ) )
    {
        // Generate possible white moves
        // Do it in a hamfisted way first and then optimize
        unsigned long long normalPawnMove = 1 << 8;
        unsigned long long capturePawnMove = 1 << 7 | 1 << 9;
        unsigned long long startingPawnMove = 1 << 16;

        for ( int loop = 0; loop < 64; loop++, mask <<= 1 )
        {
            // Find a pawn
            if ( whitePawns & mask )
            {
                unsigned long long possibleMoves = Bitboards->getPawnMoves( loop );
                possibleMoves &= emptySquares;

                while ( possibleMoves > 0 )
                {
                    unsigned long long msb = possibleMoves & ~( possibleMoves - 1 );
                    
                    // Fortunately, msb will not be zero here, so 'std::bit_width( msb ) - 1' should fine
                    moves.push_back( Move( loop, (unsigned short) std::bit_width( msb ) - 1 ) );
                    possibleMoves &= ~msb;
                }

                // TODO also captures and double moves
            }
            else if ( whiteKnights & mask)
            {
                unsigned long long possibleMoves = Bitboards->getKnightMoves( loop );
                possibleMoves &= blackOrEmpty;

                while ( possibleMoves > 0 )
                {
                    unsigned long long msb = possibleMoves & ~( possibleMoves - 1 );

                    // Fortunately, msb will not be zero here, so 'std::bit_width( msb ) - 1' should fine
                    moves.push_back( Move( loop, (unsigned short) std::bit_width( msb ) - 1 ) );
                    possibleMoves &= ~msb;
                }
            }
        }
    }
    else
    {

    }

    LOG_DEBUG << "Moves:";
    for ( std::vector<Move>::iterator it = moves.begin(); it != moves.end(); it++ )
    {
        LOG_DEBUG << (*it).toString();
    }

    return moves;
}

unsigned long long Board::makePieceBitboard( unsigned char piece )
{
    unsigned long long bitboard = 0;

    unsigned long long bit = 0b0000000000000000000000000000000000000000000000000000000000000001;
    for ( int loop = 0; loop < 64; loop++, bit <<= 1 )
    {
        if ( pieceAt( loop ) == piece )
        {
            bitboard |= bit;
        }
    }

    return bitboard;
}
