#include "horse.h"
#include "util.h"

constexpr std::array<std::pair<uint8_t, uint8_t>, 8> knight_offsets{{
    {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
    {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
}};

constexpr KnightMoves create_knight_moves(int rank, int file) {
    KnightMoves knight_moves{};
    for (const auto& offset : knight_offsets) {
        int n_rank = rank + offset.first;
        int n_file = file + offset.second;
        if (util::within_bounds(n_rank, n_file)) {
            knight_moves.moves[knight_moves.count] = {n_rank, n_file};
            knight_moves.count++;
        }
    }
    return knight_moves;
}

constexpr std::array<KnightMoves, 64> precompute_knight_moves() {
    std::array<KnightMoves, 64> board{};
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            board[rank*8+file] = create_knight_moves(rank, file);
        }
    }
    return board;
}

constexpr auto knight_move_table = precompute_knight_moves();


const KnightMoves& KnightMoveTable::get(uint8_t rank, uint8_t file) {
    return knight_move_table[rank*8+file];
}
