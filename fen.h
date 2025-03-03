#pragma once

#include "board.h"

#include <sstream>

class FENParser {
public:
  Board parse_fen(const std::string &fen);
  std::string to_fen(const Board& board);

private:
  bool parse_blocks(const std::string &fen, std::string &board,
                    std::string &turn, std::string &castle,
                    std::string &en_passant, std::string &half_move,
                    std::string &full_move);

  bool parse_board(Board &board, const std::string &board_desc);

  bool parse_turn(Board &board, const std::string &turn);

  bool parse_castle(Board &board, const std::string &castle);

  bool parse_en_passant(Board &board, const std::string &en_passant);

  bool parse_move_count(Board &board, const std::string &half_move,
                        const std::string &full_move);

  void write_board(std::stringstream& ss, const Board& board);

  void write_turn(std::stringstream& ss, const Board& board);

  void write_castle(std::stringstream &ss, const Board &board);

  void write_en_passant(std::stringstream &ss, const Board &board);

  void write_move_count(std::stringstream &ss, const Board &board);
};
