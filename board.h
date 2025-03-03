#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <format>

enum class Color {
  White=0,
  Black=1
};

enum class PieceType {
  None=0,
  King=1,
  Queen=2,
  Rook=3,
  Bishop=4,
  Knight=5,
  Pawn=6
};

struct Piece {
  uint8_t type : 3;
  uint8_t color : 1;
};

struct Square {
  uint8_t rank : 3;
  uint8_t file : 3;
};

inline std::string to_string(const Square& square) {
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

  const Piece& at(const Square& sq) const {
    return pieces[sq.rank*8+sq.file];
  }

  Piece& at(const Square& sq) {
    return pieces[sq.rank*8+sq.file];
  }

  bool is_empty(const Square& sq) const {
    return static_cast<PieceType>(at(sq).type) == PieceType::None;
  }
};
