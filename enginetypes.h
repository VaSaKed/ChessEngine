#ifndef ENGINETYPES_H
#define ENGINETYPES_H

#include <cstdint>
#include <array>
#include <vector>
#include <QDebug>

namespace Chess{

using int8  = std::int8_t;
using uint8 = std::uint8_t;
using real  = float;
template <typename T> using Vector = std::vector<T, std::allocator<T>>;
template <typename T, std::size_t N> using Array = std::array<T, N>;

class Coord {
    int8 ofst;
    static constexpr int8 INVALID = -63;

public:
    constexpr Coord()
        : ofst(INVALID) {}

    constexpr Coord(int8_t value)
        : ofst( value >= 64 || value < 0 ? INVALID : value) {}

    constexpr Coord(int8_t file, int8_t rank)
        : ofst( file >= 8 || file < 0 || rank >= 8 || rank < 0 ? INVALID : file+rank*8) {}

    constexpr bool isValid() const {
        return ofst >= 0 && ofst < 64;
    }

    constexpr int8 file() const {
        return ofst%8;
    }

    constexpr int8 rank() const {
        return ofst/8;
    }

    constexpr Coord nextFile() const {
        return (ofst%8 == 7) ? Coord() : Coord(ofst + 1);
    }

    constexpr Coord prevFile() const {
        return (ofst%8 == 0) ? Coord() : Coord(ofst - 1);
    }

    constexpr Coord nextRank() const {
        return (ofst/8 == 7) ? Coord() : Coord(ofst + 8);
    }

    constexpr Coord prevRank() const {
        return (ofst/8 == 0) ? Coord() : Coord(ofst - 8);
    }

    /* Bottom-Left to Top-Right */
    constexpr Coord nextDiagMain() const {
        return nextFile().isValid() && nextRank().isValid()
                ? nextFile().nextRank() : Coord();
    }

    constexpr Coord prevDiagMain() const {
        return prevFile().isValid() && prevRank().isValid()
                ? prevFile().prevRank() : Coord();
    }

    /* Bottom-Right to Top-Left */
    constexpr Coord nextDiagAnti() const {
        return prevFile().isValid() && nextRank().isValid()
                ? prevFile().nextRank() : Coord();
    }

    constexpr Coord prevDiagAnti() const {
        return nextFile().isValid() && prevRank().isValid()
                ? nextFile().prevRank() : Coord();
    }

    constexpr operator int8() const
        { return ofst; }

}; // !class Coord

class Piece {

    // |       flags Bit Meaning        |
    // | Type  | Moved | Unused | Color |
    // | 0 - 2 |   3   | 4 - 6  |   7   |
    uint8 flags;

public:

    static constexpr uint8 TypeOfst  = 0;
    static constexpr uint8 MovedOfst = 3;
    static constexpr uint8 ColorOfst = 7;

    static constexpr uint8 TypeMask  = (7 << TypeOfst);
    static constexpr uint8 MovedMask = (1 << MovedOfst);
    static constexpr uint8 ColorMask = (1 << ColorOfst);

    enum Type {
        Empty = 0,
        Pawn,
        Knight,
        Bishop,
        Rook,
        Queen,
        King
    };

    enum Color {
        White = 0,
        Black = 1
    };

    constexpr Piece()
        : flags(0) {}

    constexpr Piece(Type type, Color color, bool hasMooved = false)
        :flags( type | (color << ColorOfst) | hasMooved << MovedOfst ) {}

    constexpr bool isEmpty() const {
        return type() == Empty;
    }

    constexpr Type type() const {
        return Type( (flags & TypeMask) >> TypeOfst );
    }

    constexpr bool moved() const {
        return (flags & MovedMask);
    }

    constexpr Color color() const {
        return Color( (flags & ColorMask) >> ColorOfst);
    }

    inline void setType(Type type) {
        flags ^= (flags ^ (type << TypeOfst)) & TypeMask;
    }

    inline void setMoved(bool hasMoved) {
        flags ^= (flags ^ (hasMoved << MovedOfst)) & MovedMask;
    }

    inline void setColor(Color color) {
        flags ^= (flags ^ (color << ColorOfst)) & ColorMask;
    }

    constexpr operator uint8() const
        { return flags; }

}; // !class Piece

class Move {

    Coord orig;
    Coord trgt;
    Piece origPiece;
    Piece captPiece;
    uint8 moveType;

public:
    enum MoveType : uint8 {
        Standart        = 0,
        PromoteToQueen  = 1,
        PromoteToKnight = (1<<1),
        PromoteToRook   = (1<<2),
        PromoteToBishop = (1<<3),
        CastleRight     = (1<<4),
        CastleLeft      = (1<<5)
    };

    constexpr Move()
        : orig(), trgt(), origPiece(), captPiece(), moveType(Standart) {}

    constexpr Move(Coord origin, Coord target, Piece sourcePiece, Piece capturedPiece = Piece(), MoveType moveType = Standart)
        : orig(origin), trgt(target), origPiece(sourcePiece), captPiece(capturedPiece), moveType(moveType) {}

    constexpr bool isValid() const {
        return !origPiece.isEmpty() && orig.isValid() && trgt.isValid();
    }

    constexpr Coord origin() const{
        return orig;
    }

    constexpr Coord target() const{
        return trgt;
    }

    constexpr Piece originPiece() const {
        return origPiece;
    }

    constexpr Piece capturedPiece() const {
        return captPiece;
    }

    constexpr MoveType type() const {
        return MoveType(moveType);
    }

    constexpr bool isPromotion() const {
        return moveType == PromoteToQueen || moveType == PromoteToKnight
                || moveType == PromoteToRook || moveType == PromoteToBishop;
    }

    constexpr Piece promotedPiece() const {
        return moveType == PromoteToQueen ? Piece(Piece::Queen, origPiece.color())
            : moveType == PromoteToKnight ? Piece(Piece::Knight, origPiece.color())
            : moveType == PromoteToRook   ? Piece(Piece::Rook, origPiece.color())
            : moveType == PromoteToBishop ? Piece(Piece::Bishop, origPiece.color())
            : Piece();
    }

    constexpr bool isCastle() const {
        return moveType == CastleRight || moveType == CastleLeft;
    }

    constexpr bool sameVector(Move other) {
        return orig == other.orig && trgt == other.trgt;
    }

    constexpr bool operator==(Move other) {
        return orig == other.orig && trgt == other.trgt
                && origPiece == other.origPiece && captPiece == other.captPiece
                && moveType == other.moveType;
    }

}; // !class Move

} // !namesapce Chess

#endif // ENGINETYPES_H
