#pragma once

#include "board.h"

class Engine {
public:
    struct Move {
        Square from;
        Square to;
    };

    Move best_move(const Board& board, int depth);

    double minimax(const Board& board, int depth, bool white);

    double evaluate(const Board& board);

    bool legal_move(const Board& board, const Square& from, const Square& to);

    void generate_moves(const Board& board, std::vector<Move>& moves);
    void propose_pawn_moves(const Board& board, std::vector<Move>& moves, const Square& from);
    void propose_knight_moves(const Board& board, std::vector<Move>& moves, const Square& from);
    void propose_king_moves(const Board& board, std::vector<Move>& moves, const Square& from);
    void propose_rook_moves(const Board& board, std::vector<Move>& moves, const Square& from);
    void propose_bishop_moves(const Board& board, std::vector<Move>& moves, const Square& from);
    void propose_queen_moves(const Board& board, std::vector<Move>& moves, const Square& from);

    size_t count_pawn_attacking(const Board& board, const Square& square, const Color& color);
    size_t count_knight_attacking(const Board& board, const Square& square, Color color);

    Board make_move(const Board& board, const Move& move);
};
