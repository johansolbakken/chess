#include "engine.h"

#include <print>
#include <vector>
#include <map>
#include <algorithm>

std::string Engine::best_move(const Board &board) {
  std::vector<Move> possible;

  for (uint8_t rank = 0; rank < 8; rank++) {
    for (uint8_t file = 0; file < 8; file++) {
      Square from = {rank, file};
      const Piece &p = board.at(from);

      if (static_cast<PieceType>(p.type) == PieceType::None) {
        continue;
      }

      if (p.color != board.turn) {
        continue;
      }

      switch (static_cast<PieceType>(p.type)) {
          case PieceType::Pawn: {
              propose_pawn_moves(board, possible, from);
          }
      default:
        break;
      }
    }
  }

  for (auto& m : possible) {
      Board b = move(board, m.from, m.to);
      m.score = evaluate(b);
  }

  std::sort(possible.begin(), possible.end(), [](const auto& a, const auto& b){ return a.score > b.score; });

  for (const auto& m : possible) {
      std::println("{} -> {} : {}", to_string(m.from), to_string(m.to), m.score);
  }

  return to_string(possible.front().to);
}

bool Engine::legal_move(const Board &board, const Square &from,
                        const Square &to) {
  if (board.is_empty(to)) {
    return true;
  }

  if (board.at(from).color == board.at(to).color) {
    return false;
  }

  return true;
}

void Engine::propose_pawn_moves(const Board &board,
                                std::vector<Move> &moves,
                                const Square &from) {
  const auto &p = board.at(from);
  int8_t dy = static_cast<Color>(p.color) == Color::White ? 1 : -1;
  bool start_square =
      (static_cast<Color>(p.color) == Color::White) && (from.rank == 1) ||
      (static_cast<Color>(p.color) == Color::Black) && (from.rank == 6);

  if (from.file > 1) {
    Square move = {static_cast<uint8_t>(from.rank + dy),
                   static_cast<uint8_t>(from.file - 1)};
    if (!board.is_empty(move) && (board.at(move).color != p.color)) {
      if (legal_move(board, from, move)) {
        moves.push_back({from, move});
      }
    }
  }

  if (from.file < 7) {
    Square move = {static_cast<uint8_t>(from.rank + dy),
                   static_cast<uint8_t>(from.file + 1)};
    if (!board.is_empty(move) && (board.at(move).color != p.color)) {
      if (legal_move(board, from, move)) {
        moves.push_back({from, move});
      }
    }
  }

  if (start_square) {
    Square move = {static_cast<uint8_t>(from.rank + 2 * dy),
                   static_cast<uint8_t>(from.file)};
    if (board.is_empty({static_cast<uint8_t>(from.rank + dy), from.file}) &&
        legal_move(board, from, move)) {
      moves.push_back({from, move});
    }
  }

  {
    Square move = {static_cast<uint8_t>(from.rank + dy),
                   static_cast<uint8_t>(from.file)};
    if (legal_move(board, from, move)) {
      moves.push_back({from, move});
    }
  }
}


Board Engine::move(const Board& board, const Square& from, const Square& to) {
  Board b = board;
  b.at(to) = b.at(from);
  b.at(from).type = static_cast<uint8_t>(PieceType::None);
  return b;
}

double Engine::evaluate(const Board &board) {
  static const std::map<PieceType, double> base_value = {
      {PieceType::King, 1000.0}, {PieceType::Queen, 9.0},
      {PieceType::Rook, 5.0},    {PieceType::Bishop, 3.0},
      {PieceType::Knight, 3.0},  {PieceType::Pawn, 1.0},
  };
  double score = 0.0;
  for (uint8_t rank = 0; rank < 8; rank++) {
    for (uint8_t file = 0; file < 8; file++) {
      if (!board.is_empty({rank, file})) {
        double piece_value =
            base_value.at(static_cast<PieceType>(board.at({rank, file}).type));

        score += piece_value;
      }
    }
  }
  return score;
}
