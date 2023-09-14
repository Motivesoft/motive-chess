#pragma once

#include <array>
#include <sstream>
#include <string>

class CastlingRights
{
private:
    inline static const unsigned char WHITE_KINGSIDE  = 0b00001000;
    inline static const unsigned char WHITE_QUEENSIDE = 0b00000100;
    inline static const unsigned char BLACK_KINGSIDE  = 0b00000010;
    inline static const unsigned char BLACK_QUEENSIDE = 0b00000001;
    inline static const unsigned char ALL_RIGHTS      = 0b00001111;
    inline static const unsigned char NO_RIGHTS       = 0b00000000;

    static const std::string castlingFENStrings[ 16 ];

    unsigned char rights;

public:
    CastlingRights( bool initial = true ) :
        rights { initial ? ALL_RIGHTS : NO_RIGHTS }
    {

    }

    CastlingRights( CastlingRights& copy ) :
        rights( copy.rights )
    {

    }

    CastlingRights( const CastlingRights& copy ) :
        rights( copy.rights )
    {

    }

    virtual ~CastlingRights()
    {

    }

    bool operator == ( const CastlingRights& other ) const
    {
        return rights == other.rights;
    }

    bool operator != ( const CastlingRights& other ) const
    {
        return rights != other.rights;
    }

    static CastlingRights fromFENString( std::string fen )
    {
        CastlingRights castlingRights( false );

        for( std::string::iterator it = fen.begin(); it != fen.end(); it++ )
        {
            switch ( *it )
            {
                case 'K':
                    castlingRights.rights |= WHITE_KINGSIDE;
                    break;

                case 'Q':
                    castlingRights.rights |= WHITE_QUEENSIDE;
                    break;

                case 'k':
                    castlingRights.rights |= BLACK_KINGSIDE;
                    break;

                case 'q':
                    castlingRights.rights |= BLACK_QUEENSIDE;
                    break;

                default:
                    break;
            }
        }

        return castlingRights;
    }

    const std::string toString() const
    {
        return castlingFENStrings[ rights ];
    }

    void removeWhiteCastlingRights()
    {
        removeWhiteKingsideCastlingRights();
        removeWhiteQueensideCastlingRights();
    }

    void removeBlackCastlingRights()
    {
        removeBlackKingsideCastlingRights();
        removeBlackQueensideCastlingRights();
    }

    void removeWhiteKingsideCastlingRights()
    {
        rights &= ~WHITE_KINGSIDE;
    }

    void removeWhiteQueensideCastlingRights()
    {
        rights &= ~WHITE_QUEENSIDE;
    }

    void removeBlackKingsideCastlingRights()
    {
        rights &= ~BLACK_KINGSIDE;
    }

    void removeBlackQueensideCastlingRights()
    {
        rights &= ~BLACK_QUEENSIDE;
    }

    inline bool canWhiteCastleKingside() const
    {
        return rights & WHITE_KINGSIDE;
    }

    inline bool canWhiteCastleQueenside() const
    {
        return rights & WHITE_QUEENSIDE;
    }

    inline bool canBlackCastleKingside() const
    {
        return rights & BLACK_KINGSIDE;
    }

    inline bool canBlackCastleQueenside() const
    {
        return rights & BLACK_QUEENSIDE;
    }
};
