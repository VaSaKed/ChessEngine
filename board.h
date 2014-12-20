#ifndef BOARD_H
#define BOARD_H

#include "enginetypes.h"

namespace Chess {

class Board
{
public: // !!!
    Array<Piece,64> squares;
    Vector<Move> movesDone;
    Piece::Color m_sideToMove;

public:
    Board() :
        squares(), m_sideToMove(Piece::White) {}

    Vector<Move> possibleMoves(const Coord from) const ;
    Vector<Move> possibleMoves(Piece::Color forSide) const ;

    void make(Move move);
    void unmake();

    inline bool isOccupied(Coord coord) const {
        return !squares[coord].isEmpty();
    }

    inline void setPiece(Coord coord, Piece piece) {
        squares[coord] = piece;
    }

    inline Piece piece(Coord coord) const {
        return squares[coord];
    }

    inline Piece::Color side() const {
        return m_sideToMove;
    }
};

} // !namespace Chess

#endif // BOARD_H
