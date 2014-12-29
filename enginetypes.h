#ifndef ENGINETYPES_H
#define ENGINETYPES_H

#include <array>
#include <vector>
#include <cstdint>
#include <algorithm>

#include <QDebug>

namespace Chess{

using sint8  = std::int8_t;
using uint8  = std::uint8_t;
using sint16 = std::int16_t;
using uint16 = std::uint16_t;
using real   = float;

template <typename T> using Vector = std::vector<T, std::allocator<T>>;
template <typename T, std::size_t N> using Array = std::array<T, N>;

enum Direction {
    North       = 0,
    NorthEast,
    East,
    SouthEast,
    South,
    SouthWest,
    West,
    NorthWest,
    //---------------//
    DirectionNumber,
    Rotate_90_Degree = 2
};

class Coord {
    sint8 ofst;
    static constexpr sint8 INVALID = -63;

public:
    constexpr Coord()
        : ofst(INVALID) {}

    constexpr Coord(sint8 value)
        : ofst( value >= 64 || value < 0 ? INVALID : value) {}

    constexpr Coord(sint8 file, sint8 rank)
        : ofst( file >= 8 || file < 0 || rank >= 8 || rank < 0 ? INVALID : file+rank*8) {}

    constexpr bool isValid() const {
        return ofst >= 0 && ofst < 64;
    }

    constexpr sint8 file() const {
        return ofst%8;
    }

    constexpr sint8 rank() const {
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

    constexpr bool sameFile(Coord other) const {
        return file() == other.file();
    }

    constexpr bool sameRank(Coord other) const {
        return rank() == other.rank();
    }

    inline uint8 chebyshevDistance(Coord other) const {
        return std::max( std::abs( other.file() - file() ), std::abs( other.rank() - rank() ) );
    }

    constexpr operator sint8() const
        { return ofst; }

}; // !class Coord

class Piece {

    uint8 flags;
    // | Piece flags bits meaning :     |
    // | ------------------------------ |
    // | Type  | Moved | Unused | Color |
    // | 0 - 2 |   3   | 4 - 6  |   7   |


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

    friend Color operator!(Color self);

}; // !class Piece

// is handy
inline Piece::Color operator!(Piece::Color self) {return Piece::Color(self == Piece::White ? Piece::Black : Piece::White);}

class Move {
    Coord orig;
    Coord trgt;
    uint8 moveFlags;
    // | moveFlags bits meaning:                                   |
    // | --------------------------------------------------------- |
    // | Capture |  FirstMove | SpecialPawn | SpecialKing | Unused |
    // |     0   |      1     |      2      |      3      |   4-7  |

    uint8 pieceSpecific;
    // holds piece specific info
    //  - castling for kings and promotion for pawns

public:

    enum MoveFlags {
        NoFlags           = 0,
        CaptureFlag       = (1<<0),
        FirstMoveFlag     = (1<<1),
        PawnMoveFlag      = (1<<2),
        KingMoveFlag      = (1<<3)
    };

    enum SpecialMove {
        NotSpecial      = 0,
        // Pawn Special
        DoublePush      = (1<<0),
        EnPassant       = (1<<1),
        PromoteToQueen  = (1<<2),
        PromoteToKnight = (1<<3),
        PromoteToRook   = (1<<4),
        PromoteToBishop = (1<<5),
        // King Special
        CastleLeft      = (1<<0),
        CastleRight     = (1<<1)
    };

    constexpr Move()
        : orig(), trgt(), moveFlags(0), pieceSpecific(0) {}

    constexpr Move(Coord origin, Coord target, bool isFirstMove = false, bool isCapture = false)
        : orig(origin), trgt(target), moveFlags( isFirstMove << 1 | isCapture << 0), pieceSpecific(0) {}

    constexpr Move(Coord origin, Coord target, MoveFlags flags, SpecialMove special)
        : orig(origin), trgt(target), moveFlags(flags), pieceSpecific(special) {}

    bool constexpr isValid() const {
        return orig.isValid() && trgt.isValid();
    }

    constexpr Coord origin() const{
        return orig;
    }

    constexpr Coord target() const{
        return trgt;
    }

    constexpr MoveFlags flags() const {
        return MoveFlags(moveFlags);
    }

    constexpr bool isPromotion() const {
        return (moveFlags & PawnMoveFlag)
                && (pieceSpecific == PromoteToQueen
                    || pieceSpecific == PromoteToKnight
                    || pieceSpecific == PromoteToRook
                    || pieceSpecific == PromoteToBishop);
    }

    constexpr bool isCastle() const {
        return (moveFlags & PawnMoveFlag)
                && (pieceSpecific == CastleRight
                    || pieceSpecific == CastleLeft);
    }

    constexpr bool sameVector(Move other) const  {
        return orig == other.orig && trgt == other.trgt;
    }

    constexpr bool operator==(Move other) const {
        return orig == other.orig && trgt == other.trgt && moveFlags == other.moveFlags && pieceSpecific == other.pieceSpecific;
    }
}; // !class Move

} // !namesapce Chess

#endif // ENGINETYPES_H
