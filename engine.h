#pragma once

#include "board.h"

#include <string>

class Engine {
public:
    struct Move {
        Square from;
        Square to;
        double score;
    };

    std::string best_move(const Board& board);

    bool legal_move(const Board& board, const Square& from, const Square& to);

    void propose_pawn_moves(const Board& board, std::vector<Move>& moves, const Square& from);

    Board move(const Board& board, const Square& from, const Square& to);

    double evaluate(const Board& board);
};
