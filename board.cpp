#include "board.h"
namespace Chess {

Vector<Move> Board::possibleMoves(const Coord from) const
{
    Vector<Move> movesList;
    Piece piece = squares[from];
    Coord to;

    if (piece.type() == Piece::Pawn) {
        /* Pawn Move */
        to = (piece.color() == Piece::White) ? from.nextRank() : from.prevRank();
        if (to.isValid() && !isOccupied(to)) {
            // Handle Promotion
            if (to.rank() == (piece.color() == Piece::White ? 7 : 0) ) {
                movesList.emplace_back(from, to, piece, squares[to], Move::PromoteToQueen);
                movesList.emplace_back(from, to, piece, squares[to], Move::PromoteToKnight);
                movesList.emplace_back(from, to, piece, squares[to], Move::PromoteToRook);
                movesList.emplace_back(from, to, piece, squares[to], Move::PromoteToBishop);
            } else {
                movesList.emplace_back(from, to, piece);
            }
        }

        /* Pawn double move */
        to = (piece.color() == Piece::White) ? from.nextRank() : from.prevRank();
        if (!piece.moved() && !isOccupied(to)) {
            to = (piece.color() == Piece::White) ? to.nextRank() : to.prevRank();
            if (to.isValid() && !isOccupied(to)) {
                movesList.emplace_back(from, to, piece);
            }
        }

        /* Pawn Take Left */
        to = (piece.color() == Piece::White) ? from.nextDiagAnti() : from.prevDiagAnti();
        if (to.isValid() && isOccupied(to) && squares[to].color() != piece.color()) {
            // Handle Promotion
            if (to.rank() == (piece.color() == Piece::White ? 7 : 0) ) {
                movesList.emplace_back(from, to, piece, squares[to], Move::PromoteToQueen);
                movesList.emplace_back(from, to, piece, squares[to], Move::PromoteToKnight);
                movesList.emplace_back(from, to, piece, squares[to], Move::PromoteToRook);
                movesList.emplace_back(from, to, piece, squares[to], Move::PromoteToBishop);
            } else {
                movesList.emplace_back(from, to, piece, squares[to]);
            }
        }

        /* Pawn Take Right */
        to = (piece.color() == Piece::White) ? from.nextDiagMain() : from.prevDiagMain();
        if (to.isValid() && isOccupied(to) && squares[to].color() != piece.color()) {
            // Handle Promotion
            if (to.rank() == (piece.color() == Piece::White ? 7 : 0) ) {
                movesList.emplace_back(from, to, piece, squares[to], Move::PromoteToQueen);
                movesList.emplace_back(from, to, piece, squares[to], Move::PromoteToKnight);
                movesList.emplace_back(from, to, piece, squares[to], Move::PromoteToRook);
                movesList.emplace_back(from, to, piece, squares[to], Move::PromoteToBishop);
            } else {
                movesList.emplace_back(from, to, piece, squares[to]);
            }
        }
    }

    if (piece.type() == Piece::King) {

        for (int direction=0; direction < 8; direction++) {
            switch (direction) {
                case 0: to = from.nextFile(); break;
                case 1: to = from.prevFile(); break;
                case 2: to = from.nextRank(); break;
                case 3: to = from.prevRank(); break;
                case 4: to = from.nextDiagMain(); break;
                case 5: to = from.prevDiagMain(); break;
                case 6: to = from.nextDiagAnti();break;
                case 7: to = from.prevDiagAnti(); break;
            }

            if (to.isValid() && (!isOccupied(to) || (isOccupied(to) && squares[to].color() != piece.color())))
                movesList.emplace_back(from, to, piece, squares[to]);
        }
    }

    if (piece.type() == Piece::Knight) {

        Coord diagMainF = from.nextDiagMain();
        Coord diagAntiF = from.nextDiagAnti();
        Coord diagMainB = from.prevDiagMain();
        Coord diagAntiB = from.prevDiagAnti();

        if (diagMainF.isValid()){
            to = diagMainF.nextFile();
            if (to.isValid() && (!isOccupied(to) || (isOccupied(to) && squares[to].color() != piece.color())))
                movesList.emplace_back(from, to, piece, squares[to]);
            to = diagMainF.nextRank();
            if (to.isValid() && (!isOccupied(to) || (isOccupied(to) && squares[to].color() != piece.color())))
                movesList.emplace_back(from, to, piece, squares[to]);
        }
        if (diagAntiF.isValid()){
            to = diagAntiF.prevFile();
            if (to.isValid() && (!isOccupied(to) || (isOccupied(to) && squares[to].color() != piece.color())))
                movesList.emplace_back(from, to, piece, squares[to]);
            to = diagAntiF.nextRank();
            if (to.isValid() && (!isOccupied(to) || (isOccupied(to) && squares[to].color() != piece.color())))
                movesList.emplace_back(from, to, piece, squares[to]);
        }
        if (diagMainB.isValid()){
            to = diagMainB.prevFile();
            if (to.isValid() && (!isOccupied(to) || (isOccupied(to) && squares[to].color() != piece.color())))
                movesList.emplace_back(from, to, piece, squares[to]);
            to = diagMainB.prevRank();
            if (to.isValid() && (!isOccupied(to) || (isOccupied(to) && squares[to].color() != piece.color())))
                movesList.emplace_back(from, to, piece, squares[to]);
        }
        if (diagAntiB.isValid()){
            to = diagAntiB.nextFile();
            if (to.isValid() && (!isOccupied(to) || (isOccupied(to) && squares[to].color() != piece.color())))
                movesList.emplace_back(from, to, piece, squares[to]);
            to = diagAntiB.prevRank();
            if (to.isValid() && (!isOccupied(to) || (isOccupied(to) && squares[to].color() != piece.color())))
                movesList.emplace_back(from, to, piece, squares[to]);
        }
    }

    if (piece.type() == Piece::Bishop || piece.type() == Piece::Queen ) {
        for(int direction=0; direction < 4; ++direction) {

            to = from;
            while(to.isValid()) {

                switch (direction) {
                case 0: to = to.nextDiagMain(); break;
                case 1: to = to.prevDiagMain(); break;
                case 2: to = to.nextDiagAnti(); break;
                case 3: to = to.prevDiagAnti(); break;
                }

                if (!to.isValid())
                    break;

                if (!isOccupied(to)) {
                    movesList.emplace_back(from, to, piece);
                } else {
                    if ( piece.color() != squares[to].color() )
                        movesList.emplace_back(from, to, piece, squares[to]);
                    break;
                }

            }
        }
    }

    if ( piece.type() == Piece::Rook || piece.type() == Piece::Queen) {
        for(int direction=0; direction < 4; ++direction) {

            to = from;
            while(to.isValid()) {

                switch (direction) {
                case 0: to = to.nextFile(); break;
                case 1: to = to.prevFile(); break;
                case 2: to = to.nextRank(); break;
                case 3: to = to.prevRank(); break;
                }

                if (!to.isValid())
                    break;

                if (!isOccupied(to)) {
                    movesList.emplace_back(from, to, piece);
                } else {
                    if ( piece.color() != squares[to].color() )
                        movesList.emplace_back(from, to, piece, squares[to]);
                    break;
                }

            }
        }
    }

    return movesList;
}

Vector<Move> Board::possibleMoves(Piece::Color forSide) const
{
    Vector<Move> movesList;

    for ( int index = 0; index < 64; ++index) {
        if (isOccupied(index) && squares[index].color() == forSide) {
            Vector<Move> pieceMoves = possibleMoves(Coord(index));
            movesList.insert(movesList.end(), pieceMoves.begin(), pieceMoves.end());
        }
    }
    return movesList;
}

void Board::make(Move move)
{
    Piece piece = move.originPiece();

    if (move.type() == Move::Standart){
        piece.setMoved(true);
        setPiece(move.origin(), Piece());
        setPiece(move.target(), piece);
    } else if (move.isPromotion()) {
        setPiece(move.origin(), Piece());
        setPiece(move.target(), move.promotedPiece());
    }

    movesDone.emplace_back(move);
    m_sideToMove = (side() == Piece::White) ? Piece::Black : Piece::White;
}

void Board::unmake()
{
    if (movesDone.size() == 0)
        return;

    Move move = movesDone.back();

    if (move.type() == Move::Standart || move.isPromotion()) {
        setPiece(move.origin(), move.originPiece());
        setPiece(move.target(), move.capturedPiece());
    }

    movesDone.pop_back();

    m_sideToMove = move.originPiece().color();
}

} // namespace ChessEngine

