#pragma once

#include <array>
#include <memory>

#include "CastlingRights.h"
#include "Fen.h"
#include "Move.h"
#include "Piece.h"
#include "Utilities.h"

class Board
{
private:
    // Square indices for the first and last ranks
    inline static const unsigned short A1 = 0;
    inline static const unsigned short B1 = 1;
    inline static const unsigned short C1 = 2;
    inline static const unsigned short D1 = 3;
    inline static const unsigned short E1 = 4;
    inline static const unsigned short F1 = 5;
    inline static const unsigned short G1 = 6;
    inline static const unsigned short H1 = 7;

    inline static const unsigned short A8 = 56;
    inline static const unsigned short B8 = 57;
    inline static const unsigned short C8 = 58;
    inline static const unsigned short D8 = 59;
    inline static const unsigned short E8 = 60;
    inline static const unsigned short F8 = 61;
    inline static const unsigned short G8 = 62;
    inline static const unsigned short H8 = 63;

    inline static const unsigned short FILE_A = 0;
    inline static const unsigned short FILE_B = 1;
    inline static const unsigned short FILE_C = 2;
    inline static const unsigned short FILE_D = 3;
    inline static const unsigned short FILE_E = 4;
    inline static const unsigned short FILE_F = 5;
    inline static const unsigned short FILE_G = 6;
    inline static const unsigned short FILE_H = 7;

    inline static const unsigned short RANK_1 = 0;
    inline static const unsigned short RANK_2 = 1;
    inline static const unsigned short RANK_3 = 2;
    inline static const unsigned short RANK_4 = 3;
    inline static const unsigned short RANK_5 = 4;
    inline static const unsigned short RANK_6 = 5;
    inline static const unsigned short RANK_7 = 6;
    inline static const unsigned short RANK_8 = 7;

    std::array<unsigned char, 64> pieces;
    unsigned char activeColor;
    CastlingRights castlingRights;
    unsigned short enPassantIndex;
    unsigned short halfmoveClock;
    unsigned short fullmoveNumber;

    inline void setPiece( const unsigned short index, const unsigned char piece )
    {
        pieces[ index ] = piece;
    }

    inline void setPiece( const unsigned short file, const unsigned short rank, const unsigned char piece )
    {
        setPiece( Utilities::squareToIndex( file, rank ), piece );
    }

    inline void removePiece( const unsigned short index )
    {
        setPiece( index, Piece::emptyPiece() );
    }

    inline void removePiece( const unsigned short file, const unsigned short rank )
    {
        removePiece( Utilities::squareToIndex( file, rank ) );
    }

    inline void movePiece( const unsigned short indexFrom, const unsigned short indexTo )
    {
        setPiece( indexTo, pieceAt( indexFrom ) );
        removePiece( indexFrom );
    }

    inline bool isEmpty( const unsigned short index ) const
    {
        return Piece::isEmpty( pieceAt( index ) );
    }

    inline bool isEmpty( const unsigned short file, const unsigned short rank ) const
    {
        return isEmpty( Utilities::squareToIndex( file, rank ) );
    }

    inline unsigned char pieceAt( const unsigned short index ) const
    {
        return pieces[ index ];
    }

    inline unsigned char pieceAt( const unsigned short file, const unsigned short rank ) const
    {
        return pieceAt( Utilities::squareToIndex( file, rank ) );
    }

    // This is a precise equality check, not a "is this the same position" check
    bool positionMatch( const Board& board ) const;

    /// <summary>
    /// Applies move to this board
    /// </summary>
    /// <param name="move">the move</param>
    void applyMove( Move* move );

    void validateCastlingRights();

    bool failsCheckTests( unsigned long long protectedSquares, bool asWhite ) const;

    unsigned long long movesInARay( unsigned long long possibleMoves,
                                    unsigned long long rayMask,
                                    unsigned long long ownPieces,
                                    unsigned long long enemyPieces,
                                    unsigned long long aboveMask,
                                    unsigned long long belowMask,
                                    bool supportsCaptures = true ) const;

    unsigned long long makePieceBitboard( unsigned char piece ) const;

    /// <summary>
    /// A collection class for all bitmasks for one particular color.
    /// Arrays and references here assume 8 piece (rather than 6) where 0 and 7 are unused
    /// The use of indexes 1-6 is intended to tally with the Piece constants for Pawn, Knight, ...
    /// </summary>
    class PieceBitboards
    {
    public:
        unsigned long long pieceMask[ 8 ];
        unsigned long long allPiecesMask;

        PieceBitboards() :
            allPiecesMask( 0 )
        {
            pieceMask[ 1 ] = pieceMask[ 2 ] = pieceMask[ 3 ] = pieceMask[ 4 ] = pieceMask[ 5 ] = pieceMask[ 6 ] = 0;
        }

        void complete()
        {
            allPiecesMask = pieceMask[ 1 ] | pieceMask[ 2 ] | pieceMask[ 3 ] | pieceMask[ 4 ] | pieceMask[ 5 ] | pieceMask[ 6 ];
        }

        // Helper methods - magic numbers below (and above) match Piece representations

        inline unsigned long long pawnMask() const
        {
            return pieceMask[ 1 ];
        }

        inline unsigned long long knightMask() const
        {
            return pieceMask[ 2 ];
        }

        inline unsigned long long bishopMask() const
        {
            return pieceMask[ 3 ];
        }

        inline unsigned long long rookMask() const
        {
            return pieceMask[ 4 ];
        }

        inline unsigned long long queenMask() const
        {
            return pieceMask[ 5 ];
        }

        inline unsigned long long kingMask() const
        {
            return pieceMask[ 6 ];
        }

        inline unsigned long long allMask() const
        {
            return allPiecesMask;
        }
    };

    void makePieceBitboards( bool isWhite, PieceBitboards& ownBitboards, PieceBitboards& enemyBitboards ) const;

public:
    Board() :
        pieces( std::array< unsigned char, 64>() ),
        activeColor( Piece::getStartingColor() ),
        castlingRights( CastlingRights( true ) ),
        enPassantIndex( Utilities::getOffboardLocation() ),
        halfmoveClock( 0 ),
        fullmoveNumber( 1 )
    {
        std::fill( pieces.begin(), pieces.end(), Piece::emptyPiece() );
        LOG_TRACE( "DefaultConstructor" );
    };

    Board( std::array< unsigned char, 64 > pieces,
           unsigned char activeColor,
           CastlingRights castlingRights,
           unsigned short enPassantIndex,
           unsigned short halfmoveClock,
           unsigned short fullmoveNumber ) :
        pieces( pieces ),
        activeColor( activeColor ),
        castlingRights( castlingRights ),
        enPassantIndex( enPassantIndex ),
        halfmoveClock( halfmoveClock ),
        fullmoveNumber( fullmoveNumber )
    {
        validateCastlingRights();
        LOG_TRACE( "FullConstructor" );
    };

    Board( Board& board ) :
        pieces( board.pieces ),
        activeColor( board.activeColor ),
        castlingRights( board.castlingRights ),
        enPassantIndex( board.enPassantIndex ),
        halfmoveClock( board.halfmoveClock ),
        fullmoveNumber( board.fullmoveNumber )
    {
        // Plain copy, nothing to do
        LOG_TRACE( "CopyConstructor" );
    };

    Board( const Board& board ) :
        pieces( board.pieces ),
        activeColor( board.activeColor ),
        castlingRights( board.castlingRights ),
        enPassantIndex( board.enPassantIndex ),
        halfmoveClock( board.halfmoveClock ),
        fullmoveNumber( board.fullmoveNumber )
    {
        // Plain copy, nothing to do
        LOG_TRACE( "ConstCopyConstructor" );
    };

    static std::unique_ptr<Board> fromFEN( const Fen& fen )
    {
        return std::make_unique<Board>( fen.pieces, fen.activeColor, fen.castlingRights, fen.enPassantIndex, fen.halfmoveClock, fen.fullmoveNumber );
    }

    virtual ~Board()
    {
        // Do nothing
        LOG_TRACE( "Destructor" );
    }

    bool operator == ( const Board& board ) const
    {
        // TODO implement other attrs
        return positionMatch( board );
    }

    bool operator != ( const Board& board ) const
    {
        // TODO implement other attrs
        return !positionMatch( board );
    }

    Fen toFEN() const
    {
        return Fen( pieces, activeColor, castlingRights, enPassantIndex, halfmoveClock, fullmoveNumber );
    }

    std::string toFENString() const
    {
        return toFEN().toString();
    }

    /// <summary>
    /// Performs an "is this the same position" check, not a precise equality check
    /// </summary>
    /// <param name="board">the board to compare with</param>
    /// <returns>true if the on-board position is the same</returns>
    bool isSamePosition( const Board& board ) const;

    /// <summary>
    /// Returns a new board, based on the current board but with this move applied
    /// </summary>
    /// <param name="move">the move</param>
    /// <returns>a new board</returns>
    std::unique_ptr<Board> makeMove( Move* move );

    std::unique_ptr<MoveArray> getMoves();

    /// <summary>
    /// Looks for terminal positions and reports back with details as applied to the current board
    /// </summary>
    /// <param name="result">pointer to accept the outcome of a terminal position -1/0/+1</param>
    /// <returns>true if the position is terminal</returns>
    bool isTerminal( short* result );

    inline unsigned char getActiveColor()
    {
        return activeColor;
    }

    /// <summary>
    /// Convenience method to print the current board to the log
    /// </summary>
    /// <param name="title">a title for the log</param>
    Board& dumpBoard( std::string title = "" )
    {
        Utilities::dumpBoard( pieces, title );
        return *this;
    }

    friend class Evaluation;
};

