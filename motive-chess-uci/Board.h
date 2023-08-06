#pragma once

class Board
{
private:
    unsigned char pieces[ 64 ];

    bool positionMatch( const Board& board )
    {
        // TODO keep this up to date with any additional state
        for ( int loop = 0; loop < 64; loop++ )
        {
            if ( pieces[ loop ] != board.pieces[ loop ] )
            {
                return false;
            }
        }
        return true;
    }

public:
    Board()
    {

    }

    Board( Board& board )
    {

    }

    virtual ~Board()
    {

    }

    bool operator == ( const Board& board )
    {
        return positionMatch( board );
    }

    bool operator != ( const Board& board )
    {
        return !positionMatch( board );
    }

    inline static unsigned short squareToIndex( unsigned short file, unsigned short rank )
    {
        return (rank << 3) + (file & 7);
    }

    inline static unsigned short indexToFile( unsigned short index )
    {
        return index & 7;
    }

    inline static unsigned short indexToRank( unsigned short index )
    {
        return index >> 3;
    }
};

