#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <format>
#include <cassert>

enum class Color : uint8_t {
  White=0,
  Black=1
};

enum class PieceType : uint8_t {
  None=0,
  King=1,
  Queen=2,
  Rook=3,
  Bishop=4,
  Knight=5,
  Pawn=6
};

inline int compare(PieceType a, PieceType b) {
  return static_cast<int>(a) - static_cast<int>(b);
}

struct Piece {
  uint8_t _type : 3;
  uint8_t _color : 1;

  inline constexpr Color color() const { return static_cast<Color>(_color); }
  inline constexpr PieceType type() const { return static_cast<PieceType>(_type); }
};

struct Square {
  uint8_t rank : 3;
  uint8_t file : 3;

  Square() = default;

  template <std::integral T, std::integral U>
  Square(T r, U f)
      : rank(static_cast<uint8_t>(r)), file(static_cast<uint8_t>(f)) {
    assert(util::within_bounds(r, f) && "Square coordinates must be between 0 and 7");
  }
};

inline std::string to_string(const Square &square) {
  return std::format("{}{}",
    static_cast<char>(square.file + 'a'),
    static_cast<char>(square.rank + '1'));
}

struct Board {
  std::array<Piece,64> pieces = {};

  uint8_t turn : 1 = 0;

  uint8_t castle_white_kingside  : 1 = 0;
  uint8_t castle_white_queenside : 1 = 0;
  uint8_t castle_black_kingside  : 1 = 0;
  uint8_t castle_black_queenside : 1 = 0;

  uint8_t has_en_passant  : 1 = 0;
  uint8_t en_passant_file : 3 = 0;
  uint8_t en_passant_rank : 3 = 0;

  uint8_t half_move : 8 = 0;
  uint8_t full_move : 8 = 1;

  inline constexpr const Piece& at(uint8_t rank, uint8_t file) const {
    return pieces[rank*8+file];
  }

  inline constexpr Piece& at(const Square& sq) {
    return pieces[sq.rank*8+sq.file];
  }

  inline bool is_empty(uint8_t rank, uint8_t file) const {
    return at(rank, file).type()== PieceType::None;
  }
};
