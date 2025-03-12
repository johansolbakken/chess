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
    board.aggregate();

    Engine engine;
    for (int i = 0; i < 50 && !board.game_over; i++) {
      Engine::Move move = engine.best_move(board, 15);
      std::println("Best move: {} -> {}", to_string(move.from),
                   to_string(move.to));
      board = engine.make_move(board, move);
    }

    if (board.game_over) {
      switch (board.result) {
      case Result::WhiteWins:
        std::println("White wins!!");
        break;
      case Result::BlackWins:
        std::println("Black wins!!");
        break;
      case Result::Draw:
        std::println("Draw o.o");
        break;
      case Result::Stalemate:
        std::println("Stalemate ::");
        break;
      }
    }

    return 0;
}
