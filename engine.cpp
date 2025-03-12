#include "engine.h"

#include <limits>
#include <print>
#include <vector>
#include <bit>

#include "board.h"
#include "util.h"
#include "horse.h"

Engine::Move Engine::best_move(const Board &board, int depth) {
  Move best_move = {};
  double best_score = -std::numeric_limits<double>::infinity();
  double alpha = -std::numeric_limits<double>::infinity();
  double beta = std::numeric_limits<double>::infinity();

  std::vector<Move> moves;
  moves.reserve(64);
  generate_moves(board, moves);

  for (const auto& m : moves) {
    Board b = make_move(board, m);

    double score = alpha_beta(b, depth - 1, b.turn == Color::White, alpha, beta);

    if (score > best_score) {
      best_score = score;
      best_move = m;
    }

    alpha = std::max(alpha, best_score);
  }

  return best_move;
}



double Engine::alpha_beta(const Board& board, int depth, bool white, double alpha, double beta) {
  if (depth == 0 || board.game_over) {
    return evaluate(board);
  }

  std::vector<Move> moves;
  moves.reserve(64);
  generate_moves(board, moves);

  if (moves.empty()) {
    return evaluate(board);
  }

  if (white) {
    double best_score = -std::numeric_limits<double>::infinity();

    for (const auto &m : moves) {
      Board b = make_move(board, m);
      double score = alpha_beta(b, depth-1, false, alpha, beta);
      best_score = std::max(best_score, score);
      alpha = std::max(alpha, best_score);

      if (beta <= alpha) {
        break;
      }
    }

    return best_score;
  } else {
    double best_score = std::numeric_limits<double>::infinity();

    for (const auto &m : moves) {
      Board b = make_move(board, m);
      double score = alpha_beta(b, depth-1, true, alpha, beta);
      best_score = std::min(best_score, score);
      beta = std::min(beta, best_score);
      if (beta <= alpha) {
        break;
      }
    }

    return best_score;
  }
}

double Engine::evaluate(const Board &board) {
  if (board.game_over) {
    switch (board.result) {
      case Result::WhiteWins:
        return 1E10;
      case Result::BlackWins:
        return -1E10;
      case Result::Stalemate:
      case Result::Draw:
        return 0;
    }
  }

  double score = 0.0;

  score += std::popcount(board.white_pawns)   * 1.0;
  score += std::popcount(board.white_knights) * 3.0;
  score += std::popcount(board.white_bishops) * 3.0;
  score += std::popcount(board.white_rooks)   * 5.0;
  score += std::popcount(board.white_queens)  * 9.0;
  score += std::popcount(board.white_kings)   * 1E4;

  score -= std::popcount(board.black_pawns)   * 1.0;
  score -= std::popcount(board.black_knights) * 3.0;
  score -= std::popcount(board.black_bishops) * 3.0;
  score -= std::popcount(board.black_rooks)   * 5.0;
  score -= std::popcount(board.black_queens)  * 9.0;
  score -= std::popcount(board.black_kings)   * 1E4;

  return score;
}

void Engine::generate_moves(const Board &board, std::vector<Move> &moves) {
  for (uint8_t rank = 0; rank < 8; rank++) {
    for (uint8_t file = 0; file < 8; file++) {
      uint64_t pos = (1ULL << (rank * 8 + file));

      if (board.turn == Color::White) {
        if (board.white_pawns & pos) {
          propose_pawn_moves(board, moves, {rank, file});
        } else if (board.white_knights & pos) {
          propose_knight_moves(board, moves, {rank, file});
        } else if (board.white_bishops & pos) {
          propose_bishop_moves(board, moves, {rank, file});
        } else if (board.white_rooks & pos) {
          propose_rook_moves(board, moves, {rank, file});
        } else if (board.white_queens & pos) {
          propose_queen_moves(board, moves, {rank, file});
        } else if (board.white_kings & pos) {
          propose_king_moves(board, moves, {rank, file});
        }
      } else {
        if (board.black_pawns & pos) {
          propose_pawn_moves(board, moves, {rank, file});
        } else if (board.black_knights & pos) {
          propose_knight_moves(board, moves, {rank, file});
        } else if (board.black_bishops & pos) {
          propose_bishop_moves(board, moves, {rank, file});
        } else if (board.black_rooks & pos) {
          propose_rook_moves(board, moves, {rank, file});
        } else if (board.black_queens & pos) {
          propose_queen_moves(board, moves, {rank, file});
        } else if (board.black_kings & pos) {
          propose_king_moves(board, moves, {rank, file});
        }
      }
    }
  }
}

void Engine::propose_pawn_moves(const Board &board, std::vector<Move> &moves,
                                const Square &from) {
  uint64_t from_pos = (1ULL << (from.rank * 8 + from.file));
  bool white = board.turn == Color::White;

  int dy = white ? 1 : -1;
  bool start_square = ((white && (from.rank == 1)) || (!white && (from.rank == 6)));

  // Propose attacking moves
  {
    // Attack right
    {
      int rank = from.rank + dy;
      int file = from.file + 1;
      int pos = (1ULL << (rank * 8 + file));
      if (util::within_bounds(rank, file) && (board.occupied_squares & pos)) {
        if ((white && !(board.white_pieces & pos)) ||
            (!white && !(board.black_pieces & pos))) {
          moves.push_back({from, {rank, file}});
        }
      }
    }

    // Attack left
    {
      int rank = from.rank + dy;
      int file = from.file - 1;
      int pos = (1ULL << (rank * 8 + file));
      if (util::within_bounds(rank, file) && (board.occupied_squares & pos)) {
        if ((white && !(board.white_pieces & pos)) ||
            (!white && !(board.black_pieces & pos))) {
          moves.push_back({from, {rank, file}});
        }
      }
    }
  }

  {
    int rank = from.rank + dy;
    int file = from.file;

    if (!util::within_bounds(rank, file)) {
      return;
    }

    uint64_t pos = (1ULL << (rank * 8 + file));

    if (!(board.occupied_squares & pos)) {
      moves.push_back({from, {rank, file}});
    }

    if (!start_square) {
      return;
    }

    rank += dy; // if on start pos, move one extra forward for pawn.
    pos = (1ULL << (rank * 8 + file));

    if (!(board.occupied_squares & pos)) {
      moves.push_back({from, {rank, file}});
    }
  }
}

void Engine::propose_knight_moves(const Board &board, std::vector<Move> &moves,
                                  const Square &from) {
  uint64_t fromPos = 1ULL << (from.rank * 8 + from.file);
  bool white = (board.white_knights & fromPos) != 0;

  for (auto [r, f] : KnightMoveTable::get(from.rank, from.file)) {
    uint64_t toPos = 1ULL << (r * 8 + f);

    // If same color piece is there, can’t move
    if (white && (board.white_pieces & toPos)) {
      continue;
    }
    if (!white && (board.black_pieces & toPos)) {
      continue;
    }

    // Otherwise, it’s either empty or an enemy piece, so valid
    moves.push_back({from, {r, f}});
  }
}

void Engine::propose_king_moves(const Board &board, std::vector<Move> &moves,
                                const Square &from) {
  uint64_t kingPos = 1ULL << (from.rank * 8 + from.file);
  bool white = (board.white_kings & kingPos) != 0;

  for (int dy = -1; dy <= 1; dy++) {
    for (int dx = -1; dx <= 1; dx++) {
      if (dy == 0 && dx == 0) {
        continue;
      }

      int rank = from.rank + dy;
      int file = from.file + dx;

      if (!util::within_bounds(rank, file)) {
        continue;
      }

      Square move = {rank, file};
      uint64_t move_pos = (1ULL << (rank * 8 + file));

      if (white) {
        if (board.white_pieces & move_pos) {
          continue;
        }
      } else {
        if (board.black_pieces & move_pos) {
          continue;
        }
      }

      moves.push_back({from, move});
    }
  }
}

void Engine::propose_rook_moves(const Board &board, std::vector<Move> &moves,
                                const Square &from) {

  // Lambda to be used by functions that propose moves.
  // returns true if to break out of for loops.
  auto propose_rook_move = [&](int rank, int file) {
    if (!util::within_bounds(rank, file)) {
      // rook moves outisde of the board, return true
      return true;
    }

    Square move = {static_cast<uint8_t>(rank), static_cast<uint8_t>(file)};
    uint64_t move_pos = (1ULL << (rank * 8 + file));
    uint64_t from_pos = (1ULL << (from.rank * 8 + from.file));
    bool white = board.white_rooks & from_pos;

    if (board.occupied_squares & move_pos) {
      if ((white && (board.black_pieces & move_pos)) ||
          (!white && (board.white_pieces & move_pos))) {
        moves.push_back({from, move});
      }

      // square was not empty, return true
      return true;
    }

    moves.push_back({from, move});

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

void Engine::propose_bishop_moves(const Board &board, std::vector<Move> &moves,
                                  const Square &from) {
  // Lambda to be used by functions that propose moves.
  // returns true if to break out of for loops.
  auto propose_bishop_move = [&](int rank, int file) {
    if (!util::within_bounds(rank, file)) {
      // bishop moves outisde of the board, return true
      return true;
    }

    Square move = {static_cast<uint8_t>(rank), static_cast<uint8_t>(file)};
    uint64_t move_pos = (1ULL << (rank * 8 + file));
    uint64_t from_pos = (1ULL << (from.rank * 8 + from.file));
    bool white = board.white_bishops & from_pos;

    if (board.occupied_squares & move_pos) {
      if ((white && (board.black_pieces & move_pos)) ||
          (!white && (board.white_pieces & move_pos))) {
        moves.push_back({from, move});
      }

      // square was not empty, return true
      return true;
    }

    moves.push_back({from, move});

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

void Engine::propose_queen_moves(const Board &board, std::vector<Move> &moves,
                                 const Square &from) {
  propose_bishop_moves(board, moves, from);
  propose_rook_moves(board, moves, from);
}

Board Engine::make_move(const Board &board, const Move &move) {
  Board b = board;
  uint64_t from = (1ULL << (move.from.rank * 8 + move.from.file));
  uint64_t to = (1ULL << (move.to.rank * 8 + move.to.file));

  if (board.white_pawns & from) {
    b.white_pawns = (board.white_pawns & ~from) | to;
  } else if (board.white_knights & from) {
    b.white_knights = (board.white_knights & ~from) | to;
  } else if (board.white_bishops & from) {
    b.white_bishops = (board.white_bishops & ~from) | to;
  } else if (board.white_rooks & from) {
    b.white_rooks = (board.white_rooks & ~from) | to;
  } else if (board.white_queens & from) {
    b.white_queens = (board.white_queens & ~from) | to;
  } else if (board.white_kings & from) {
    b.white_kings = (board.white_kings & ~from) | to;
  } else if (board.black_pawns & from) {
    b.black_pawns = (board.black_pawns & ~from) | to;
  } else if (board.black_knights & from) {
    b.black_knights = (board.black_knights & ~from) | to;
  } else if (board.black_bishops & from) {
    b.black_bishops = (board.black_bishops & ~from) | to;
  } else if (board.black_rooks & from) {
    b.black_rooks = (board.black_rooks & ~from) | to;
  } else if (board.black_queens & from) {
    b.black_queens = (board.black_queens & ~from) | to;
  } else if (board.black_kings & from) {
    b.black_kings = (board.black_kings & ~from) | to;
  }

  if (board.turn == Color::White) {
    b.turn = Color::Black;
  } else if (board.turn == Color::Black) {
    b.turn = Color::White;
  }

  b.aggregate();

  if (checkmate(b)) {
    b.game_over = true;
    b.result = (b.turn == Color::White) ? Result::BlackWins : Result::WhiteWins;
  }

  return b;
}


bool Engine::checkmate(const Board &board) {
  // TODO: Fix this file
  for (int rank = 0; rank < 8; rank++) {
    for (int file = 0; file < 8; file++) {
      uint64_t pos = (1ULL << (rank * 8 + file));
      if (board.turn == Color::White && (board.white_kings & pos)) {
        return false;
      } else if (board.turn == Color::Black && (board.black_kings & pos)) {
        return false;
      }
    }
  }

  return false;
}
