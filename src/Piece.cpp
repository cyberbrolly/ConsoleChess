#include "Piece.h"
#include "Board.h"
#include <cmath>

// Pawn Move Validation
bool Pawn::isValidMove(Position start, Position end, const Board& board) const {
    int direction = (color == Color::WHITE) ? -1 : 1;
    int startRow = (color == Color::WHITE) ? 6 : 1;

    // 1. One square forward
    if (end.row == start.row + direction && end.col == start.col) {
        return !board.isSquareOccupied(end);
    }

    // 2. Two squares forward from initial position
    if (start.row == startRow && end.row == start.row + 2 * direction && end.col == start.col) {
        Position intermediate = {start.row + direction, start.col};
        return !board.isSquareOccupied(end) && !board.isSquareOccupied(intermediate);
    }

    // 3. Diagonal capture
    if (end.row == start.row + direction && std::abs(end.col - start.col) == 1) {
        if (board.isSquareOccupied(end)) {
            return board.getPiece(end)->getColor() != color;
        }
    }

    return false;
}

std::string Pawn::getSymbol() const {
    return "♟";
}

// Rook Move Validation
bool Rook::isValidMove(Position start, Position end, const Board& board) const {
    if (start.row != end.row && start.col != end.col) {
        return false;
    }

    if (!board.isPathClear(start, end)) {
        return false;
    }

    if (board.isSquareOccupied(end)) {
        return board.getPiece(end)->getColor() != color;
    }

    return true;
}

std::string Rook::getSymbol() const {
    return "♜";
}

// Knight Move Validation
bool Knight::isValidMove(Position start, Position end, const Board& board) const {
    int rowDiff = std::abs(start.row - end.row);
    int colDiff = std::abs(start.col - end.col);

    if (!((rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2))) {
        return false;
    }

    if (board.isSquareOccupied(end)) {
        return board.getPiece(end)->getColor() != color;
    }

    return true;
}

std::string Knight::getSymbol() const {
    return "♞";
}

// Bishop Move Validation
bool Bishop::isValidMove(Position start, Position end, const Board& board) const {
    if (std::abs(start.row - end.row) != std::abs(start.col - end.col)) {
        return false;
    }

    if (!board.isPathClear(start, end)) {
        return false;
    }

    if (board.isSquareOccupied(end)) {
        return board.getPiece(end)->getColor() != color;
    }

    return true;
}

std::string Bishop::getSymbol() const {
    return "♝";
}

// Queen Move Validation
bool Queen::isValidMove(Position start, Position end, const Board& board) const {
    bool isRookMove = (start.row == end.row || start.col == end.col);
    bool isBishopMove = (std::abs(start.row - end.row) == std::abs(start.col - end.col));

    if (!isRookMove && !isBishopMove) {
        return false;
    }

    if (!board.isPathClear(start, end)) {
        return false;
    }

    if (board.isSquareOccupied(end)) {
        return board.getPiece(end)->getColor() != color;
    }

    return true;
}

std::string Queen::getSymbol() const {
    return "♛";
}

// King Move Validation
bool King::isValidMove(Position start, Position end, const Board& board) const {
    int rowDiff = std::abs(start.row - end.row);
    int colDiff = std::abs(start.col - end.col);

    if (rowDiff > 1 || colDiff > 1) {
        return false;
    }

    if (board.isSquareOccupied(end)) {
        return board.getPiece(end)->getColor() != color;
    }

    return true;
}

std::string King::getSymbol() const {
    return "♚";
}
