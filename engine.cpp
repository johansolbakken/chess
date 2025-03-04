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

  std::vector<Move> moves;
  moves.reserve(20);
  generate_moves(board, moves);

  for (const auto& m : moves) {
      Board b = make_move(board, m);

      double score = alpha_beta(
          /* board */ b,
          /* depth */ depth,
          /* white */ static_cast<Color>(board.turn) == Color::White,
          /* alpha */ -std::numeric_limits<double>::infinity(),
          /* beta */ std::numeric_limits<double>::infinity());
      if (score > best_score) {
        best_score = score;
        best_move.to = m.to;
        best_move.from = m.from;
      }
  }

  return best_move;
}


double Engine::alpha_beta(const Board& board, int depth, bool white, double alpha, double beta) {
  if (depth == 0 || board.game_over) {
    return evaluate(board);
  }

  std::vector<Move> moves;
  moves.reserve(20);
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

  static constexpr std::array<double, 7> base_value = {/* None   */ 0.0,
                                                       /* King   */ 1000.0,
                                                       /* Queen  */ 9.0,
                                                       /* Rook   */ 5.0,
                                                       /* Bishop */ 3.0,
                                                       /* Knight */ 3.0,
                                                       /* Pawn   */ 1.0};

  static const double knight_table[8][8] = {
      {-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0},
      {-4.0, -2.0, 0.0, 0.5, 0.5, 0.0, -2.0, -4.0},
      {-3.0, 0.5, 1.0, 1.5, 1.5, 1.0, 0.5, -3.0},
      {-3.0, 0.0, 1.5, 2.0, 2.0, 1.5, 0.0, -3.0},
      {-3.0, 0.5, 1.5, 2.0, 2.0, 1.5, 0.5, -3.0},
      {-3.0, 0.0, 1.0, 1.5, 1.5, 1.0, 0.0, -3.0},
      {-4.0, -2.0, 0.0, 0.0, 0.0, 0.0, -2.0, -4.0},
      {-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0}};

  static const double pawn_table[8][8] = {
      {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
      {5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0},
      {1.0, 1.0, 2.0, 3.0, 3.0, 2.0, 1.0, 1.0},
      {0.5, 0.5, 1.0, 2.5, 2.5, 1.0, 0.5, 0.5},
      {0.0, 0.0, 0.0, 2.0, 2.0, 0.0, 0.0, 0.0},
      {0.5, -0.5, -1.0, 0.0, 0.0, -1.0, -0.5, 0.5},
      {0.5, 1.0, 1.0, -2.0, -2.0, 1.0, 1.0, 0.5},
      {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
  };

  double score = 0.0;

  score += std::popcount(board.white_control);
  score -= std::popcount(board.black_control);

  for (uint8_t rank = 0; rank < 8; rank++) {
    for (uint8_t file = 0; file < 8; file++) {
      const auto& p = board.pieces[rank*8 + file];
      bool white = p.color() == Color::White;
      PieceType type = p.type();

      if (type == PieceType::None) {
        continue;
      }

      double piece_value = base_value[p._type];

      // Subtract enemy pieces
      if (p._color != board.turn) {
        piece_value *= -1.0;
      }

      score += piece_value;
    }
  }

  return score;
}

void Engine::generate_moves(const Board &board, std::vector<Move> &moves) {
  for (uint8_t rank = 0; rank < 8; rank++) {
    for (uint8_t file = 0; file < 8; file++) {
      Square from = {rank, file};
      const Piece &p = board.pieces[from.rank * 8 + from.file];

      if (p.type() == PieceType::None) {
        continue;
      }

      if (p._color != board.turn) {
        continue;
      }

      switch (p.type()) {
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
  const auto &p = board.pieces[from.rank * 8 + from.file];
  int8_t dy = p.color() == Color::White ? 1 : -1;
  bool start_square = (p.color() == Color::White) && (from.rank == 1) ||
                      (p.color() == Color::Black) && (from.rank == 6);

  // Propose attacking moves
  {
    // Attack right
    if (util::within_bounds(from.rank + dy, from.file + 1)) {
      Square move = {from.rank + dy, from.file + 1};
      if (board.pieces[move.rank * 8 + move.file].type() != PieceType::None &&
          (board.pieces[move.rank * 8 + move.file]._color != p._color)) {
        moves.push_back({from, move});
      }
    }

    // Attack left
    if (util::within_bounds(from.rank + dy, from.file - 1)) {
      Square move = {from.rank + dy, from.file - 1};
      if (board.pieces[move.rank * 8 + move.file].type() != PieceType::None &&
          (board.pieces[move.rank * 8 + move.file]._color != p._color)) {
        moves.push_back({from, move});
      }
    }
  }

  // Propose moving moves
  if (start_square) {
    Square move = {from.rank + 2 * dy, from.file};
    if (board.pieces[(from.rank + dy) * 8 + from.file].type() == PieceType::None && board.pieces[move.rank*8 + move.file].type() == PieceType::None) {
      moves.push_back({from, move});
    }
  }

  {
    Square move = {from.rank + dy, from.file};
    if (board.pieces[move.rank*8 +move.file].type() == PieceType::None) {
      moves.push_back({from, move});
    }
  }
}

void Engine::propose_knight_moves(const Board &board, std::vector<Move> &moves,
                                  const Square &from) {
  for (const auto &[rank, file] : KnightMoveTable::get(from.rank, from.file)) {
    moves.push_back({from, {rank, file}});
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

      if (!util::within_bounds(rank, file)) {
        continue;
      }

      Square move = {rank, file};
      if (board.pieces[move.rank * 8 + move.file].type() != PieceType::None && board.pieces[move.rank * 8 + move.file]._color == board.turn) {
        continue;
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
    if (board.pieces[move.rank*8+ move.file].type() != PieceType::None) {
      if (board.pieces[move.rank*8+ move.file]._color != board.turn) {
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
    if (board.pieces[move.rank*8+ move.file].type() != PieceType::None) {
      if (board.pieces[move.rank*8+ move.file]._color != board.turn) {
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
  b.pieces[move.to.rank*8+move.to.file] = b.pieces[move.from.rank*8+move.from.file];
  b.pieces[move.from.rank*8+move.from.file]._type = static_cast<uint8_t>(PieceType::None);
  b.turn = !b.turn;
  calculate_occupy(b);
  if (checkmate(b)) {
    b.game_over = true;
    if (static_cast<Color>(board.turn) == Color::White) {
      b.result = Result::BlackWins;
    } else {
      b.result = Result::WhiteWins;
    }
  }
  return b;
}

void Engine::calculate_occupy(Board& board) {
  board.white_control = 0;
  board.black_control = 0;

  for (int rank = 0; rank < 8; rank++) {
    for (int file = 0; file < 8; file++) {
      const auto& piece = board.pieces[rank*8+file];
      bool white = piece.color() == Color::White;

      switch (piece.type()) {
      case PieceType::Pawn:
        if (white) {
          board.occupy(rank+1, file+1, /* white */true);
          board.occupy(rank+1, file-1, /* white */true);
        } else {
          board.occupy(rank-1, file+1, /* white */false);
          board.occupy(rank-1, file-1, /* white */false);
        }
        break;
      case PieceType::Knight:
        for (const auto& [k_rank, k_file] : KnightMoveTable::get(rank, file)) {
          board.occupy(k_rank, k_file, white);
        }
        break;
      case PieceType::Rook:
        rook_occupy(board, rank, file, white);
        break;
      case PieceType::King:
        for (int dy = -1; dy <= 1; dy++) {
          for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) {
              continue;
            }

            board.occupy(rank+dy, file+dx, white);
          }
        }
        break;
      case PieceType::Bishop:
        bishop_occupy(board, rank, file, white);
        break;
      case PieceType::Queen:
        rook_occupy(board, rank, file, white);
        bishop_occupy(board, rank, file, white);
        break;
      default:
        break;
      }
    }
  }
}

void Engine::rook_occupy(Board& board, int rank, int file, bool white) {
  // occupy leftward
  for (int left = 1; left < 7; left++) {
    int p_rank = rank;
    int p_file = file - left;
    if (!util::within_bounds(p_rank, p_file)) {
      continue;
    }

    const auto &p = board.pieces[p_rank * 8 + p_file];
    board.occupy(p_rank, p_file, white);
    if (p.type() != PieceType::None) {
      break;
    }
  }

  // occupy rightward
  for (int right = 1; right < 7; right++) {
    int p_rank = rank;
    int p_file = file + right;
    if (!util::within_bounds(p_rank, p_file)) {
      continue;
    }

    const auto &p = board.pieces[p_rank * 8 + p_file];
    board.occupy(p_rank, p_file, white);
    if (p.type() != PieceType::None) {
      break;
    }
  }

  // occupy up
  for (int up = 1; up < 7; up++) {
    int p_rank = rank + up;
    int p_file = file;
    if (!util::within_bounds(p_rank, p_file)) {
      continue;
    }

    const auto &p = board.pieces[p_rank * 8 + p_file];
    board.occupy(p_rank, p_file, white);
    if (p.type() != PieceType::None) {
      break;
    }
  }

  // occupy up
  for (int down = 1; down < 7; down++) {
    int p_rank = rank - down;
    int p_file = file;
    if (!util::within_bounds(p_rank, p_file)) {
      continue;
    }

    const auto &p = board.pieces[p_rank * 8 + p_file];
    board.occupy(p_rank, p_file, white);
    if (p.type() != PieceType::None) {
      break;
    }
  }
}

void Engine::bishop_occupy(Board& board, int rank, int file, bool white) {
  // occupy leftward
  for (int up_left = 1; up_left < 7; up_left++) {
    int p_rank = rank + up_left;
    int p_file = file - up_left;
    if (!util::within_bounds(p_rank, p_file)) {
      continue;
    }

    const auto &p = board.pieces[p_rank * 8 + p_file];
    board.occupy(p_rank, p_file, white);
    if (p.type() != PieceType::None) {
      break;
    }
  }

  // occupy rightward
  for (int up_right = 1; up_right < 7; up_right++) {
    int p_rank = rank + up_right;
    int p_file = file + up_right;
    if (!util::within_bounds(p_rank, p_file)) {
      continue;
    }

    const auto &p = board.pieces[p_rank * 8 + p_file];
    board.occupy(p_rank, p_file, white);
    if (p.type() != PieceType::None) {
      break;
    }
  }

  // occupy up
  for (int down_left = 1; down_left < 7; down_left++) {
    int p_rank = rank - down_left;
    int p_file = file - down_left;
    if (!util::within_bounds(p_rank, p_file)) {
      continue;
    }

    const auto &p = board.pieces[p_rank * 8 + p_file];
    board.occupy(p_rank, p_file, white);
    if (p.type() != PieceType::None) {
      break;
    }
  }

  // occupy up
  for (int down_right = 1; down_right < 7; down_right++) {
    int p_rank = rank - down_right;
    int p_file = file + down_right;
    if (!util::within_bounds(p_rank, p_file)) {
      continue;
    }

    const auto &p = board.pieces[p_rank * 8 + p_file];
    board.occupy(p_rank, p_file, white);
    if (p.type() != PieceType::None) {
      break;
    }
  }
}

bool Engine::checkmate(const Board &board) {
  for (int rank = 0; rank < 8; rank++) {
    for (int file = 0; file < 8; file++) {
      const auto& piece = board.pieces[rank*8+file];
      if (piece.type() != PieceType::King) {
        continue;
      }

      bool white_controls_square =
          board.white_control & (1ULL << (rank * 8 + file));
      bool black_controls_square =
          board.black_control & (1ULL << (rank * 8 + file));
      bool white = piece.color() == Color::White;

      if (white && black_controls_square || !white && white_controls_square) {
        // Check
        bool any_escape = false;
        for (int dy = -1; dy <= 1; dy++) {
          for (int dx = -1; dx <= 1; dx++) {
            if (dy == 0 && dx == 0) {
              continue;
            }

            int o_rank = rank + dy;
            int o_file = file + dx;
            if (!util::within_bounds(o_rank, o_file)) {
              continue;
            }

            bool o_black_controls_square =
                board.black_control & (1ULL << (o_rank * 8 + o_file));
            bool o_white_controls_square =
                board.white_control & (1ULL << (o_rank * 8 + o_file));
            bool empty =
                board.pieces[o_rank * 8 + o_file].type() == PieceType::None;
            if ((white && !o_black_controls_square) ||
                (!white && !o_white_controls_square) && empty) {
              any_escape = true;
              break;
            }
          }
          if (any_escape) {
            break;
          }
        }

        if (!any_escape && board.turn == piece._color) {
          return true;
        }
      }
    }
  }
  return false;
}
