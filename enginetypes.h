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

    Coord src;
    Coord trgt;
    Piece srcPiece;
    Piece trgtPiece;
    // store the promoted-to piece in the move class
    // probably a bad solution, but easy to implement
    Piece m_promotedPiece; // !!! sizeof(Move) > 32; member is very rarely used

public:

    constexpr Move()
        : src(), trgt(), srcPiece(), trgtPiece(), m_promotedPiece() {}

    constexpr Move(Coord source, Coord target, Piece sourcePiece, Piece targetPiece = Piece(), Piece promotedPiece = Piece())
        : src(source), trgt(target), srcPiece(sourcePiece), trgtPiece(targetPiece), m_promotedPiece(promotedPiece) {}

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

    constexpr Piece promotedPiece() const {
        return m_promotedPiece;
    }

    constexpr bool isPromotion() const {
        return !m_promotedPiece.isEmpty();
    }

    constexpr bool operator==(Move other) {
        return src == other.src && trgt == other.trgt
                && srcPiece == other.srcPiece && trgtPiece == other.trgtPiece
                && m_promotedPiece == other.m_promotedPiece;
    }

}; // !class Move

} // !namesapce Chess

#endif // ENGINETYPES_H
