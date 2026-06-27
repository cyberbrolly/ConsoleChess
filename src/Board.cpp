#include "Board.h"
#include <iostream>

Board::Board() {
    initialize();
}

void Board::initialize() {
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            grid[r][c].reset();

    grid[0][0] = std::make_unique<Rook>(Color::BLACK);
    grid[0][1] = std::make_unique<Knight>(Color::BLACK);
    grid[0][2] = std::make_unique<Bishop>(Color::BLACK);
    grid[0][3] = std::make_unique<Queen>(Color::BLACK);
    grid[0][4] = std::make_unique<King>(Color::BLACK);
    grid[0][5] = std::make_unique<Bishop>(Color::BLACK);
    grid[0][6] = std::make_unique<Knight>(Color::BLACK);
    grid[0][7] = std::make_unique<Rook>(Color::BLACK);

    for (int c = 0; c < 8; ++c)
        grid[1][c] = std::make_unique<Pawn>(Color::BLACK);

    for (int c = 0; c < 8; ++c)
        grid[6][c] = std::make_unique<Pawn>(Color::WHITE);

    grid[7][0] = std::make_unique<Rook>(Color::WHITE);
    grid[7][1] = std::make_unique<Knight>(Color::WHITE);
    grid[7][2] = std::make_unique<Bishop>(Color::WHITE);
    grid[7][3] = std::make_unique<Queen>(Color::WHITE);
    grid[7][4] = std::make_unique<King>(Color::WHITE);
    grid[7][5] = std::make_unique<Bishop>(Color::WHITE);
    grid[7][6] = std::make_unique<Knight>(Color::WHITE);
    grid[7][7] = std::make_unique<Rook>(Color::WHITE);
}

void Board::display() const {
    std::cout << "\n    a  b  c  d  e  f  g  h\n";
    std::cout << "  +------------------------+\n";

    for (int r = 0; r < 8; ++r) {
        std::cout << (8 - r) << " |";
        for (int c = 0; c < 8; ++c) {
            bool isLightSquare = ((r + c) % 2 == 0);
            std::cout << (isLightSquare ? "\033[47m" : "\033[42m");

            Piece* piece = grid[r][c].get();
            if (piece) {
                if (piece->getColor() == Color::WHITE)
                    std::cout << " " << piece->getSymbol() << " ";
                else
                    std::cout << "\033[1;30m " << piece->getSymbol() << " ";
            } else {
                std::cout << "   ";
            }
            std::cout << "\033[0m";
        }
        std::cout << "| " << (8 - r) << "\n";
    }

    std::cout << "  +------------------------+\n";
    std::cout << "    a  b  c  d  e  f  g  h\n\n";
}

Piece* Board::getPiece(Position pos) const {
    if (!isWithinBounds(pos)) return nullptr;
    return grid[pos.row][pos.col].get();
}

bool Board::movePiece(Position start, Position end) {
    if (!isWithinBounds(start) || !isWithinBounds(end)) return false;

    Piece* movingPiece = grid[start.row][start.col].get();
    if (!movingPiece) return false;

    if (!movingPiece->isValidMove(start, end, *this)) return false;

    grid[end.row][end.col] = std::move(grid[start.row][start.col]);
    grid[start.row][start.col].reset();
    return true;
}

bool Board::isSquareOccupied(Position pos) const {
    if (!isWithinBounds(pos)) return false;
    return grid[pos.row][pos.col] != nullptr;
}

bool Board::isPathClear(Position start, Position end) const {
    int rowStep = 0, colStep = 0;
    int rowDiff = end.row - start.row;
    int colDiff = end.col - start.col;

    if (rowDiff > 0) rowStep = 1;
    else if (rowDiff < 0) rowStep = -1;

    if (colDiff > 0) colStep = 1;
    else if (colDiff < 0) colStep = -1;

    int currRow = start.row + rowStep;
    int currCol = start.col + colStep;

    while (currRow != end.row || currCol != end.col) {
        if (grid[currRow][currCol] != nullptr) return false;
        currRow += rowStep;
        currCol += colStep;
    }
    return true;
}

bool Board::isWithinBounds(Position pos) const {
    return pos.row >= 0 && pos.row < 8 && pos.col >= 0 && pos.col < 8;
}

// --- NEW METHODS BELOW ---

Position Board::findKing(Color color) const {
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            if (grid[r][c] && grid[r][c]->getType() == PieceType::KING
                && grid[r][c]->getColor() == color)
                return {r, c};
    return {-1, -1};
}

bool Board::isKingInCheck(Color color) const {
    Position kingPos = findKing(color);
    if (!isWithinBounds(kingPos)) return false;

    Color enemy = (color == Color::WHITE) ? Color::BLACK : Color::WHITE;
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            if (grid[r][c] && grid[r][c]->getColor() == enemy)
                if (grid[r][c]->isValidMove({r, c}, kingPos, *this))
                    return true;
    return false;
}

bool Board::wouldBeInCheck(Position start, Position end, Color color) {
    // Temporarily make the move
    std::unique_ptr<Piece> captured = std::move(grid[end.row][end.col]);
    grid[end.row][end.col] = std::move(grid[start.row][start.col]);
    grid[start.row][start.col].reset();

    bool inCheck = isKingInCheck(color);

    // Undo the move
    grid[start.row][start.col] = std::move(grid[end.row][end.col]);
    grid[end.row][end.col] = std::move(captured);

    return inCheck;
}

bool Board::hasLegalMoves(Color color) {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            if (!grid[r][c] || grid[r][c]->getColor() != color) continue;
            for (int tr = 0; tr < 8; ++tr) {
                for (int tc = 0; tc < 8; ++tc) {
                    Position start = {r, c};
                    Position end = {tr, tc};
                    if (grid[r][c]->isValidMove(start, end, *this))
                        if (!wouldBeInCheck(start, end, color))
                            return true;
                }
            }
        }
    }
    return false;
}

void Board::promotePawn(Position pos, char promotionType) {
    if (!isWithinBounds(pos)) return;
    Piece* p = grid[pos.row][pos.col].get();
    if (!p || p->getType() != PieceType::PAWN) return;

    Color c = p->getColor();
    switch (std::toupper(promotionType)) {
        case 'Q': grid[pos.row][pos.col] = std::make_unique<Queen>(c); break;
        case 'R': grid[pos.row][pos.col] = std::make_unique<Rook>(c); break;
        case 'B': grid[pos.row][pos.col] = std::make_unique<Bishop>(c); break;
        case 'N': grid[pos.row][pos.col] = std::make_unique<Knight>(c); break;
        default:  grid[pos.row][pos.col] = std::make_unique<Queen>(c); break;
    }
}

Board Board::clone() const {
    Board newBoard;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            newBoard.grid[r][c].reset();
            if (!grid[r][c]) continue;
            Color col = grid[r][c]->getColor();
            switch (grid[r][c]->getType()) {
                case PieceType::PAWN:   newBoard.grid[r][c] = std::make_unique<Pawn>(col); break;
                case PieceType::ROOK:   newBoard.grid[r][c] = std::make_unique<Rook>(col); break;
                case PieceType::KNIGHT: newBoard.grid[r][c] = std::make_unique<Knight>(col); break;
                case PieceType::BISHOP: newBoard.grid[r][c] = std::make_unique<Bishop>(col); break;
                case PieceType::QUEEN:  newBoard.grid[r][c] = std::make_unique<Queen>(col); break;
                case PieceType::KING:   newBoard.grid[r][c] = std::make_unique<King>(col); break;
                default: break;
            }
        }
    }
    return newBoard;
}