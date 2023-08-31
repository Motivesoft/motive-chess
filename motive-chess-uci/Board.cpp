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
    Log::Trace( [&] ( const Log::Logger& logger )
    {
        logger << "Apply move: " << move.toString() << std::endl;
    } );

    if ( move.isNullMove() )
    {
        // To be honest, not sure what to do here - return an unchanged board, or an updated one with no move made
        // but other attributes updated as though a move had been made and it was now the other side's go?
        Log::Trace << "Ignoring null move" << std::endl;
        return;
    }

    // Store this for later tests
    unsigned char movingPiece = pieceAt( move.getFrom() ); // Will not be Piece::NOTHING
    unsigned char capturedPiece = pieceAt( move.getTo() ); // May be Piece::NOTHING

    // Make the main part of the move and then check for other actions

    movePiece( move.getFrom(), move.getTo() );

    // Handle castling - check the piece that has just moved and work it out from there
    // TODO use Move's new knowledge about whether it is a castling move
    if ( movingPiece == Piece::WKING )
    {
        if ( move.getFrom() == Board::E1 )
        {
            if ( move.getTo() == Board::C1 )
            {
                movePiece( Board::A1, Board::D1 );

                Log::Trace << "White castling queen side" << std::endl;
            }
            else if ( move.getTo() == Board::G1 )
            {
                movePiece( Board::H1, Board::F1 );

                Log::Trace << "White castling king side" << std::endl;
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

                Log::Trace << "Black castling queen side" << std::endl;
            }
            else if ( move.getTo() == Board::G8 )
            {
                movePiece( Board::H8, Board::F8 );

                Log::Trace << "Black castling king side" << std::endl;
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

    // If a piece captures a rook, that prevents castling
    if ( move.getTo() == Board::H8 )
    {
        castlingRights.removeBlackKingsideCastlingRights();
    }
    else if ( move.getTo() == Board::A8 )
    {
        castlingRights.removeBlackQueensideCastlingRights();
    }
    else if ( move.getTo() == Board::H1 )
    {
        castlingRights.removeWhiteKingsideCastlingRights();
    }
    else if ( move.getTo() == Board::A1 )
    {
        castlingRights.removeWhiteQueensideCastlingRights();
    }

    Log::Trace( [&] ( const Log::Logger& logger )
    {
        logger << "Castling set to " << castlingRights.toString() << std::endl;
    } );

    // Promotions

    if ( move.isPromotion() )
    {
        setPiece( move.getTo(), move.getPromotionPiece() );

        Log::Trace( [&] ( const Log::Logger& logger )
        {
            logger << "Handling promotion to " << Piece::toFENString( move.getPromotionPiece() ) << std::endl;
        } );
    }

    // En-passant

    if ( move.getTo() == enPassantIndex )
    {
        if ( Piece::isPawn( movingPiece ) )
        {
            Log::Trace( [&] ( const Log::Logger& logger )
            {
                logger << "Handling en-passant capture at " << Utilities::indexToSquare( enPassantIndex ) << std::endl;
            } );

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

    Log::Trace( [&] ( const Log::Logger& logger )
    {
        logger << "Active color now " << Piece::toColorString( activeColor ) << std::endl;
    } );

    // Clear this but then determine whether this new move sets it again
    enPassantIndex = Utilities::getOffboardLocation();

    if ( Piece::isPawn( movingPiece ) )
    {
        unsigned short file = Utilities::indexToFile( move.getFrom() );

        if ( Utilities::indexToRank( move.getFrom() ) == RANK_2 && Utilities::indexToRank( move.getTo() ) == RANK_4 )
        {
            enPassantIndex = Utilities::squareToIndex( file, RANK_3 );

            Log::Trace( [&] ( const Log::Logger& logger )
            {
                logger << "En-passant square: " << Utilities::indexToSquare( enPassantIndex ) << std::endl;
            } );
        }
        else if ( Utilities::indexToRank( move.getFrom() ) == RANK_7 && Utilities::indexToRank( move.getTo() ) == RANK_5 )
        {
            enPassantIndex = Utilities::squareToIndex( file, RANK_6 );

            Log::Trace( [&] ( const Log::Logger& logger )
            {
                logger << "En-passant square: " << Utilities::indexToSquare( enPassantIndex ) << std::endl;
            } );
        }
    }

    // Halfmove increment? Only if not a capture or pawn move
    if ( Piece::isEmpty( capturedPiece ) && !Piece::isPawn( movingPiece ) )
    {
        halfmoveClock++;

        Log::Trace( [&] ( const Log::Logger& logger )
        {
            logger << "Adding one to halfmove clock. Now " << halfmoveClock << std::endl;
        } );
    }
    else
    {
        halfmoveClock = 0;

        Log::Trace << "Reset halfmove clock to zero" << std::endl;
    }

    // Increment move number
    if ( Piece::isWhite( activeColor ) )
    {
        fullmoveNumber++;

        Log::Trace( [&] ( const Log::Logger& logger )
        {
            logger << "Full move incrementing to " << fullmoveNumber << std::endl;
        } );
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

unsigned long long Board::movesInARay( unsigned long long possibleMoves,
                                       unsigned long long rayMask,
                                       unsigned long long ownPieces,
                                       unsigned long long enemyPieces,
                                       unsigned long long aboveMask,
                                       unsigned long long belowMask,
                                       bool supportsCaptures )
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
            // If no matches (blockers), set the mask extent to the upper limit
            lsb = 63;
        }

        unsigned long msb;
        if ( !_BitScanReverse64( &msb, botBlocks ) )
        {
            // If no matches (blockers), set the mask extent to the lower limit
            msb = 0;
        }

        // As we're looking at blocking pieces here, exclude the actual found squares
        // The effect of the '&' should be only to remove upper or lower bits 
        // of the mask if they contain blocking pieces, where if they contain capturable
        // enemies we should keep those bits set
        unsigned long long mask = Bitboard::makeMask( static_cast<unsigned short>( msb ), static_cast<unsigned short>( lsb ) ) & ~absoluteBlockers;

        // Limit this to moves possible when surrounded by friendly pieces
        rayMoves &= mask;
    }

    // Actual moves available including captures of the closest enemy piece
    if( supportsCaptures )
    {
        // NB at this point, rayMoves has already been constrained by friendly pieces
        unsigned long long topBlocks = aboveMask & rayMoves & enemyPieces;
        unsigned long long botBlocks = belowMask & rayMoves & enemyPieces;

        unsigned long lsb;
        if ( !_BitScanForward64( &lsb, topBlocks ) )
        {
            // If no matches (blockers), set the mask extent to the upper limit
            lsb = 63;
        }

        unsigned long msb;
        if ( !_BitScanReverse64( &msb, botBlocks ) )
        {
            // If no matches (blockers), set the mask extent to the lower limit
            msb = 0;
        }

        // As we're looking at enemy pieces here, the mask covers everything we need
        unsigned long long mask = Bitboard::makeMask( static_cast<unsigned short>( msb ), static_cast<unsigned short>( lsb ) );

        moves |= ( rayMoves & mask );
    }
    else
    {
        moves = rayMoves;
    }

    return moves;
}

std::vector<Move> Board::getMoves()
{ 
    bool isWhite = Piece::isWhite( activeColor );

    // Worker variables
    std::vector<Move> moves;

    unsigned long long mask = 1;
    unsigned short index;
    unsigned short destination;
    unsigned long long pieces;

    // Let's make some bitboards

    PieceBitboards own;
    PieceBitboards enemy;
    makePieceBitboards( isWhite, own, enemy );

    const unsigned short promotionRank = isWhite ? 7 : 0;

    const unsigned long long emptySquares = ~( own.allMask() | enemy.allMask() );
    const unsigned long long ownOrEmpty = own.allMask() | emptySquares;
    const unsigned long long enemyOrEmpty = enemy.allMask() | emptySquares;

    // Generate possible moves

    // Iterate through all pieces by popping them out of the mask
    pieces = own.pawnMask();
    while ( Bitboard::getEachIndexForward( &index, pieces ) )
    {
        // Determine piece moves
        unsigned long long setOfMoves = 0;

        unsigned long long possibleMoves = Bitboard::getPawnMoves( index, isWhite );
        unsigned long long possibleCaptures = Bitboard::getPawnCaptures( index, isWhite );

        unsigned long long aboveMask = Bitboard::makeUpperMask( index );
        unsigned long long belowMask = Bitboard::makeLowerMask( index );

        // Masks for specific directions of travel
        unsigned long long fileMask = Bitboard::getFileMask( index );

        setOfMoves |= movesInARay( possibleMoves, fileMask, own.allMask(), enemy.allMask(), aboveMask, belowMask, false);

        // Include captures, include en passant
        possibleCaptures &= ( Utilities::isOffboard( enPassantIndex ) ? enemy.allMask() : ( enemy.allMask() | 1ull << enPassantIndex ) );

        setOfMoves |= possibleCaptures;

        while ( Bitboard::getEachIndexForward( &destination, setOfMoves ) )
        {
            // Promotions lead to extra moves
            if ( Utilities::indexToRank( destination ) == promotionRank )
            {
                // Promote to...
                moves.push_back( Move::createPromotionMove( index, destination, isWhite ? Piece::WQUEEN : Piece::BQUEEN ) );
                moves.push_back( Move::createPromotionMove( index, destination, isWhite ? Piece::WROOK : Piece::BROOK ) );
                moves.push_back( Move::createPromotionMove( index, destination, isWhite ? Piece::WBISHOP : Piece::BBISHOP ) );
                moves.push_back( Move::createPromotionMove( index, destination, isWhite ? Piece::WKNIGHT : Piece::BKNIGHT ) );
            }
            else
            {
                moves.push_back( Move::createMove( index, destination ) );
            }
        }
    }

    pieces = own.knightMask();
    while ( Bitboard::getEachIndexForward( &index, pieces ) )
    {
        // Determine piece moves
        unsigned long long setOfMoves = Bitboard::getKnightMoves( index );
        setOfMoves &= enemyOrEmpty;

        while ( Bitboard::getEachIndexForward( &destination, setOfMoves ) )
        {
            moves.push_back( Move::createMove( index, destination ) );
        }
    }

    // Tackle bishops and queens together for the move similarities
    pieces = own.bishopMask() | own.queenMask();
    while ( Bitboard::getEachIndexForward( &index, pieces ) )
    {
        // Determine piece moves
        unsigned long long setOfMoves = 0;

        unsigned long long possibleMoves = Bitboard::getBishopMoves( index );

        unsigned long long aboveMask = Bitboard::makeUpperMask( index );
        unsigned long long belowMask = Bitboard::makeLowerMask( index );

        // Masks for specific directions of travel
        unsigned long long diagMask = Bitboard::getDiagonalMask( index );

        setOfMoves |= movesInARay( possibleMoves, diagMask, own.allMask(), enemy.allMask(), aboveMask, belowMask );

        unsigned long long antiMask = Bitboard::getAntiDiagonalMask( index );

        setOfMoves |= movesInARay( possibleMoves, antiMask, own.allMask(), enemy.allMask(), aboveMask, belowMask );

        while ( Bitboard::getEachIndexForward( &destination, setOfMoves ) )
        {
            moves.push_back( Move::createMove( index, destination ) );
        }
    }

    // Tackle rooks and queens together for the move similarities
    pieces = own.rookMask() | own.queenMask();
    while ( Bitboard::getEachIndexForward( &index, pieces ) )
    {
        // Determine piece moves
        unsigned long long setOfMoves = 0;

        unsigned long long possibleMoves = Bitboard::getRookMoves( index );

        unsigned long long aboveMask = Bitboard::makeUpperMask( index );
        unsigned long long belowMask = Bitboard::makeLowerMask( index );

        // Masks for specific directions of travel
        unsigned long long rankMask = Bitboard::getRankMask( index );

        setOfMoves |= movesInARay( possibleMoves, rankMask, own.allMask(), enemy.allMask(), aboveMask, belowMask );

        unsigned long long fileMask = Bitboard::getFileMask( index );
        
        setOfMoves |= movesInARay( possibleMoves, fileMask, own.allMask(), enemy.allMask(), aboveMask, belowMask );

        while ( Bitboard::getEachIndexForward( &destination, setOfMoves ) )
        {
            moves.push_back( Move::createMove( index, destination ) );
        }
    }

    pieces = own.kingMask();
    while ( Bitboard::getEachIndexForward( &index, pieces ) )
    {
        // Determine piece moves
        unsigned long long setOfMoves = Bitboard::getKingMoves( index );
        setOfMoves &= enemyOrEmpty;

        while ( Bitboard::getEachIndexForward( &destination, setOfMoves ) )
        {
            moves.push_back( Move::createMove( index, destination ) );
        }

        bool kingside;
        bool queenside;
        unsigned long long kingsideMask;
        unsigned long long queensideMask;

        if ( isWhite )
        {
            kingside = castlingRights.canWhiteCastleKingside();
            queenside = castlingRights.canWhiteCastleQueenside();
            kingsideMask = Bitboard::getWhiteKingsideCastlingMask();
            queensideMask = Bitboard::getWhiteQueensideCastlingMask();
        }
        else
        {
            kingside = castlingRights.canBlackCastleKingside();
            queenside = castlingRights.canBlackCastleQueenside();
            kingsideMask = Bitboard::getBlackKingsideCastlingMask();
            queensideMask = Bitboard::getBlackQueensideCastlingMask();
        }

        if ( kingside )
        {
            if ( ( kingsideMask & emptySquares ) == kingsideMask )
            {
                // Check for moving out of, or through check is done as a refutation check, later
                moves.push_back( Move::createKingsideCastlingMove( index, index + 2 ) );
            }
        }

        if ( queenside )
        {
            if ( ( queensideMask & emptySquares ) == queensideMask )
            {
                // Check for moving out of, or through check is done as a refutation check, later
                moves.push_back( Move::createQueensideCastlingMove( index, index - 2 ) );
            }
        }
    }

    // Make the move and test whether it is really legal, not just pseudo legal
    unsigned long long protectedSquares;
    for ( std::vector<Move>::iterator it = moves.begin(); it != moves.end(); )
    {
        Move& move = *it;
        Board testBoard = makeMove( move );

        // Which suqares are we testing? Just the king for check, or the squares it passes
        // through when castling
        if ( move.isCastle() )
        {
            if ( move.isKingsideCastle() )
            {
                protectedSquares = ( isWhite ? 0b01110000ull : 0b01110000ull << 56 );
            }
            else // if ( move.isQueensideCastle() )
            {
                protectedSquares = ( isWhite ? 0b00011100ull : 0b00011100ull << 56 );
            }
        }
        else
        {
            protectedSquares = testBoard.makePieceBitboard( isWhite ? Piece::WKING : Piece::BKING );
        }

        if ( testBoard.failsCheckTests( protectedSquares ) )
        {
            it = moves.erase( it );
        }
        else
        {
            // Move is fine, go on to the next one
            it++;
        }
    }

    Log::Trace( [&] ( const Log::Logger& logger )
    {
        logger << "Generated moves:" << std::endl;
        for ( std::vector<Move>::iterator it = moves.begin(); it != moves.end(); it++ )
        {
            logger << (*it).toString() << std::endl;
        }
    } );

    return moves;
}

/// <summary>
/// Check the provided squres and see if any are under attack.
/// This is used during move generation to making a candidate move and then calling
/// this to see whether any of the opponents pieces can 'catpure' any of the provided squares.
/// The 'opponent' at this point is the avtiveColor - but assigned to 'enemy' variables for clarity.
/// The squares to test are either just the king, or those the king passes through on the way to castling
/// </summary>
/// <param name="protectedSquares">bitmask of square or squares to test</param>
/// <returns>true if a square is under attack</returns>
bool Board::failsCheckTests( unsigned long long protectedSquares )
{
    // If any of the protected squares are attacked by this player, the test fails and should return true immediately
    // This will be called after making our move and so the state should be as though the opponent was about to play
    // This takes a mask as it might be up to three squares we need to check, during a castling operation

    bool isWhite = Piece::isWhite( activeColor );

    // Let's make some bitboards

    PieceBitboards own;
    PieceBitboards enemy;
    makePieceBitboards( !isWhite, own, enemy );

    // Worker variables
    unsigned short index;
    unsigned long long captureMask;

    while ( Bitboard::getEachIndexForward( &index, protectedSquares ) )
    {
        // Is 'square' reachable by this color's pieces

        // Pawn
        // Captures are reflections, so can index 'capture' potential pawn is a viable test
        captureMask = Bitboard::getPawnCaptures( index, !isWhite );

        if ( captureMask & enemy.pawnMask() )
        {
            return true;
        }

        // Knight
        captureMask = Bitboard::getKnightMoves( index );

        if ( captureMask & enemy.knightMask() )
        {
            return true;
        }

        // Bishop
        {
            unsigned long long setOfMoves = 0;

            unsigned long long possibleMoves = Bitboard::getBishopMoves( index );

            unsigned long long aboveMask = Bitboard::makeUpperMask( index );
            unsigned long long belowMask = Bitboard::makeLowerMask( index );

            // Masks for specific directions of travel
            unsigned long long diagMask = Bitboard::getDiagonalMask( index );

            setOfMoves |= movesInARay( possibleMoves, diagMask, own.allMask(), enemy.allMask(), aboveMask, belowMask );

            unsigned long long antiMask = Bitboard::getAntiDiagonalMask( index );

            setOfMoves |= movesInARay( possibleMoves, antiMask, own.allMask(), enemy.allMask(), aboveMask, belowMask );

            if ( setOfMoves & ( enemy.bishopMask() | enemy.queenMask() ) )
            {
                return true;
            }
        }

        // Rook
        {
            unsigned long long setOfMoves = 0;

            unsigned long long possibleMoves = Bitboard::getRookMoves( index );

            unsigned long long aboveMask = Bitboard::makeUpperMask( index );
            unsigned long long belowMask = Bitboard::makeLowerMask( index );

            // Masks for specific directions of travel
            unsigned long long rankMask = Bitboard::getRankMask( index );

            setOfMoves |= movesInARay( possibleMoves, rankMask, own.allMask(), enemy.allMask(), aboveMask, belowMask );

            unsigned long long fileMask = Bitboard::getFileMask( index );

            setOfMoves |= movesInARay( possibleMoves, fileMask, own.allMask(), enemy.allMask(), aboveMask, belowMask );

            if ( setOfMoves & ( enemy.rookMask() | enemy.queenMask() ) )
            {
                return true;
            }
        }

        // King
        captureMask = Bitboard::getKingMoves( index );

        if ( captureMask & enemy.kingMask() )
        {
            return true;
        }
    }

    return false;
}

void Board::validateCastlingRights()
{
    // Some FEN strings in the wild have wrong castling flags. Nip this in the board to
    // avoid having to consider it during thinking time
    if ( pieces[ Board::E1 ] != Piece::WKING || pieces[ Board::H1 ] != Piece::WROOK )
    {
        castlingRights.removeWhiteKingsideCastlingRights();
    }
    if ( pieces[ Board::E1 ] != Piece::WKING || pieces[ Board::A1 ] != Piece::WROOK )
    {
        castlingRights.removeWhiteQueensideCastlingRights();
    }
    if ( pieces[ Board::E8 ] != Piece::BKING || pieces[ Board::H8 ] != Piece::BROOK )
    {
        castlingRights.removeBlackKingsideCastlingRights();
    }
    if ( pieces[ Board::E8 ] != Piece::BKING || pieces[ Board::A8 ] != Piece::BROOK )
    {
        castlingRights.removeBlackQueensideCastlingRights();
    }
}

/// <summary>
/// Create a bitmask of locations of a specific piece type and color
/// </summary>
/// <param name="piece">the piece</param>
/// <returns>the bitboard for that piece type and color</returns>
unsigned long long Board::makePieceBitboard( unsigned char piece )
{
    unsigned long long bitboard = 0;

    for ( unsigned short index = 0; index < 64; index++ )
    {
        if ( pieceAt( index ) == piece )
        {
            bitboard |= Bitboard::indexToBit( index );
        }
    }

    return bitboard;
}

void Board::makePieceBitboards( bool isWhite,
                                unsigned long long& pawn,
                                unsigned long long& knight,
                                unsigned long long& bishop,
                                unsigned long long& rook,
                                unsigned long long& queen,
                                unsigned long long& king )
{
    pawn = knight = bishop = rook = queen = king = 0;

    unsigned char colorMask = isWhite ? 0b00001000 : 0b00010000;

    for ( unsigned short index = 0; index < 64; index++ )
    {
        unsigned char piece = pieceAt( index );
        if( piece & colorMask )
        {
            switch ( piece & 0b00000111 )
            {
                case 0b00000001:
                    pawn |= Bitboard::indexToBit( index );
                    break;

                case 0b00000010:
                    knight |= Bitboard::indexToBit( index );
                    break;

                case 0b00000011:
                    bishop |= Bitboard::indexToBit( index );
                    break;

                case 0b00000100:
                    rook |= Bitboard::indexToBit( index );
                    break;

                case 0b00000101:
                    queen |= Bitboard::indexToBit( index );
                    break;

                case 0b00000110:
                    king |= Bitboard::indexToBit( index );
                    break;

                default:
                    break;
            }
        }
    }
}

void Board::makePieceBitboards( bool isWhite,
                                PieceBitboards& ownBitboards,
                                PieceBitboards& enemyBitboards )
{
    unsigned char colorMask = isWhite ? 0b00001000 : 0b00010000;

    for ( unsigned short index = 0; index < 64; index++ )
    {
        unsigned char piece = pieceAt( index );

        PieceBitboards& bitboards = ( piece & colorMask ) ? ownBitboards : enemyBitboards;

        bitboards.pieceMask[ piece & 0b00000111 ] |= Bitboard::indexToBit( index );
    }

    ownBitboards.complete();
    enemyBitboards.complete();
}