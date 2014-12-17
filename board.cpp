#include "board.h"
namespace Chess {

Vector<Move> Board::possibleMoves(const Coord from) const
{
    Vector<Move> movesList;
    Piece piece = squares[from];
    Coord to;

    if (piece.type() == Piece::Pawn) {
        /* Pawn Move */
        to = (piece.color() == Piece::White) ? from.nextFile() : from.prevFile();
        if (to.isValid() && !isOccupied(to)) {
                movesList.emplace_back(from, to, piece);
        }

        /* Pawn double move */
        to = (piece.color() == Piece::White) ? from.nextFile() : from.prevFile();
        if (!piece.moved() && !isOccupied(to)) {
            to = (piece.color() == Piece::White) ? to.nextFile() : to.prevFile();
            if (to.isValid() && !isOccupied(to)) {
                movesList.emplace_back(from, to, piece);
            }
        }

        /* Pawn Take Left */
        to = (piece.color() == Piece::White) ? from.nextDiagAnti() : from.prevDiagAnti();
        if (to.isValid() && isOccupied(to) && squares[to].color() != piece.color()) {
            movesList.emplace_back(from, to, piece, squares[to]);
        }
        /* Pawn Take Right */
        to = (piece.color() == Piece::White) ? from.nextDiagMain() : from.prevDiagMain();
        if (to.isValid() && isOccupied(to) && squares[to].color() != piece.color()) {
            movesList.emplace_back(from, to, piece, squares[to]);
        }
    }
    if (piece.type() == Piece::King) {
        to = from.nextRank();
        if (to.isValid() && (!isOccupied(to) || (isOccupied(to) && squares[to].color() != piece.color())))
            movesList.emplace_back(from, to, piece, squares[to]);
        to = from.prevRank();
        if (to.isValid() && (!isOccupied(to) || (isOccupied(to) && squares[to].color() != piece.color())))
            movesList.emplace_back(from, to, piece, squares[to]);
        to = from.nextFile();
        if (to.isValid() && (!isOccupied(to) || (isOccupied(to) && squares[to].color() != piece.color())))
            movesList.emplace_back(from, to, piece, squares[to]);
        to = from.prevFile();
        if (to.isValid() && (!isOccupied(to) || (isOccupied(to) && squares[to].color() != piece.color())))
            movesList.emplace_back(from, to, piece, squares[to]);

        to = from.nextDiagMain();
        if (to.isValid() && (!isOccupied(to) || (isOccupied(to) && squares[to].color() != piece.color())))
            movesList.emplace_back(from, to, piece, squares[to]);
        to = from.prevDiagMain();
        if (to.isValid() && (!isOccupied(to) || (isOccupied(to) && squares[to].color() != piece.color())))
            movesList.emplace_back(from, to, piece, squares[to]);
        to = from.nextDiagAnti();
        if (to.isValid() && (!isOccupied(to) || (isOccupied(to) && squares[to].color() != piece.color())))
            movesList.emplace_back(from, to, piece, squares[to]);
        to = from.prevDiagAnti();
        if (to.isValid() && (!isOccupied(to) || (isOccupied(to) && squares[to].color() != piece.color())))
            movesList.emplace_back(from, to, piece, squares[to]);

    }

    if (piece.type() == Piece::Knight) {

        Coord diagMainF = from.nextDiagMain();
        Coord diagAntiF = from.nextDiagAnti();
        Coord diagMainB = from.prevDiagMain();
        Coord diagAntiB = from.prevDiagAnti();

        if (diagMainF.isValid()){
            to = diagMainF.nextRank();
            if (to.isValid() && (!isOccupied(to) || (isOccupied(to) && squares[to].color() != piece.color())))
                movesList.emplace_back(from, to, piece, squares[to]);
            to = diagMainF.nextFile();
            if (to.isValid() && (!isOccupied(to) || (isOccupied(to) && squares[to].color() != piece.color())))
                movesList.emplace_back(from, to, piece, squares[to]);
        }
        if (diagAntiF.isValid()){
            to = diagAntiF.prevRank();
            if (to.isValid() && (!isOccupied(to) || (isOccupied(to) && squares[to].color() != piece.color())))
                movesList.emplace_back(from, to, piece, squares[to]);
            to = diagAntiF.nextFile();
            if (to.isValid() && (!isOccupied(to) || (isOccupied(to) && squares[to].color() != piece.color())))
                movesList.emplace_back(from, to, piece, squares[to]);
        }
        if (diagMainB.isValid()){
            to = diagMainB.prevRank();
            if (to.isValid() && (!isOccupied(to) || (isOccupied(to) && squares[to].color() != piece.color())))
                movesList.emplace_back(from, to, piece, squares[to]);
            to = diagMainB.prevFile();
            if (to.isValid() && (!isOccupied(to) || (isOccupied(to) && squares[to].color() != piece.color())))
                movesList.emplace_back(from, to, piece, squares[to]);
        }
        if (diagAntiB.isValid()){
            to = diagAntiB.nextRank();
            if (to.isValid() && (!isOccupied(to) || (isOccupied(to) && squares[to].color() != piece.color())))
                movesList.emplace_back(from, to, piece, squares[to]);
            to = diagAntiB.prevFile();
            if (to.isValid() && (!isOccupied(to) || (isOccupied(to) && squares[to].color() != piece.color())))
                movesList.emplace_back(from, to, piece, squares[to]);
        }
    }

    if (piece.type() == Piece::Bishop || piece.type() == Piece::Queen ) {
        /* Piece ray up-right */
        for (to=from.nextDiagMain(); to.isValid(); to=to.nextDiagMain()){
            if (isOccupied(to)) {
                if (piece.color() != squares[to].color() ) {
                    movesList.emplace_back(from, to, piece, squares[to]);
                }
                break;
            }
            movesList.emplace_back(from, to, piece);
        }
        /* Piece ray down-left */
        for (to=from.prevDiagMain(); to.isValid(); to=to.prevDiagMain()){
            if (isOccupied(to)) {
                if (piece.color() != squares[to].color() ) {
                    movesList.emplace_back(from, to, piece, squares[to]);
                }
                break;
            }
            movesList.emplace_back(from, to, piece);
        }
        /* Piece ray up-left */
        for (to=from.nextDiagAnti(); to.isValid(); to=to.nextDiagAnti()){
            if (isOccupied(to)) {
                if (piece.color() != squares[to].color() ) {
                    movesList.emplace_back(from, to, piece, squares[to]);
                }
                break;
            }
            movesList.emplace_back(from, to, piece);
        }
        /* Piece ray down-left */
        for (to=from.prevDiagAnti(); to.isValid(); to=to.prevDiagAnti()){
            if (isOccupied(to)) {
                if (piece.color() != squares[to].color() ) {
                    movesList.emplace_back(from, to, piece, squares[to]);
                }
                break;
            }
            movesList.emplace_back(from, to, piece);
        }
    }

    if ( piece.type() == Piece::Rook || piece.type() == Piece::Queen) {
        /* Piece ray right */
        for (to=from.nextRank(); to.isValid(); to=to.nextRank()){
            if (isOccupied(to)) {
                if (piece.color() != squares[to].color() ) {
                    movesList.emplace_back(from, to, piece, squares[to]);
                }
                break;
            }
            movesList.emplace_back(from, to, piece);
        }
        /* Piece ray left */
        for (to=from.prevRank(); to.isValid(); to=to.prevRank()){
            if (isOccupied(to)) {
                if (piece.color() != squares[to].color() ) {
                    movesList.emplace_back(from, to, piece, squares[to]);
                }
                break;
            }
            movesList.emplace_back(from, to, piece);
        }
        /* Piece ray up */
        for (to=from.nextFile(); to.isValid(); to=to.nextFile()){
            if (isOccupied(to)) {
                if (piece.color() != squares[to].color() ) {
                    movesList.emplace_back(from, to, piece, squares[to]);
                }
                break;
            }
            movesList.emplace_back(from, to, piece);
        }
        /* Piece ray down */
        for (to=from.prevFile(); to.isValid(); to=to.prevFile()){
            if (isOccupied(to)) {
                if (piece.color() != squares[to].color() ) {
                    movesList.emplace_back(from, to, piece, squares[to]);
                }
                break;
            }
            movesList.emplace_back(from, to, piece);
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
    Piece srcPiece = move.sourcePiece();
    srcPiece.setMoved(true);

    setPiece(move.source(), Piece());
    setPiece(move.target(), srcPiece);

    movesDone.emplace_back(move);
    m_sideToMove = (side() == Piece::White) ? Piece::Black : Piece::White;
}

void Board::unmake()
{
    Move move = movesDone.back();
    setPiece(move.source(), move.sourcePiece());
    setPiece(move.target(), move.targetPiece());
    movesDone.pop_back();

    m_sideToMove = move.sourcePiece().color();
}

} // namespace ChessEngine

