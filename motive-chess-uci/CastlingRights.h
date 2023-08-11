#pragma once

#include <array>
#include <sstream>
#include <string>

class CastlingRights
{
private:
    std::array<bool,4> rights;

    inline bool canCastle( unsigned short index )
    {
        return rights[ index ];
    }

public:
    CastlingRights( bool initial = true ) :
        rights { initial, initial, initial, initial }
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

    bool operator == ( const CastlingRights& other )
    {
        return rights == other.rights;
    }

    bool operator != ( const CastlingRights& other )
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
                    castlingRights.rights[ 0 ] = true;
                    break;

                case 'Q':
                    castlingRights.rights[ 1 ] = true;
                    break;

                case 'k':
                    castlingRights.rights[ 2 ] = true;
                    break;

                case 'q':
                    castlingRights.rights[ 3 ] = true;
                    break;

                default:
                    break;
            }
        }

        return castlingRights;
    }

    static std::string toFENString( CastlingRights& castlingRights )
    {
        std::stringstream stream;

        if ( castlingRights.canWhiteCastleKingside() )
        {
            stream << "K";
        }
        if ( castlingRights.canWhiteCastleQueenside() )
        {
            stream << "Q";
        }
        if ( castlingRights.canBlackCastleKingside() )
        {
            stream << "k";
        }
        if ( castlingRights.canBlackCastleQueenside() )
        {
            stream << "q";
        }
        if ( stream.str().empty() )
        {
            stream << "-";
        }

        return stream.str();
    }

    std::string toString()
    {
        return toFENString( *this );
    }

    void removeWhiteCastlingRights()
    {
        removeWhiteKingsideCastlingRights();
        removeWhiteQueensideCastlingRights();
    }

    void removeWhiteKingsideCastlingRights()
    {
        rights[ 0 ] = false;
    }

    void removeWhiteQueensideCastlingRights()
    {
        rights[ 1 ] = false;
    }

    void removeBlackCastlingRights()
    {
        removeBlackKingsideCastlingRights();
        removeBlackQueensideCastlingRights();
    }

    void removeBlackKingsideCastlingRights()
    {
        rights[ 2 ] = false;
    }

    void removeBlackQueensideCastlingRights()
    {
        rights[ 3 ] = false;
    }

    inline bool canWhiteCastleKingside()
    {
        return canCastle( 0 );
    }

    inline bool canWhiteCastleQueenside()
    {
        return canCastle( 1 );
    }

    inline bool canBlackCastleKingside()
    {
        return canCastle( 2 );
    }

    inline bool canBlackCastleQueenside()
    {
        return canCastle( 3 );
    }
};
