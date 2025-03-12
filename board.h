#pragma once

#include <cstdint>
#include <string>
#include <format>

enum class Color : uint8_t {
  White=0,
  Black=1
};

struct Square {
  uint8_t rank : 3;
  uint8_t file : 3;

  Square() = default;

  template <std::integral T, std::integral U>
  Square(T r, U f)
      : rank(static_cast<uint8_t>(r)), file(static_cast<uint8_t>(f)) {
  }
};

inline std::string to_string(const Square &square) {
  return std::format("{}{}",
    static_cast<char>(square.file + 'a'),
    static_cast<char>(square.rank + '1'));
}

enum class Result {
  WhiteWins,
  BlackWins,
  Stalemate,
  Draw,
};

struct Board {
  bool game_over = false;
  Result result = Result::Draw;

  uint64_t white_pawns = 0;
  uint64_t white_queens = 0;
  uint64_t white_kings = 0;
  uint64_t white_knights = 0;
  uint64_t white_bishops = 0;
  uint64_t white_rooks = 0;

  uint64_t black_pawns = 0;
  uint64_t black_queens = 0;
  uint64_t black_kings = 0;
  uint64_t black_knights = 0;
  uint64_t black_bishops = 0;
  uint64_t black_rooks = 0;

  Color turn = Color::White;

  bool castle_white_kingside = false;
  bool castle_white_queenside = false;
  bool castle_black_kingside = false;
  bool castle_black_queenside = false;

  bool has_en_passant = false;
  uint8_t en_passant_file = 0;
  uint8_t en_passant_rank = 0;

  uint8_t half_move = 0;
  uint8_t full_move = 1;

  uint64_t white_pieces = 0;
  uint64_t black_pieces = 0;
  uint64_t occupied_squares = 0;
  uint64_t empty_squares = 0;

  void aggregate() {
    white_pieces = 0;
    black_pieces = 0;
    occupied_squares = 0;
    empty_squares = 0;

    white_pieces = white_pawns | white_knights | white_bishops | white_rooks |
                   white_queens | white_kings;
    black_pieces = black_pawns | black_knights | black_bishops | black_rooks |
                   black_queens | black_kings;
    occupied_squares = white_pieces | black_pieces;
    empty_squares = ~white_pieces & ~black_pieces;
  }
};
