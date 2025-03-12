#pragma once

#include "board.h"

class Engine {
public:
    struct Move {
        Square from;
        Square to;
    };

    Move best_move(const Board& board, int depth);

    double alpha_beta(const Board& board, int depth, bool white, double alpha, double beta);

    double evaluate(const Board& board);
    double evaluate_material_count(const Board& board);
    double evaluate_piece_tables(const Board& board);

    void generate_moves(const Board& board, std::vector<Move>& moves);
    void propose_pawn_moves(const Board& board, std::vector<Move>& moves, const Square& from);
    void propose_knight_moves(const Board& board, std::vector<Move>& moves, const Square& from);
    void propose_king_moves(const Board& board, std::vector<Move>& moves, const Square& from);
    void propose_rook_moves(const Board& board, std::vector<Move>& moves, const Square& from);
    void propose_bishop_moves(const Board& board, std::vector<Move>& moves, const Square& from);
    void propose_queen_moves(const Board& board, std::vector<Move>& moves, const Square& from);

    Board make_move(const Board& board, const Move& move);

    bool is_checkmate(const Board& board);
    bool is_stalemate(const Board& board);
    bool in_check(const Board& board, Color side);
};
