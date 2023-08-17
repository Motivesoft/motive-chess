#include "Board.h"

#include <algorithm>

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
            else if ( rank == RANK_6 )
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

bool Board::isRefutation( const Move& move, const Move& response ) const
{
    // TODO consider whether any other checks need to go in here
    // TODO change this to a trace message
    LOG_DEBUG << "Looking at any refutation of " << move.toString() << " by " << response.toString();
    if ( Piece::isKing( pieceAt( response.getTo() ) ) )
    { 
        LOG_TRACE << "Move " << move.toString() << " is refuted by " << response.toString();
        return true;
    }

    if ( move.isKingsideCastle() )
    {
        unsigned long long refutationSquares = Bitboards->getKingsideCastlingJourney();
        Utilities::dumpBitmask( response.getTo() );
        Utilities::dumpBitmask( refutationSquares );
        Utilities::dumpBitmask( refutationSquares & response.getTo() );
        if ( response.getTo() & refutationSquares )
        {
            return true;
        }
    }
    else if ( move.isKingsideCastle() )
    {
        unsigned long long refutationSquares = Bitboards->getQueensideCastlingJourney();
        if ( response.getTo() & refutationSquares )
        {
            return true;
        }
    }

    return false;
}

unsigned long long makeMask1( unsigned long from, unsigned long to )
{
    unsigned long long result = 0;
    unsigned long long mask = 1ull << from;

    if ( to < from )
    {
        return makeMask1( to, from );
    }

    for ( unsigned long loop = from; loop <= to; loop++, mask <<= 1 )
    {
        result |= mask;
    }

    return result;
}


unsigned long long movesInARay( unsigned long long possibleMoves,
                                unsigned long long rayMask,
                                unsigned long long ownPieces,
                                unsigned long long enemyPieces,
                                unsigned long long aboveMask,
                                unsigned long long belowMask,
                                bool supportsCaptures = true )
{
    unsigned long long moves = 0;
    unsigned long long rayMoves = possibleMoves & rayMask;

    // If a sliding move can't be a capture (pawns), then both own and enemy pieces are blocking
    unsigned long long absoluteBlockers = supportsCaptures ? ownPieces : ownPieces | enemyPieces;

    // Possible moves available despite friendly pieces getting in the way
    {
        unsigned long long topBlocks = aboveMask & rayMoves & absoluteBlockers;
        unsigned long long botBlocks = belowMask & rayMoves & absoluteBlockers;

        unsigned long lsb;
        if ( !_BitScanForward64( &lsb, topBlocks ) )
        {
            _BitScanReverse64( &lsb, aboveMask & rayMoves );
        }

        unsigned long msb;
        if ( !_BitScanReverse64( &msb, botBlocks ) )
        {
            _BitScanForward64( &msb, belowMask & rayMoves );
        }

        // As we're looking at blocking pieces here, exclude the actual found squares
        // The effect of the '&' should be only to remove upper or lower bits 
        // of the mask if they contain blocking pieces, where if they contain capturable
        // enemies we should keep those bits set
        unsigned long long mask = makeMask1( msb, lsb ) & ~absoluteBlockers;

        // Limit this to moves possible when surrounded by friendly pieces
        rayMoves &= mask;
    }

    // Actual moves available including captures of the closest enemy piece
    if( supportsCaptures )
    {
        // NB at this point, rayMoves has already been constrained by friendly pieces
        unsigned long long topBlocks = aboveMask & rayMoves & enemyPieces;
        unsigned long long botBlocks = belowMask & rayMoves & enemyPieces;

        // TODO check that we couldn't/shouldn't just set the value inside the if to a fixed/known value
        unsigned long lsb;
        if ( !_BitScanForward64( &lsb, topBlocks ) )
        {
            _BitScanReverse64( &lsb, aboveMask & rayMoves );
        }

        // TODO check that we couldn't/shouldn't just set the value inside the if to a fixed/known value
        unsigned long msb;
        if ( !_BitScanReverse64( &msb, botBlocks ) )
        {
            _BitScanForward64( &msb, belowMask & rayMoves );
        }

        // As we're looking at enemy pieces here, the mask covers everything we need
        unsigned long long mask = makeMask1( msb, lsb );

        moves |= ( rayMoves & mask );
    }
    else
    {
        moves = rayMoves;
    }

    return moves;
}

std::vector<Move> Board::getPseudoLegalMoves( bool isWhite )
{
    // Worker variables
    std::vector<Move> moves;

    unsigned long long mask = 1;
    unsigned long piece;
    unsigned short index;
    unsigned long long pieces;

    // Let's make some bitboards
    // TODO see if we can make these const
    // TODO see if we can populate them better
    unsigned long long ownPawns = makePieceBitboard( isWhite ? Piece::WPAWN : Piece::BPAWN );
    unsigned long long ownKnights = makePieceBitboard( isWhite ? Piece::WKNIGHT : Piece::BKNIGHT );
    unsigned long long ownBishops = makePieceBitboard( isWhite ? Piece::WBISHOP : Piece::BBISHOP );
    unsigned long long ownRooks = makePieceBitboard( isWhite ? Piece::WROOK : Piece::BROOK );
    unsigned long long ownQueens = makePieceBitboard( isWhite ? Piece::WQUEEN : Piece::BQUEEN );
    unsigned long long ownKing = makePieceBitboard( isWhite ? Piece::WKING : Piece::BKING );
    unsigned long long enemyPawns = makePieceBitboard( isWhite ? Piece::BPAWN : Piece::WPAWN );
    unsigned long long enemyKnights = makePieceBitboard( isWhite ? Piece::BKNIGHT : Piece::WKNIGHT );
    unsigned long long enemyBishops = makePieceBitboard( isWhite ? Piece::BBISHOP : Piece::WBISHOP );
    unsigned long long enemyRooks = makePieceBitboard( isWhite ? Piece::BROOK : Piece::WROOK );
    unsigned long long enemyQueens = makePieceBitboard( isWhite ? Piece::BQUEEN : Piece::WQUEEN );
    unsigned long long enemyKing = makePieceBitboard( isWhite ? Piece::BKING : Piece::WKING );

    unsigned short promotionRank = isWhite ? 7 : 0;

    unsigned long long ownPieces = ownPawns | ownKnights | ownBishops | ownRooks | ownQueens | ownKing;
    unsigned long long enemyPieces = enemyPawns | enemyKnights | enemyBishops | enemyRooks | enemyQueens | enemyKing;
    unsigned long long emptySquares = ~( ownPieces | enemyPieces );
    unsigned long long ownOrEmpty = ownPieces | emptySquares;
    unsigned long long enemyOrEmpty = enemyPieces | emptySquares;

    // Generate possible moves

    // Iterate through all pieces
    pieces = ownPawns;
    while ( _BitScanForward64( &piece, pieces ) )
    {
        // TODO check this works to mask the piece we've just extracted
        Utilities::dumpBitmask( pieces );
        pieces ^= (1ull << piece);
        Utilities::dumpBitmask( pieces );

        index = static_cast<unsigned short>( piece );

        // Determine piece moves
        unsigned long long setOfMoves = 0;

        unsigned long long possibleMoves = Bitboards->getPawnMoves( index );

        // TODO actually have premade bitboards for each color
        if ( !isWhite )
        {
            possibleMoves = _byteswap_uint64( possibleMoves );
        }

        unsigned long long aboveMask = index == 63 ? 0 : makeMask1( index + 1, 63 );
        unsigned long long belowMask = index == 0 ? 0 : makeMask1( 0, index - 1 );

        // Masks for specific directions of travel
        unsigned long long fileMask = Bitboards->getFileMask( Utilities::indexToFile( index ) );

        setOfMoves |= movesInARay( possibleMoves, fileMask, ownPieces, enemyPieces, aboveMask, belowMask, false );

        // Include captures, include en passant
        unsigned long long possibleCaptures = Bitboards->getPawnCaptures( index );
        possibleCaptures &= ( Utilities::isOffboard( enPassantIndex ) ? enemyPieces : ( enemyPieces | 1ull << enPassantIndex ) );

        setOfMoves |= possibleCaptures;

        while ( setOfMoves != 0 )
        {
            unsigned long destination;

            if ( _BitScanForward64( &destination, setOfMoves ) )
            {
                setOfMoves &= ~( 1ull << destination );

                unsigned short destinationShort = static_cast<unsigned short>( destination );

                // Promotions lead to extra moves
                if ( Utilities::indexToRank( destinationShort ) == promotionRank )
                {
                    // Promote to...
                    moves.push_back( Move::createPromotionMove( index, destinationShort, Piece::WQUEEN ) );
                    moves.push_back( Move::createPromotionMove( index, destinationShort, Piece::WROOK ) );
                    moves.push_back( Move::createPromotionMove( index, destinationShort, Piece::WBISHOP ) );
                    moves.push_back( Move::createPromotionMove( index, destinationShort, Piece::WKNIGHT ) );
                }
                else
                {
                    // Destination will not be damaged by cast to short
                    moves.push_back( Move::createMove( index, destinationShort ) );
                }
            }
            else
            {
                break;
            }
        }
    }

    pieces = ownKnights;
    while ( _BitScanForward64( &piece, pieces ) )
    {
        // TODO check this works to mask the piece we've just extracted
        pieces ^= ( 1ull << piece );

        // Determine knight moves
    }

    pieces = ownBishops;
    while ( _BitScanForward64( &piece, pieces ) )
    {
        // TODO check this works to mask the piece we've just extracted
        pieces ^= ( 1ull << piece );

        // Determine piece moves
    }

    pieces = ownRooks;
    while ( _BitScanForward64( &piece, pieces ) )
    {
        // TODO check this works to mask the piece we've just extracted
        pieces ^= ( 1ull << piece );

        // Determine knight moves
    }

    pieces = ownQueens;
    while ( _BitScanForward64( &piece, pieces ) )
    {
        // TODO check this works to mask the piece we've just extracted
        pieces ^= ( 1ull << piece );

        // Determine piece moves
    }

    pieces = ownKing;
    while ( _BitScanForward64( &piece, pieces ) )
    {
        // TODO check this works to mask the piece we've just extracted
        pieces ^= ( 1ull << piece );

        // Determine knight moves
    }

    return moves;
}

std::vector<Move> Board::getPseudoLegalMoves()
{
    std::vector<Move> moves;

    // TODO this is only a testing step - remove when ready
    if ( Piece::isWhite( activeColor ) )
    {
        return getPseudoLegalMoves( true );
    }
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

    unsigned long long mask = 1;

    // There must be a better way to do this - rotation maybe?
    if ( Piece::isWhite( activeColor ) )
    {
        // Generate possible white moves

        for ( int loop = 0; loop < 64; loop++, mask <<= 1 )
        {
            // Find a pawn
            if ( whitePawns & mask )
            {
                unsigned long long setOfMoves = 0;

                unsigned long long possibleMoves = Bitboards->getPawnMoves( loop );

                unsigned long long aboveMask = loop == 63 ? 0 : makeMask1( loop + 1, 63 );
                unsigned long long belowMask = loop == 0 ? 0 : makeMask1( 0, loop - 1 );

                // Masks for specific directions of travel
                unsigned long long fileMask = Bitboards->getFileMask( Utilities::indexToFile( loop ) );

                setOfMoves |= movesInARay( possibleMoves, fileMask, whitePieces, blackPieces, aboveMask, belowMask, false );

                // Include captures, include en passant
                unsigned long long possibleCaptures = Bitboards->getPawnCaptures( loop );
                possibleCaptures &= (Utilities::isOffboard( enPassantIndex ) ? blackPieces : ( blackPieces | 1ull << enPassantIndex ) );

                setOfMoves |= possibleCaptures;

                while ( setOfMoves != 0 )
                {
                    unsigned long destination;

                    if ( _BitScanForward64( &destination, setOfMoves ) )
                    {
                        setOfMoves &= ~( 1ull << destination );

                        unsigned short destinationShort = static_cast<unsigned short>( destination );

                        // Promotions lead to extra moves
                        if ( Utilities::indexToRank( destinationShort ) == 7 )
                        {
                            // Promote to...
                            moves.push_back( Move::createPromotionMove( loop, destinationShort, Piece::WQUEEN ) );
                            moves.push_back( Move::createPromotionMove( loop, destinationShort, Piece::WROOK ) );
                            moves.push_back( Move::createPromotionMove( loop, destinationShort, Piece::WBISHOP ) );
                            moves.push_back( Move::createPromotionMove( loop, destinationShort, Piece::WKNIGHT ) );
                        }
                        else
                        {
                            // Destination will not be damaged by cast to short
                            moves.push_back( Move::createMove( loop, destinationShort ) );
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }
            else if ( whiteKnights & mask )
            {
                unsigned long long possibleMoves = Bitboards->getKnightMoves( loop );
                possibleMoves &= blackOrEmpty;

                while ( possibleMoves > 0 )
                {
                    unsigned long long msb = possibleMoves & ~( possibleMoves - 1 );

                    // Fortunately, msb will not be zero here, so 'std::bit_width( msb ) - 1' should fine
                    moves.push_back( Move::createMove( loop, (unsigned short) std::bit_width( msb ) - 1 ) );
                    possibleMoves &= ~msb;
                }
            }
            else if ( whiteBishops & mask )
            {
                unsigned long long setOfMoves = 0;

                unsigned long long possibleMoves = Bitboards->getBishopMoves( loop );

                unsigned long long aboveMask = loop == 63 ? 0 : makeMask1( loop + 1, 63 );
                unsigned long long belowMask = loop == 0 ? 0 : makeMask1( 0, loop - 1 );

                // Masks for specific directions of travel
                unsigned long long diagMask = Bitboards->getDiagonalMask( Utilities::indexToFile( loop ),
                                                                          Utilities::indexToRank( loop ) );

                setOfMoves |= movesInARay( possibleMoves, diagMask, whitePieces, blackPieces, aboveMask, belowMask );

                unsigned long long antiMask = Bitboards->getAntiDiagonalMask( Utilities::indexToFile( loop ),
                                                                              Utilities::indexToRank( loop ) );

                setOfMoves |= movesInARay( possibleMoves, antiMask, whitePieces, blackPieces, aboveMask, belowMask );

                while ( setOfMoves != 0 )
                {
                    unsigned long destination;

                    if ( _BitScanForward64( &destination, setOfMoves ) )
                    {
                        setOfMoves &= ~( 1ull << destination );

                        // Destination will not be damaged by cast to short
                        moves.push_back( Move::createMove( loop, (unsigned short) destination ) );
                    }
                    else
                    {
                        break;
                    }
                }
            }
            else if ( whiteRooks & mask )
            {
            unsigned long long setOfMoves = 0;

            unsigned long long possibleMoves = Bitboards->getRookMoves( loop );

            unsigned long long aboveMask = loop == 63 ? 0 : makeMask1( loop + 1, 63 );
            unsigned long long belowMask = loop == 0 ? 0 : makeMask1( 0, loop - 1 );

            // Masks for specific directions of travel
            unsigned long long rankMask = Bitboards->getRankMask( Utilities::indexToRank( loop ) );

            setOfMoves |= movesInARay( possibleMoves, rankMask, whitePieces, blackPieces, aboveMask, belowMask );

            unsigned long long fileMask = Bitboards->getFileMask( Utilities::indexToFile( loop ) );

            setOfMoves |= movesInARay( possibleMoves, fileMask, whitePieces, blackPieces, aboveMask, belowMask );

            while ( setOfMoves != 0 )
            {
                unsigned long destination;

                if ( _BitScanForward64( &destination, setOfMoves ) )
                {
                    setOfMoves &= ~( 1ull << destination );

                    // Destination will not be damaged by cast to short
                    moves.push_back( Move::createMove( loop, (unsigned short) destination ) );
                }
                else
                {
                    break;
                }
            }
            }
            else if ( whiteQueens & mask )
            {
            unsigned long long setOfMoves = 0;

            unsigned long long possibleMoves = Bitboards->getQueenMoves( loop );

            unsigned long long aboveMask = loop == 63 ? 0 : makeMask1( loop + 1, 63 );
            unsigned long long belowMask = loop == 0 ? 0 : makeMask1( 0, loop - 1 );

            // Masks for specific directions of travel
            unsigned long long rankMask = Bitboards->getRankMask( Utilities::indexToRank( loop ) );

            setOfMoves |= movesInARay( possibleMoves, rankMask, whitePieces, blackPieces, aboveMask, belowMask );

            unsigned long long fileMask = Bitboards->getFileMask( Utilities::indexToFile( loop ) );

            setOfMoves |= movesInARay( possibleMoves, fileMask, whitePieces, blackPieces, aboveMask, belowMask );

            unsigned long long diagMask = Bitboards->getDiagonalMask( Utilities::indexToFile( loop ),
                                                                      Utilities::indexToRank( loop ) );

            setOfMoves |= movesInARay( possibleMoves, diagMask, whitePieces, blackPieces, aboveMask, belowMask );

            unsigned long long antiMask = Bitboards->getAntiDiagonalMask( Utilities::indexToFile( loop ),
                                                                          Utilities::indexToRank( loop ) );

            setOfMoves |= movesInARay( possibleMoves, antiMask, whitePieces, blackPieces, aboveMask, belowMask );

            while ( setOfMoves != 0 )
            {
                unsigned long destination;

                if ( _BitScanForward64( &destination, setOfMoves ) )
                {
                    setOfMoves &= ~( 1ull << destination );

                    // Destination will not be damaged by cast to short
                    moves.push_back( Move::createMove( loop, (unsigned short) destination ) );
                }
                else
                {
                    break;
                }
            }
            }
            else if ( whiteKing & mask )
            {
                unsigned long long possibleMoves = Bitboards->getKingMoves( loop );
                possibleMoves &= blackOrEmpty;

                while ( possibleMoves > 0 )
                {
                    unsigned long long msb = possibleMoves & ~( possibleMoves - 1 );

                    // Fortunately, msb will not be zero here, so 'std::bit_width( msb ) - 1' should fine
                    moves.push_back( Move::createMove( loop, (unsigned short) std::bit_width( msb ) - 1 ) );
                    possibleMoves &= ~msb;
                }

                if ( castlingRights.canWhiteCastleKingside() )
                {
                    if ( ( Bitboards->getKingsideCastlingMask() & emptySquares ) == Bitboards->getKingsideCastlingMask() )
                    {
                        // Check for moving out of, or through check is done as a refutation check, later
                        // TODO See if we can improve this and make it more color-neutral
                        moves.push_back( Move::createKingsideCastlingMove( loop, loop + 2 ) );
                    }
                }

                if ( castlingRights.canWhiteCastleQueenside() )
                {
                    if ( ( Bitboards->getQueensideCastlingMask() & emptySquares ) == Bitboards->getQueensideCastlingMask() )
                    {
                        // Check for moving out of, or through check is done as a refutation check, later
                        // TODO See if we can improve this and make it more color-neutral
                        moves.push_back( Move::createQueensideCastlingMove( loop, loop - 2 ) );
                    }
                }
            }
        }
    }

    LOG_TRACE << "Moves:";
    for ( std::vector<Move>::iterator it = moves.begin(); it != moves.end(); it++ )
    {
        LOG_TRACE << ( *it ).toString();
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
