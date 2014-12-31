#include "board.h"
#include <sstream>

namespace Chess {

Board Board::fromFEN(std::string fenRecord)
{
    using namespace std;
    Board board;

    /* split fenRecord */
    istringstream iss(fenRecord);
    vector<std::string> records;
    copy(istream_iterator<std::string>(iss), istream_iterator<string>(), back_inserter(records));

    int rank = 7;
    int file = 0;
    size_t i;
    /* find the positions of the pieces */
    for ( i = 0; i < fenRecord.size() && fenRecord[i] != ' '; ++i) {

        char ch = fenRecord[i];
        if (ch == '/') {
            rank -= 1;
            file = 0;
        } else  if (std::isdigit(ch)) {
            int digit = ch - '0';
            file += digit;
        } else {
            Piece piece;

            char name = std::toupper(ch);
            if (name == 'P') {
                piece = Piece(Piece::Pawn, std::isupper(ch) ? Piece::White : Piece::Black);
            } else if (name == 'N') {
                piece = Piece(Piece::Knight, std::isupper(ch) ? Piece::White : Piece::Black);
            } else if (name == 'B') {
                piece = Piece(Piece::Bishop, std::isupper(ch) ? Piece::White : Piece::Black);
            } else if (name == 'R') {
                piece = Piece(Piece::Rook, std::isupper(ch) ? Piece::White : Piece::Black);
            } else if (name == 'Q') {
                piece = Piece(Piece::Queen, std::isupper(ch) ? Piece::White : Piece::Black);
            } else if (name == 'K') {
                piece = Piece(Piece::King, std::isupper(ch) ? Piece::White : Piece::Black);
            }

            board.setPiece(Coord(file, rank), piece);
            ++file;
        }
    }

    /* get active color */
    if (records.size() >= 2)
        board.m_sideToMove = records[1] == "b" ? Piece::Black : Piece::White;


    return board;
}

/* TODO: Add enpassant attacker check */
bool Board::isSquareAttacked(Coord square, Piece::Color attackingSide)
{
    for (int direction=North; direction < DirectionMax; direction += Rotate_45_Degree) {

        int distance = 0;
        Coord from = square;

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
                    switch(direction) {
                    case North: case South: case East: case West: {
                        if ( attacker.isRook() || attacker.isQueen() )
                            return true;
                        else if (distance == 1 && attacker.isKing())
                            return true;
                        break;
                    }
                    case NorthEast: case SouthWest: case NorthWest: case SouthEast: {
                        if ( attacker.isBishop() || attacker.isQueen() ) {
                            return true;
                        } else if (distance == 1 && attacker.isKing()){
                            return true;
                        } else if (distance == 1 && attacker.isPawn()) {
                            if (attacker.isWhite() && (direction == SouthEast || direction == SouthWest))
                                return true;
                            else if (attacker.isBlack() && (direction == NorthEast || direction == NorthWest))
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
        knightAttacker = Coord( square.file() + knightOffsets[i][0], square.rank() + knightOffsets[i][1]);
        if (knightAttacker.isValid() && squares[knightAttacker].isKnight() && squares[knightAttacker].color() == attackingSide)
            return true;
    }


    return false;
}

/* TODO: optimize, looks silly, should be faster */
bool Board::isKingAttacked(Piece::Color side)
{
    for (int i = 0; i < 64; ++i) {
        if (squares[i].isKing() && squares[i].color() == side) {
            return isSquareAttacked(i, !side);
        }
    }
    return false;
}



Vector<Move> Board::possibleMoves(const Coord from)
{
    Vector<Move> movesList;
    Piece piece = squares[from];
    Coord to;

    if (piece.isPawn() ) {
        /* Pawn Move */
        to = (piece.isWhite() ? from.nextRank() : from.prevRank() );
        if (to.isValid() && !isOccupied(to)) {
            // Handle Promotion
            if (to.rank() == (piece.isWhite() ? 7 : 0) ) {
                uint8 moveFlags = piece.moved() ? Move::PawnMoveFlag : Move::PawnMoveFlag | Move::FirstMoveFlag;
                movesList.emplace_back(from, to, moveFlags, Move::PromoteToQueen);
                movesList.emplace_back(from, to, moveFlags, Move::PromoteToKnight);
                movesList.emplace_back(from, to, moveFlags, Move::PromoteToRook);
                movesList.emplace_back(from, to, moveFlags, Move::PromoteToBishop);
            } else {
                movesList.emplace_back(from, to, !piece.moved(), false);
            }
        }

        if (!piece.moved() ) {
            /* Pawn double move */
            to = (piece.isWhite() ? from.nextRank() : from.prevRank() );
            if (to.isValid() && !isOccupied(to) ) {
                to = (piece.isWhite()) ? to.nextRank() : to.prevRank();
                if (to.isValid() && !isOccupied(to) ) {
                    movesList.emplace_back(from, to, Move::FirstMoveFlag | Move::PawnMoveFlag, Move::DoubleStep);
                }
            }
        }

        /* Pawn Takes Left */
        to = (piece.isWhite() ? from.nextDiagAnti() : from.prevDiagAnti() );
        if (to.isValid() && isOccupied(to) && squares[to].color() != piece.color() ) {
            // Handle Promotion
            if (to.rank() == (piece.isWhite() ? 7 : 0) ) {
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

        /* Pawn Takes Right */
        to = (piece.isWhite() ? from.nextDiagMain() : from.prevDiagMain() );
        if (to.isValid() && isOccupied(to) && !piece.sameColor(squares[to]) ) {
            // Handle Promotion
            if (to.rank() == (piece.isWhite() ? 7 : 0) ) {
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

        /* En Passant */
        if (movesDone.size() > 0 && (movesDone.back().flags() & Move::PawnMoveFlag) && movesDone.back().type() == Move::DoubleStep) {
            Coord enPasPawn = movesDone.back().target();

            if (from.sameRank(enPasPawn) && from.chebyshevDistance(enPasPawn) == 1) {
                uint8 moveFlags = piece.moved() ? Move::PawnMoveFlag : Move::PawnMoveFlag | Move::FirstMoveFlag;
                moveFlags |= Move::CaptureFlag;
                to = (squares[enPasPawn].isWhite() ? enPasPawn.prevRank() : enPasPawn.nextRank() );
                movesList.emplace_back(from, to, moveFlags, Move::EnPassant);
            }
        }
    }

    if (piece.isKing()) {

        for (int direction=North; direction < DirectionMax; direction += Rotate_90_Degree) {
            switch (direction) {
            case East:      to = from.nextFile(); break;
            case West:      to = from.prevFile(); break;
            case North:     to = from.nextRank(); break;
            case South:     to = from.prevRank(); break;
            case NorthEast: to = from.nextDiagMain(); break;
            case SouthWest: to = from.prevDiagMain(); break;
            case NorthWest: to = from.nextDiagAnti(); break;
            case SouthEast: to = from.prevDiagAnti(); break;
            default: ; // honestly it really should not happen
            }

            if (to.isValid() && (!isOccupied(to) || (isOccupied(to) && !piece.sameColor(squares[to]) )))
                movesList.emplace_back(from, to, !piece.moved(), isOccupied(to));
        }

        /* Castling */
        if (!piece.moved() ) {
            Coord castleLeft  = Coord(2, from.rank() );
            Coord castleRight = Coord(6, from.rank() );
            Coord rookLeft    = Coord(0, from.rank() );
            Coord rookRight   = Coord(7, from.rank() );

            if (squares[rookLeft].isRook() && !squares[rookLeft].moved() ) {
                bool canCastleLeft= true;
                for (Coord path = from.prevFile(); path != rookLeft; path = path.prevFile() ) {
                    if (isOccupied(path)) {
                        canCastleLeft = false;
                        break;
                    }
                }

                for (Coord path = from; path != castleLeft; path = path.prevFile() ) {
                    if (isSquareAttacked(path, !piece.color() )) {
                        canCastleLeft = false;
                        break;
                    }
                }

                if (canCastleLeft) {
                    uint8 moveFlags = Move::FirstMoveFlag | Move::KingMoveFlag;
                    movesList.emplace_back(from, castleLeft, moveFlags, Move::CastleLeft);
                }
            }

            if (squares[rookRight].isRook() && !squares[rookRight].moved() ) {
                bool canCastleRight= true;
                for (Coord path = from.nextFile(); path != rookRight; path = path.nextFile() ) {
                    if (isOccupied(path)) {
                        canCastleRight = false;
                        break;
                    }
                }

                for (Coord path = from; path != castleRight; path = path.nextFile() ) {
                    if (isSquareAttacked(path, !piece.color() )) {
                        canCastleRight = false;
                        break;
                    }
                }

                if (canCastleRight) {
                    uint8 moveFlags = Move::FirstMoveFlag | Move::KingMoveFlag;
                    movesList.emplace_back(from, castleRight, moveFlags, Move::CastleRight );
                }
            }
        }
    }

    if (piece.isKnight() ) {
        /* Knight's offsets clockwise */
        static const sint8 knightOffsets[8][2] = { {+1,+2}, {+2,+1}, {+2,-1}, {+1,-2}, {-1,-2}, {-2,-1}, {-2,+1}, {-1,+2} };

        for (size_t i=0; i < 8; ++i) {
            to = Coord( from.file() + knightOffsets[i][0], from.rank() + knightOffsets[i][1]);
            if (to.isValid() && !(isOccupied(to) && piece.sameColor(squares[to]) ))
                movesList.emplace_back(from, to, !piece.moved(), isOccupied(to) );
        }
    }

    if (piece.isBishop() || piece.isQueen() ) {
        for(int direction=NorthEast; direction < DirectionMax; direction += Rotate_90_Degree) {

            to = from;
            while(to.isValid() ) {

                switch (direction) {
                case NorthEast: to = to.nextDiagMain(); break;
                case SouthWest: to = to.prevDiagMain(); break;
                case NorthWest: to = to.nextDiagAnti(); break;
                case SouthEast: to = to.prevDiagAnti(); break;
                default: ; // honestly it really should not happen
                }

                if (!to.isValid() )
                    break;

                if (!isOccupied(to) ) {
                    movesList.emplace_back(from, to, !piece.moved(), false);
                } else {
                    if ( !piece.sameColor(squares[to]) )
                        movesList.emplace_back(from, to, !piece.moved(), true);
                    break;
                }

            }
        }
    }

    if (piece.isRook() || piece.isQueen() ) {
        for(int direction=East; direction < DirectionMax; direction += Rotate_90_Degree) {

            to = from;
            while(to.isValid() ) {

                switch (direction) {
                case East:      to = to.nextFile(); break;
                case West:      to = to.prevFile(); break;
                case North:     to = to.nextRank(); break;
                case South:     to = to.prevRank(); break;
                default: ; // honestly it really should not happen
                }

                if (!to.isValid() )
                    break;

                if (!isOccupied(to) ) {
                    movesList.emplace_back(from, to, !piece.moved(), false);
                } else {
                    if ( !piece.sameColor(squares[to]) )
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

        if (isKingAttacked(piece.color() )) {
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
            Vector<Move> pieceMoves = possibleMoves(Coord(index) );
            movesList.insert(movesList.end(), pieceMoves.begin(), pieceMoves.end() );
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
        if (move.flags() & Move::PawnMoveFlag && move.type() == Move::EnPassant){
            Piece::Color side = squares[move.origin()].color() ;
            Coord capturedPawn = (side == Piece::White ? move.target().prevRank(): move.target().nextRank() );
            capturedPieces.push_back(squares[capturedPawn]);
            setPiece(capturedPawn, Piece());
        } else {
            capturedPieces.push_back(squares[move.target()]);
        }
    }

    if (move.isCastle() ) {
        Coord rookOrig    = (move.type() == Move::CastleLeft
                ? Coord(0, move.origin().rank() )
                : Coord(7, move.origin().rank() ));
        Coord rookTrgt = (move.type() == Move::CastleLeft
                ? Coord(3, move.origin().rank() )
                : Coord(5, move.origin().rank() ));
        Piece rook = squares[rookOrig];
        rook.setMoved(true);
        setPiece(rookOrig, Piece() );
        setPiece(rookTrgt, rook);

    } else if (move.isPromotion() ){
        switch (move.type() ) {
        case Move::PromoteToQueen:  piece = Piece(Piece::Queen,  piece.color()); break;
        case Move::PromoteToKnight: piece = Piece(Piece::Knight, piece.color()); break;
        case Move::PromoteToBishop: piece = Piece(Piece::Bishop, piece.color()); break;
        case Move::PromoteToRook:   piece = Piece(Piece::Rook,   piece.color()); break;
        default:;
        }
    }

    setPiece(move.origin(), Piece() );
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
    Piece piece_trgt;

    if (move.flags() & Move::CaptureFlag) {
        if (move.flags() & Move::PawnMoveFlag && move.type() == Move::EnPassant){
            Piece::Color side = squares[move.target()].color() ;
            Coord capturedPawn = (side == Piece::White ? move.target().prevRank(): move.target().nextRank() );
            setPiece(capturedPawn, capturedPieces.back() );
        }else {
            piece_trgt = capturedPieces.back();
        }
        capturedPieces.pop_back();
    }

    if (move.isCastle() ) {
        Coord rookOrig    = (move.type() == Move::CastleLeft
                ? Coord(0, move.origin().rank() )
                : Coord(7, move.origin().rank() ));
        Coord rookTrgt = (move.type() == Move::CastleLeft
                ? Coord(3, move.origin().rank() )
                : Coord(5, move.origin().rank() ));
        Piece rook = squares[rookTrgt];
        rook.setMoved(false);
        setPiece(rookTrgt, Piece() );
        setPiece(rookOrig, rook);
    } else if (move.isPromotion() ) {
        piece = Piece(Piece::Pawn, piece.color(), true);
    }

    if (move.flags() & Move::FirstMoveFlag)
        piece.setMoved(false);

    setPiece(move.target(), piece_trgt);
    setPiece(move.origin(), piece);

    movesDone.pop_back();

    m_sideToMove = piece.color();
}

} // namespace ChessEngine

