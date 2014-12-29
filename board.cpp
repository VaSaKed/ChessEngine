#include "board.h"
namespace Chess {

bool Board::isSquareAttacked(Coord where, Piece::Color attackingSide)
{
    for (int direction=North; direction < DirectionNumber; ++direction) {

        int distance = 0;
        Coord from = where;

        while (from.isValid()) {

            switch (direction) {
            case East:      from = from.nextFile(); break;
            case West:      from = from.prevFile(); break;
            case North:     from = from.nextRank(); break;
            case South:     from = from.prevRank(); break;
            case NorthEast: from = from.nextDiagMain(); break;
            case SouthWest: from = from.prevDiagMain(); break;
            case NorthWest: from = from.nextDiagAnti(); break;
            case SouthEast: from = from.prevDiagAnti(); break;
            }

            distance++;

            if (!from.isValid())
                break;

            if (isOccupied(from)) {
                if (squares[from].color() == attackingSide) {
                    Piece attacker = squares[from];
                    switch(direction){
                    case North: case South: case East: case West: {
                        if ( attacker.type() == Piece::Rook || attacker.type() == Piece::Queen )
                            return true;
                        else if (distance == 1 && attacker.type() == Piece::King)
                            return true;
                        break;
                    }
                    case NorthEast: case SouthWest: case NorthWest: case SouthEast: {
                        if ( attacker.type() == Piece::Bishop || attacker.type() == Piece::Queen ) {
                            return true;
                        } else if (distance == 1 && attacker.type() == Piece::King){
                            return true;
                        } else if (distance == 1 && attacker.type() == Piece::Pawn) {
                            if (attacker.color() == Piece::White && (direction == SouthEast || direction == SouthWest))
                                return true;
                            else if (attacker.color() == Piece::Black && (direction == NorthEast || direction == NorthWest))
                                return true;
                        }
                        break;
                    }
                    }
                }
                break; //  if (isOccupied(from))
            }
        }
    }

    /* Knight's offsets clockwise */
    Coord knightAttacker;
    static const sint8 knightOffsets[8][2] = { {+1,+2}, {+2,+1}, {+2,-1}, {+1,-2}, {-1,-2}, {-2,-1}, {-2,+1}, {-1,+2} };
    for (size_t i=0; i < 8; ++i) {
        knightAttacker = Coord( where.file() + knightOffsets[i][0], where.rank() + knightOffsets[i][1]);
        if (knightAttacker.isValid() && squares[knightAttacker].type() == Piece::Knight && squares[knightAttacker].color() == attackingSide)
            return true;
    }


    return false;
}

/* TODO: optimize, looks silly, should be faster */
bool Board::isKingAttacked(Piece::Color side)
{
    for (int i = 0; i < 64; ++i) {
        if (squares[i].type() == Piece::King && squares[i].color() == side) {
            return isSquareAttacked(i, !side);
        }
    }
    return false;
}



Vector<Move> Board::possibleMoves(Coord from)
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
                uint8 moveFlags = piece.moved() ? Move::PawnMoveFlag : Move::PawnMoveFlag | Move::FirstMoveFlag;
                movesList.emplace_back(from, to, moveFlags, Move::PromoteToQueen);
                movesList.emplace_back(from, to, moveFlags, Move::PromoteToKnight);
                movesList.emplace_back(from, to, moveFlags, Move::PromoteToRook);
                movesList.emplace_back(from, to, moveFlags, Move::PromoteToBishop);
            } else {
                movesList.emplace_back(from, to, !piece.moved(), false);
            }
        }

        if (!piece.moved()) {
            /* Pawn double move */
            to = (piece.color() == Piece::White) ? from.nextRank() : from.prevRank();
            if (to.isValid() && !isOccupied(to)) {
                to = (piece.color() == Piece::White) ? to.nextRank() : to.prevRank();
                if (to.isValid() && !isOccupied(to)) {
                    movesList.emplace_back(from, to, Move::FirstMoveFlag | Move::PawnMoveFlag, Move::DoublePush);
                }
            }
        }

        /* Pawn Take Left */
        to = (piece.color() == Piece::White) ? from.nextDiagAnti() : from.prevDiagAnti();
        if (to.isValid() && isOccupied(to) && squares[to].color() != piece.color()) {
            // Handle Promotion
            if (to.rank() == (piece.color() == Piece::White ? 7 : 0) ) {
                uint8 moveFlags = piece.moved() ? Move::PawnMoveFlag : Move::PawnMoveFlag | Move::FirstMoveFlag;
                moveFlags |= Move::CaptureFlag;
                movesList.emplace_back(from, to, moveFlags, Move::PromoteToQueen);
                movesList.emplace_back(from, to, moveFlags, Move::PromoteToKnight);
                movesList.emplace_back(from, to, moveFlags, Move::PromoteToRook);
                movesList.emplace_back(from, to, moveFlags, Move::PromoteToBishop);
            } else {
                movesList.emplace_back(from, to, !piece.moved(), true);
            }
        }

        /* Pawn Take Right */
        to = (piece.color() == Piece::White) ? from.nextDiagMain() : from.prevDiagMain();
        if (to.isValid() && isOccupied(to) && squares[to].color() != piece.color()) {
            // Handle Promotion
            if (to.rank() == (piece.color() == Piece::White ? 7 : 0) ) {
                uint8 moveFlags = piece.moved() ? Move::PawnMoveFlag : Move::PawnMoveFlag | Move::FirstMoveFlag;
                moveFlags |= Move::CaptureFlag;
                movesList.emplace_back(from, to, moveFlags, Move::PromoteToQueen);
                movesList.emplace_back(from, to, moveFlags, Move::PromoteToKnight);
                movesList.emplace_back(from, to, moveFlags, Move::PromoteToRook);
                movesList.emplace_back(from, to, moveFlags, Move::PromoteToBishop);
            } else {
                movesList.emplace_back(from, to, !piece.moved(), true);
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
                movesList.emplace_back(from, to, !piece.moved(), isOccupied(to));
        }

//        /* Castling Left*/
//        if (!piece.moved()) {
//            Coord castleLeft  = Coord(2, from.rank());
//            Coord castleRight = Coord(6, from.rank());
//            Coord rookLeft    = Coord(0, from.rank());
//            Coord rookRight   = Coord(7, from.rank());

//            if ( !squares[rookLeft].moved() ) {
//                bool canCastleLeft= true;
//                for (Coord path = from.prevFile(); path != rookLeft; path = path.prevFile()) {
//                    if (isOccupied(path)) {
//                        canCastleLeft = false;
//                        break;
//                    }
//                }

//                if (canCastleLeft) {
//                    movesList.emplace_back(from, castleLeft, !piece.moved(), squares[castleLeft], Move::CastleLeft );
//                }
//            }
//        }
    }

    if (piece.type() == Piece::Knight) {
        /* Knight's offsets clockwise */
        static const sint8 knightOffsets[8][2] = { {+1,+2}, {+2,+1}, {+2,-1}, {+1,-2}, {-1,-2}, {-2,-1}, {-2,+1}, {-1,+2} };

        for (size_t i=0; i < 8; ++i) {
            to = Coord( from.file() + knightOffsets[i][0], from.rank() + knightOffsets[i][1]);
            if (to.isValid() && !(isOccupied(to) && squares[to].color() == piece.color()))
                movesList.emplace_back(from, to, !piece.moved(), isOccupied(to));
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
                    movesList.emplace_back(from, to, !piece.moved(), false);
                } else {
                    if ( piece.color() != squares[to].color() )
                        movesList.emplace_back(from, to, !piece.moved(), true);
                    break;
                }

            }
        }
    }

    if (piece.type() == Piece::Rook || piece.type() == Piece::Queen) {
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
                    movesList.emplace_back(from, to, !piece.moved(), false);
                } else {
                    if ( piece.color() != squares[to].color() )
                        movesList.emplace_back(from, to, !piece.moved(), true);
                    break;
                }

            }
        }
    }


    /* Remove all moves that put our king under attack. */
    for (size_t i=0; i < movesList.size(); ++i) {

        Move move = movesList[i];
        make(move);

        if (isKingAttacked(piece.color())) {
            movesList[i--] = movesList.back();  // delete i from movesList
            movesList.pop_back();
        }

        unmake();
    }

    return movesList;
}

Vector<Move> Board::possibleMoves(Piece::Color forSide)
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
    Piece piece = squares[move.origin()];

    if (move.flags() & Move::FirstMoveFlag)
        piece.setMoved(true);

    if (move.flags() & Move::CaptureFlag) {
        capturedPieces.push_back(squares[move.target()]);
    }

    setPiece(move.origin(), Piece());
    setPiece(move.target(), piece);

    movesDone.emplace_back(move);
    m_sideToMove = !piece.color();
}

void Board::unmake()
{
    if (movesDone.size() == 0)
        return;

    Move move = movesDone.back();
    Piece piece = squares[move.target()];

    if (move.flags() & Move::FirstMoveFlag)
        piece.setMoved(false);

    if (move.flags() & Move::CaptureFlag) {
        setPiece(move.origin(), piece);
        setPiece(move.target(), capturedPieces.back());
        capturedPieces.pop_back();
    } else {
        setPiece(move.origin(), piece);
        setPiece(move.target(), Piece());
    }

    movesDone.pop_back();

    m_sideToMove = piece.color();
}

} // namespace ChessEngine

