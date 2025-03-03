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

bool FENParser::parse_board(Board& board, const std::string& board_desc) {
    std::stringstream ss(board_desc);
    std::string rank_line;
    int rank = 7;
    while (std::getline(ss, rank_line, '/')) {
        int file = 0;
        for (int i = 0; i < rank_line.size(); i++) {
            switch (rank_line[i]) {
                case 'r':
                    board.pieces[rank * 8 + file].color = static_cast<uint8_t>(Color::Black);
                    board.pieces[rank * 8 + file].type = static_cast<uint8_t>(PieceType::Rook);
                    break;
                case 'n':
                    board.pieces[rank * 8 + file].color = static_cast<uint8_t>(Color::Black);
                    board.pieces[rank * 8 + file].type = static_cast<uint8_t>(PieceType::Knight);
                    break;
                case 'b':
                    board.pieces[rank * 8 + file].color = static_cast<uint8_t>(Color::Black);
                    board.pieces[rank * 8 + file].type = static_cast<uint8_t>(PieceType::Bishop);
                    break;
                case 'q':
                    board.pieces[rank * 8 + file].color = static_cast<uint8_t>(Color::Black);
                    board.pieces[rank * 8 + file].type = static_cast<uint8_t>(PieceType::Queen);
                    break;
                case 'k':
                    board.pieces[rank * 8 + file].color = static_cast<uint8_t>(Color::Black);
                    board.pieces[rank * 8 + file].type = static_cast<uint8_t>(PieceType::King);
                    break;
                case 'p':
                    board.pieces[rank * 8 + file].color = static_cast<uint8_t>(Color::Black);
                    board.pieces[rank * 8 + file].type = static_cast<uint8_t>(PieceType::Pawn);
                    break;

                case 'R':
                    board.pieces[rank * 8 + file].color = static_cast<uint8_t>(Color::White);
                    board.pieces[rank * 8 + file].type = static_cast<uint8_t>(PieceType::Rook);
                    break;
                case 'N':
                    board.pieces[rank * 8 + file].color = static_cast<uint8_t>(Color::White);
                    board.pieces[rank * 8 + file].type = static_cast<uint8_t>(PieceType::Knight);
                    break;
                case 'B':
                    board.pieces[rank * 8 + file].color = static_cast<uint8_t>(Color::White);
                    board.pieces[rank * 8 + file].type = static_cast<uint8_t>(PieceType::Bishop);
                    break;
                case 'Q':
                    board.pieces[rank * 8 + file].color = static_cast<uint8_t>(Color::White);
                    board.pieces[rank * 8 + file].type = static_cast<uint8_t>(PieceType::Queen);
                    break;
                case 'K':
                    board.pieces[rank * 8 + file].color = static_cast<uint8_t>(Color::White);
                    board.pieces[rank * 8 + file].type = static_cast<uint8_t>(PieceType::King);
                    break;
                case 'P':
                    board.pieces[rank * 8 + file].color = static_cast<uint8_t>(Color::White);
                    board.pieces[rank * 8 + file].type = static_cast<uint8_t>(PieceType::Pawn);
                    break;

                default:
                    if (rank_line[i] >= '1' && rank_line[i] <= '8') {
                        file += static_cast<size_t>(rank_line[i] - '0');
                        continue;
                    }

                    std::println(stderr, "Unrecognized piece or number in board rank={} i={} char={}", rank, i, rank_line[i]);
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
        board.turn = static_cast<uint8_t>(Color::White);
    } else if (turn[0] == 'b') {
        board.turn = static_cast<uint8_t>(Color::Black);
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

    board.en_passant_file = static_cast<uint8_t>(en_passant[0] - 'a');
    board.en_passant_rank = static_cast<uint8_t>(en_passant[1] - '1');

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
