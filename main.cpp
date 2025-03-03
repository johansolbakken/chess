#include "fen.h"
#include "util.h"
#include "engine.h"

// #include <fstream>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::println("Usage: {} file_path.fen", argv[0]);
        return 1;
    }

    FENParser parser;
    Board board = parser.parse_fen(read_file(argv[1]));



    //std::ofstream file("output.fen");
    //file << parser.to_fen(board);

    Engine engine;
    std::println("Best move: {}", engine.best_move(board));

    return 0;
}
