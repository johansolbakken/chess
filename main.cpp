#include "fen.h"
#include "util.h"
#include "engine.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::println("Usage: {} file_path.fen", argv[0]);
        return 1;
    }

    FENParser parser;
    Board board = parser.parse_fen(util::read_file(argv[1]));

    Engine engine;
    engine.calculate_occupy(board);
    for (int i = 0; i < 10; i++) {
      Engine::Move move = engine.best_move(board, 5);
      std::println("Best move: {} -> {}", to_string(move.from),
                   to_string(move.to));
      board = engine.make_move(board, move);
    }

    return 0;
}
