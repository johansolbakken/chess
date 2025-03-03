#include "fen.h"
#include "util.h"
#include "engine.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::println("Usage: {} file_path.fen", argv[0]);
        return 1;
    }

    FENParser parser;
    Board board = parser.parse_fen(read_file(argv[1]));

    Engine engine;
    Engine::Move move = engine.best_move(board, 2);
    std::println("Best move: {} -> {}", to_string(move.from), to_string(move.to));

    return 0;
}
