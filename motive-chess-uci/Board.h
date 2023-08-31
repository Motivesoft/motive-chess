#pragma once

#include <array>

#include "CastlingRights.h"
#include "Fen.h"
#include "Move.h"
#include "Piece.h"

class PieceMasks
{
public:
    inline static const unsigned short PAWN = 1;
    inline static const unsigned short KNIGHT = 2;
    inline static const unsigned short BISHOP = 3;
    inline static const unsigned short ROOK = 4;
    inline static const unsigned short QUEEN = 5;
    inline static const unsigned short KING = 6;

    unsigned long long masks[ 8 ];

    unsigned long long all;

    PieceMasks()
    {
        masks[ PAWN ] = 0;
        masks[ KNIGHT ] = 0;
        masks[ BISHOP ] = 0;
        masks[ ROOK ] = 0;
        masks[ QUEEN ] = 0;
        masks[ KING ] = 0;
    }
};

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
    void applyMove( const Move& move );

    void validateCastlingRights()
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

    bool failsCheckTests( unsigned long long protectedSquares );

    unsigned long long movesInARay( unsigned long long possibleMoves,
                                    unsigned long long rayMask,
                                    unsigned long long ownPieces,
                                    unsigned long long enemyPieces,
                                    unsigned long long aboveMask,
                                    unsigned long long belowMask,
                                    bool supportsCaptures = true );

    unsigned long long makePieceBitboard( unsigned char piece );

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
    };

    Board( const Fen& fen ) :
        pieces( fen.pieces ),
        activeColor( fen.activeColor ),
        castlingRights( fen.castlingRights ),
        enPassantIndex( fen.enPassantIndex ),
        halfmoveClock( fen.halfmoveClock ),
        fullmoveNumber( fen.fullmoveNumber )
    {
        validateCastlingRights();
    }

    virtual ~Board()
    {

    }

    bool operator == ( const Board& board )
    {
        // TODO implement other attrs
        return positionMatch( board );
    }

    bool operator != ( const Board& board )
    {
        // TODO implement other attrs
        return !positionMatch( board );
    }

    Fen toFEN()
    {
        return Fen( pieces, activeColor, castlingRights, enPassantIndex, halfmoveClock, fullmoveNumber );
    }

    /// <summary>
    /// Performs an "is this the same position" check, not a precise equality check
    /// </summary>
    /// <param name="board">the board to compare with</param>
    /// <returns>true if the on-board position is the same</returns>
    bool isSamePosition( const Board& board ) const;

    std::vector<Move> getMoves();

    /// <summary>
    /// Represents the state changed by the last move applied to this board via MakeMove that can undo that move in UndoMove
    /// </summary>
    class UndoState
    {
    private:
        std::array<unsigned char, 64> pieces;
        unsigned char activeColor;
        CastlingRights castlingRights;
        unsigned short enPassantIndex;
        unsigned short halfmoveClock;
        unsigned short fullmoveNumber;

    public:
        UndoState( Board& board ) :
            pieces( board.pieces ),
            activeColor( board.activeColor ),
            castlingRights( board.castlingRights ),
            enPassantIndex( board.enPassantIndex ),
            halfmoveClock( board.halfmoveClock ),
            fullmoveNumber( board.fullmoveNumber )
        {

        }

        friend class Board;
    };

    std::unique_ptr<UndoState> makeMove( const Move& move )
    {
        std::unique_ptr<UndoState> state = std::make_unique<UndoState>( *this );

        applyMove( move );

        return state;
    }

    void unmakeMove( std::unique_ptr<UndoState>& undoState )
    {
        pieces = undoState->pieces;
        activeColor = undoState->activeColor;
        castlingRights = undoState->castlingRights;
        enPassantIndex = undoState->enPassantIndex;
        halfmoveClock = undoState->halfmoveClock;
        fullmoveNumber = undoState->fullmoveNumber;
    }

    void mPP( unsigned char piece, PieceMasks& pieceMasks );
};
