#include "Game.h"
#include <iostream>
#include <sstream>
#include <algorithm>

Game::Game() : ai(Color::BLACK, 3), currentTurn(Color::WHITE), isRunning(true) {}

Position Game::parsePosition(const std::string& input) const {
    if (input.length() != 2) return {-1, -1};
    char colChar = std::tolower(input[0]);
    char rowChar = input[1];
    if (colChar < 'a' || colChar > 'h' || rowChar < '1' || rowChar > '8')
        return {-1, -1};
    return {8 - (rowChar - '0'), colChar - 'a'};
}

bool Game::isValidTurnMove(Position start, Position end) const {
    Piece* piece = board.getPiece(start);
    if (!piece) {
        std::cout << "Error: No piece at the starting square.\n";
        return false;
    }
    if (piece->getColor() != currentTurn) {
        std::cout << "Error: That's not your piece to move.\n";
        return false;
    }
    return true;
}

void Game::switchTurn() {
    currentTurn = (currentTurn == Color::WHITE) ? Color::BLACK : Color::WHITE;
}

void Game::handleAITurn() {
    std::cout << "AI is thinking...\n";
    Move best = ai.getBestMove(board);
    if (best.start.row == -1) {
        std::cout << "AI has no moves!\n";
        isRunning = false;
        return;
    }
    board.movePiece(best.start, best.end);

    // Convert to chess notation for display
    char startCol = 'a' + best.start.col;
    int  startRow = 8 - best.start.row;
    char endCol   = 'a' + best.end.col;
    int  endRow   = 8 - best.end.row;
    std::cout << "AI moved: " << startCol << startRow
              << " -> " << endCol << endRow << "\n";
    switchTurn();
}

void Game::play() {
    std::cout << "======================================\n";
    std::cout << "      WELCOME TO CONSOLE CHESS        \n";
    std::cout << "======================================\n";
    std::cout << "You play as White. AI plays as Black.\n";
    std::cout << "Moves: [start] [end] (e.g. \"e2 e4\")\n";
    std::cout << "Type \"exit\" to quit.\n\n";

    while (isRunning) {
        board.display();

        // Check for game over
        if (!board.hasLegalMoves(currentTurn)) {
            if (board.isKingInCheck(currentTurn)) {
                std::string loser = (currentTurn == Color::WHITE) ? "White" : "Black";
                std::cout << "Checkmate! " << loser << " loses!\n";
            } else {
                std::cout << "Stalemate! It's a draw.\n";
            }
            isRunning = false;
            break;
        }

        if (board.isKingInCheck(currentTurn)) {
            std::string inCheck = (currentTurn == Color::WHITE) ? "White" : "Black";
            std::cout << inCheck << " is in check!\n";
        }

        if (currentTurn == Color::BLACK) {
            handleAITurn();
            continue;
        }

        std::cout << "White's turn. Enter your move: ";
        std::string input;
        std::getline(std::cin, input);

        // Trim
        input.erase(input.begin(), std::find_if(input.begin(), input.end(),
            [](unsigned char ch) { return !std::isspace(ch); }));
        input.erase(std::find_if(input.rbegin(), input.rend(),
            [](unsigned char ch) { return !std::isspace(ch); }).base(), input.end());

        if (input == "exit" || input == "quit") {
            std::cout << "Thanks for playing!\n";
            break;
        }

        std::stringstream ss(input);
        std::string startStr, endStr;
        ss >> startStr >> endStr;

        Position start = parsePosition(startStr);
        Position end   = parsePosition(endStr);

        if (!board.isWithinBounds(start) || !board.isWithinBounds(end)) {
            std::cout << "Invalid format. Use e.g. \"e2 e4\".\n";
            continue;
        }
        if (!isValidTurnMove(start, end)) continue;
        if (board.wouldBeInCheck(start, end, Color::WHITE)) {
            std::cout << "That move leaves your king in check!\n";
            continue;
        }
        if (board.movePiece(start, end)) {
            switchTurn();
        } else {
            std::cout << "Invalid move. Try again!\n";
        }
    }
}