#ifndef BOARD_H
#define BOARD_H

#include "enginetypes.h"

namespace Chess {

class Board
{
public: // !!!
    Array<Piece,64> squares;
    Vector<Piece> capturedPieces;
    Vector<Move> movesDone;
    Piece::Color m_sideToMove;

public:
    Board() :
        squares(), m_sideToMove(Piece::White) {}

    static Board fromFEN(std::string fenRecord);

    bool isSquareAttacked(Coord square, Piece::Color attackingSide);

    bool isKingAttacked(Piece::Color side);

    Vector<Move> possibleMoves(const Coord from) ;

    Vector<Move> possibleMoves(Piece::Color forSide) ;

    void make(Move move);

    void unmake();

    inline bool isOccupied(Coord coord) const {
        return !squares[coord].isEmpty();
    }

    inline void setPiece(Coord coord, Piece piece) {
        if (coord.isValid()) {
            squares[coord] = piece;
        } else {
            qDebug() << "Board::setPiece() Invalid Coord: should never happen!";
        }
    }

    inline Piece piece(Coord coord) const {
        return squares[coord];
    }

    inline Piece::Color side() const {
        return m_sideToMove;
    }

    inline Piece & operator[](Coord square) {
        return squares[square];
    }
};

} // !namespace Chess

#endif // BOARD_H
