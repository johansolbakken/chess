#include "fen.h"
#include "util.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::println("Usage: {} file_path.fen", argv[0]);
        return 1;
    }

    FENParser parser;
    Board board = parser.parse_fen(read_file(argv[1]));
    std::println("Board: {}", sizeof(Board));

    return 0;
}
