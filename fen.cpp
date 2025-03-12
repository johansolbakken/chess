#include "fen.h"

#include <iostream>
#include <cstdint>
#include <sstream>
#include <print>
#include <array>

Board FENParser::parse_fen(const std::string& fen) {
    Board board;

    std::string board_desc, turn, castle, en_passant, half_move, full_move;
    if (parse_blocks(fen, board_desc, turn, castle, en_passant, half_move, full_move)) {
        return board;
    }

    if (parse_board(board, board_desc)) {
        return board;
    }

    if (parse_turn(board, turn)) {
        return board;
    }

    if (parse_castle(board, castle)) {
        return board;
    }

    if (parse_en_passant(board, en_passant)) {
        return board;
    }

    if (parse_move_count(board, half_move, full_move)) {
        return board;
    }

    return board;
}

bool FENParser::parse_blocks(const std::string& fen,
                 std::string& board,
                 std::string& turn,
                 std::string& castle,
                 std::string& en_passant,
                 std::string& half_move,
                 std::string& full_move) {
    std::stringstream ss(fen);

    if (!std::getline(ss, board, ' ')) {
        std::println(stderr, "Expected piece placement block.");
        return true;
    }

    if (!std::getline(ss, turn, ' ')) {
        std::println(stderr, "Expected turn block.");
        return true;
    }

    if (!std::getline(ss, castle, ' ')) {
        std::println(stderr, "Expected castle block.");
        return true;
    }

    if (!std::getline(ss, en_passant, ' ')) {
        std::println(stderr, "Expected en_passant block.");
        return true;
    }

    if (!std::getline(ss, half_move, ' ')) {
        std::println(stderr, "Expected half_move block.");
        return true;
    }

    if (!std::getline(ss, full_move, ' ')) {
        std::println(stderr, "Expected full_move block.");
        return true;
    }

    return false;
}

bool FENParser::parse_board(Board &board, const std::string &board_desc) {
  std::stringstream ss(board_desc);
  std::string rank_line;
  int rank = 7;
  while (std::getline(ss, rank_line, '/')) {
    int file = 0;
    for (int i = 0; i < rank_line.size(); i++) {
      size_t pos = (1ULL << (rank * 8 + file));
      switch (rank_line[i]) {
      case 'r':
        board.black_rooks |= pos;
        break;
      case 'n':
        board.black_knights |= pos;
        break;
      case 'b':
        board.black_bishops |= pos;
        break;
      case 'q':
        board.black_queens |= pos;
        break;
      case 'k':
        board.black_kings |= pos;
        break;
      case 'p':
        board.black_pawns |= pos;
        break;

      case 'R':
        board.white_rooks |= pos;
        break;
      case 'N':
        board.white_knights |= pos;
        break;
      case 'B':
        board.white_bishops |= pos;
        break;
      case 'Q':
        board.white_queens |= pos;
        break;
      case 'K':
        board.white_kings |= pos;
        break;
      case 'P':
        board.white_pawns |= pos;
        break;

      default:
        if (rank_line[i] >= '1' && rank_line[i] <= '8') {
          file += static_cast<size_t>(rank_line[i] - '0');
          continue;
        }

        std::println(
            stderr,
            "Unrecognized piece or number in board rank={} i={} char={}", rank,
            i, rank_line[i]);
        return true;

        break;
      }

      file++;
    }
    rank--;
  }

  return false;
}

bool FENParser::parse_turn(Board& board, const std::string& turn) {
    if (turn[0] == 'w') {
        board.turn = Color::White;
    } else if (turn[0] == 'b') {
        board.turn = Color::Black;
    } else {
        std::println("Expected turn to be either 'w' or 'b'. Was: '{}'", turn);
        return true;
    }
    return false;
}

bool FENParser::parse_castle(Board& board, const std::string& castle) {
    if (castle[0] == '-') {
        return false;
    }

    for (size_t i = 0; i < castle.size(); i++) {
        switch (castle[i]) {
            case 'q':
                board.castle_black_queenside = 1;
                break;
            case 'k':
                board.castle_black_kingside = 1;
                break;
            case 'Q':
                board.castle_white_queenside = 1;
                break;
            case 'K':
                board.castle_white_kingside = 1;
                break;
            default:
                std::println(stderr, "In castle block expected 'q', 'Q', 'k', 'K', '-' values but got '{}' in '{}'.", castle[i], castle);
                break;
        }
    }

    return false;
}

bool FENParser::parse_en_passant(Board& board, const std::string& en_passant) {
    if (en_passant[0] == '-') {
        board.has_en_passant = false;
        return false;
    }

    if (en_passant[0] < 'a' || en_passant[0] > 'h') {
        std::println(stderr, "Failed to parse file in en passant block '{}'", en_passant[0]);
        return true;
    }

    if (en_passant[1] < '1' || en_passant[1] > '8') {
        std::println(stderr, "Failed to parse rank in en passant block '{}'", en_passant[1]);
        return true;
    }

    board.en_passant_file = static_cast<uint8_t>(static_cast<int>(en_passant[0]) - 'a');
    board.en_passant_rank = static_cast<uint8_t>(static_cast<int>(en_passant[1]) - '1');
    board.has_en_passant = true;

    return false;
}

bool FENParser::parse_move_count(Board& board, const std::string& half_move, const std::string& full_move) {
    try {
        board.half_move = std::stoi(half_move);
    } catch (const std::invalid_argument& ia) {
        std::println(stderr, "Failed to parse half move.\n{}", ia.what());
        return true;
    }

    try {
        board.full_move = std::stoi(full_move);
    } catch (const std::invalid_argument& ia) {
        std::println(stderr, "Failed to parse full move.\n{}", ia.what());
        return true;
    }

    return false;
}

std::string FENParser::to_fen(const Board& board) {
    std::stringstream ss;

    write_board(ss, board);
    ss << ' ';
    write_turn(ss, board);
    ss << ' ';
    write_castle(ss, board);
    ss << ' ';
    write_en_passant(ss, board);
    ss << ' ';
    write_move_count(ss, board);
    ss << '\n';

    return ss.str();
}

void FENParser::write_board(std::stringstream& ss, const Board& board) {
    // for (int rank = 7; rank >= 0; rank--) {
    //     size_t empty_count = 0;
    //     for (size_t file = 0; file < 8; file++) {
    //         bool is_empty = board.pieces[rank*8 + file].type() == PieceType::None;

    //         if (is_empty) {
    //             empty_count++;
    //             continue;
    //         }

    //         // not empty
    //         if (empty_count > 0) {
    //             ss << empty_count;
    //             empty_count = 0;
    //         }

    //         PieceType type = board.pieces[rank*8 + file].type();
    //         Color color = board.pieces[rank*8 + file].color();
    //         switch (type) {
    //         case PieceType::Rook:
    //           if (color == Color::White) {
    //             ss << 'R';
    //           } else {
    //             ss << 'r';
    //           }
    //           break;
    //         case PieceType::Queen:
    //           if (color == Color::White) {
    //             ss << 'Q';
    //           } else {
    //             ss << 'q';
    //           }
    //           break;
    //         case PieceType::King:
    //           if (color == Color::White) {
    //             ss << 'K';
    //           } else {
    //             ss << 'k';
    //           }
    //           break;
    //         case PieceType::Bishop:
    //           if (color == Color::White) {
    //             ss << 'B';
    //           } else {
    //             ss << 'b';
    //           }
    //           break;
    //         case PieceType::Knight:
    //           if (color == Color::White) {
    //             ss << 'N';
    //           } else {
    //             ss << 'n';
    //           }
    //           break;
    //         case PieceType::Pawn:
    //           if (color == Color::White) {
    //             ss << 'P';
    //           } else {
    //             ss << 'p';
    //           }
    //           break;
    //         default:
    //             break;
    //         }
    //     }

    //     if (empty_count > 0) {
    //       ss << empty_count;
    //     }

    //     if (rank > 0) {
    //         ss << '/';
    //     }
    // }
}

void FENParser::write_turn(std::stringstream& ss, const Board& board) {
    if (static_cast<Color>(board.turn) == Color::White) {
        ss << 'w';
    } else {
        ss << 'b';
    }
}


void FENParser::write_castle(std::stringstream& ss, const Board& board) {
    if (board.castle_white_kingside) {
        ss << 'K';
    }
    if (board.castle_white_queenside) {
        ss << 'Q';
    }
    if (board.castle_black_kingside) {
        ss << 'k';
    }
    if (board.castle_black_queenside) {
        ss << 'q';
    }
    if ((board.castle_black_kingside
         | board.castle_black_queenside
         | board.castle_white_kingside
         | board.castle_white_queenside) == 0) {
        ss << '-';
    }
}


void FENParser::write_en_passant(std::stringstream& ss, const Board& board) {
    if (!board.has_en_passant) {
        ss << '-';
        return;
    }

    ss << static_cast<char>(board.en_passant_file + 'a');
    ss << static_cast<char>(board.en_passant_rank + '1');
}

void FENParser::write_move_count(std::stringstream &ss, const Board &board) {
  ss << static_cast<int>(board.half_move) << ' ' << static_cast<int>(board.full_move);
}
