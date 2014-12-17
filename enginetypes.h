#ifndef ENGINETYPES_H
#define ENGINETYPES_H

#include <cstdint>
#include <array>
#include <vector>
#include <QDebug>

namespace Chess{

using int8  = std::int8_t;
using uint8 = std::uint8_t;
template <typename T> using Vector = std::vector<T, std::allocator<T>>;

class Coord {
    int8 ofst;
    static constexpr int8 INVALID = -63;

public:
    constexpr Coord()
        : ofst(INVALID) {}

    constexpr Coord(int8_t value)
        : ofst( value >= 64 || value < 0 ? INVALID : value) {}

    constexpr Coord(int8_t rank, int8_t file)
        : ofst( rank >= 8 || rank < 0 || file >= 8 || file < 0 ? INVALID : rank+file*8) {}

    constexpr bool isValid() const {
        return ofst >= 0 && ofst < 64;
    }

    constexpr int8 rank() const {
        return ofst%8;
    }

    constexpr int8 file() const {
        return ofst/8;
    }

    constexpr Coord nextRank() const {
        return (ofst%8 == 7) ? Coord() : Coord(ofst + 1);
    }

    constexpr Coord prevRank() const {
        return (ofst%8 == 0) ? Coord() : Coord(ofst - 1);
    }

    constexpr Coord nextFile() const {
        return (ofst/8 == 7) ? Coord() : Coord(ofst + 8);
    }

    constexpr Coord prevFile() const {
        return (ofst/8 == 0) ? Coord() : Coord(ofst - 8);
    }

    /* Bottom-Left to Top-Right */
    constexpr Coord nextDiagMain() const {
        return nextRank().isValid() && nextFile().isValid()
                ? nextRank().nextFile() : Coord();
    }

    constexpr Coord prevDiagMain() const {
        return prevRank().isValid() && prevFile().isValid()
                ? prevRank().prevFile() : Coord();
    }

    /* Bottom-Right to Top-Left */
    constexpr Coord nextDiagAnti() const {
        return prevRank().isValid() && nextFile().isValid()
                ? prevRank().nextFile() : Coord();
    }

    constexpr Coord prevDiagAnti() const {
        return nextRank().isValid() && prevFile().isValid()
                ? nextRank().prevFile() : Coord();
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

    Coord src;
    Coord trgt;
    Piece srcPiece;
    Piece trgtPiece;

public:

    constexpr Move()
        : src(), trgt(), srcPiece(), trgtPiece() {}

    constexpr Move(Coord source, Coord target, Piece sourcePiece, Piece targetPiece = Piece())
        : src(source), trgt(target), srcPiece(sourcePiece), trgtPiece(targetPiece) {}

    constexpr bool isValid() const {
        return src.isValid() && trgt.isValid() && !srcPiece.isEmpty();
    }

    constexpr Coord source() const{
        return src;
    }

    constexpr Coord target() const{
        return trgt;
    }

    constexpr Piece sourcePiece() const {
        return srcPiece;
    }

    constexpr Piece targetPiece() const {
        return trgtPiece;
    }

    constexpr bool operator==(Move other) {
        return src == other.src && trgt == other.trgt
                && srcPiece == other.srcPiece && trgtPiece == other.trgtPiece;
    }

}; // !class Move

} // !namesapce Chess

#endif // ENGINETYPES_H
