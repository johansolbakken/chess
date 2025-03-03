#include "engine.h"

#include <limits>
#include <print>
#include <vector>
#include <map>

Engine::Move Engine::best_move(const Board &board, int depth) {
  Move best_move = {};
  double best_score = -std::numeric_limits<double>::infinity();

  std::vector<Move> moves;
  generate_moves(board, moves);

  for (const auto& m : moves) {
      Board b = make_move(board, m);

      double score = minimax(b, depth, static_cast<Color>(board.turn) == Color::White);
      if (score > best_score) {
        best_score = score;
        best_move.to = m.to;
        best_move.from = m.from;
      }
  }

  return best_move;
}


double Engine::minimax(const Board& board, int depth, bool white) {
  if (depth == 0) {
    return evaluate(board);
  }

  std::vector<Move> moves;
  generate_moves(board, moves);

  if (moves.empty()) {
    return evaluate(board);
  }

  if (white) {
    double best_score = -std::numeric_limits<double>::infinity();

    for (const auto &m : moves) {
      Board b = make_move(board, m);
      double score = minimax(b, depth-1, false);
      best_score = std::max(best_score, score);
    }

    return best_score;
  } else {
    double best_score = std::numeric_limits<double>::infinity();

    for (const auto &m : moves) {
      Board b = make_move(board, m);
      double score = minimax(b, depth-1, true);
      best_score = std::min(best_score, score);
    }

    return best_score;

  }
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

        // Distance to center
        std::vector<double> d = {0.8, 0.9, 1.0, 1.5, 1.5, 1.0, 0.9, 0.8};
        piece_value *= d[rank];
        piece_value *= d[file];

        if (board.at({rank, file}).color != board.turn) {
          piece_value *= -1.0;
        }

        score += piece_value;
      }
    }
  }
  return score;
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

bool Engine::within_bounds(int rank, int file) {
  return (rank >= 0 && rank <= 7 && file >= 0 && file <= 7);
}

void Engine::generate_moves(const Board &board, std::vector<Move> &moves) {
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
      case PieceType::Pawn:
        propose_pawn_moves(board, moves, from);
        break;
      case PieceType::Knight:
        propose_knight_moves(board, moves, from);
        break;
      case PieceType::Rook:
        propose_rook_moves(board, moves, from);
        break;
      case PieceType::King:
        propose_king_moves(board, moves, from);
        break;
      case PieceType::Bishop:
        propose_bishop_moves(board, moves, from);
        break;
      case PieceType::Queen:
        propose_queen_moves(board, moves, from);
        break;
      default:
        break;
      }
    }
  }
}

  void Engine::propose_pawn_moves(const Board &board, std::vector<Move> &moves,
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

  void Engine::propose_knight_moves(
      const Board &board, std::vector<Move> &moves, const Square &from) {
    std::vector<std::pair<int, int>> Ls = {
        {2, 1}, {2, -1}, {1, -2}, {1, 2}, {-1, -2}, {-1, 2}, {-2, 1}, {-2, -1}};

    for (const auto &l : Ls) {
      int rank = from.rank + l.first;
      int file = from.file + l.second;

      if (rank > 7 || rank < 0 || file > 7 || file < 0) {
        continue;
    }

    Square move = {static_cast<uint8_t>(rank),
                   static_cast<uint8_t>(file)};

    if (legal_move(board, from, move)) {
      moves.push_back({from, move});
    }
  }
}

void Engine::propose_king_moves(const Board &board, std::vector<Move> &moves,
                                const Square &from) {
  for (int dy = -1; dy <= 1; dy++) {
    for (int dx = -1; dx <= 1; dx++) {
      if (dy == 0 && dx == 0) {
        continue;
      }

      int rank = from.rank + dy;
      int file = from.file + dx;

      if (rank < 0 || rank > 7 || file < 0 || file > 7) {
        continue;
      }

      Square move = {static_cast<uint8_t>(rank), static_cast<uint8_t>(file)};

      if (legal_move(board, from, move)) {
        moves.push_back({from, move});
      }
    }
  }
}

void Engine::propose_rook_moves(const Board &board, std::vector<Move> &moves,
                                const Square &from) {

  // Lambda to be used by functions that propose moves.
  // returns true if to break out of for loops.
  auto propose_rook_move = [&](int rank, int file) {
    if (!within_bounds(rank, file)) {
      // rook moves outisde of the board, return true
      return true;
    }

    Square move = {static_cast<uint8_t>(rank), static_cast<uint8_t>(file)};
    if (!board.is_empty(move)) {
      if (board.at(move).color != board.turn) {
        if (legal_move(board, from, move)) {
          moves.push_back({from, move});
        }
      }

      // square was not empty, return true
      return true;
    }

    if (legal_move(board, from, move)) {
      moves.push_back({from, move});
    }

    return false;
  };

  for (int left = 1; left < 7; left++) {
    int rank = from.rank;
    int file = from.file - left;

    if (propose_rook_move(rank, file)) {
      break;
    }
  }

  for (int right = 1; right < 7; right++) {
    int rank = from.rank;
    int file = from.file + right;

    if (propose_rook_move(rank, file)) {
      break;
    }
  }

  for (int down = 1; down < 7; down++) {
    int rank = from.rank - down;
    int file = from.file;

    if (propose_rook_move(rank, file)) {
      break;
    }
  }

  for (int up = 1; up < 7; up++) {
    int rank = from.rank + up;
    int file = from.file;

    if (propose_rook_move(rank, file)) {
      break;
    }
  }
}


void Engine::propose_bishop_moves(const Board& board, std::vector<Move>& moves, const Square& from) {
  // Lambda to be used by functions that propose moves.
  // returns true if to break out of for loops.
  auto propose_bishop_move = [&](int rank, int file) {
    if (!within_bounds(rank, file)) {
      // bishop moves outisde of the board, return true
      return true;
    }

    Square move = {static_cast<uint8_t>(rank), static_cast<uint8_t>(file)};
    if (!board.is_empty(move)) {
      if (board.at(move).color != board.turn) {
        if (legal_move(board, from, move)) {
          moves.push_back({from, move});
        }
      }

      // square was not empty, return true
      return true;
    }

    if (legal_move(board, from, move)) {
      moves.push_back({from, move});
    }

    return false;
  };

  for (int up_left = 1; up_left < 7; up_left++) {
    int rank = from.rank + up_left;
    int file = from.file - up_left;

    if (propose_bishop_move(rank, file)) {
      break;
    }
  }

  for (int up_right = 1; up_right < 7; up_right++) {
    int rank = from.rank + up_right;
    int file = from.file + up_right;

    if (propose_bishop_move(rank, file)) {
      break;
    }
  }

  for (int down_left = 1; down_left < 7; down_left++) {
    int rank = from.rank - down_left;
    int file = from.file - down_left;

    if (propose_bishop_move(rank, file)) {
      break;
    }
  }

  for (int down_right = 1; down_right < 7; down_right++) {
    int rank = from.rank - down_right;
    int file = from.file + down_right;

    if (propose_bishop_move(rank, file)) {
      break;
    }
  }
}


void Engine::propose_queen_moves(const Board& board, std::vector<Move>& moves, const Square& from) {
 propose_bishop_moves(board, moves, from);
 propose_rook_moves(board, moves, from);
}

Board Engine::make_move(const Board& board, const Move& move) {
  Board b = board;
  b.at(move.to) = b.at(move.from);
  b.at(move.from).type = static_cast<uint8_t>(PieceType::None);
  b.turn = !b.turn;
  return b;
}
