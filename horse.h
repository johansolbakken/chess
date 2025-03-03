#pragma once

#include <array>

struct KnightMoves {
    std::array<std::pair<uint8_t, uint8_t>, 8> moves = {};
    uint8_t count = 0;

    using const_iterator = decltype(moves.cbegin());

    constexpr const_iterator begin() const { return moves.cbegin(); }
    constexpr const_iterator end() const { return moves.cbegin() + count; }
};

class KnightMoveTable {
public:
    static const KnightMoves& get(uint8_t rank, uint8_t file);
};
