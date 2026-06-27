#ifndef BOARD_H
#define BOARD_H

#include "Piece.h"
#include <memory>

class Board {
private:
    std::unique_ptr<Piece> grid[8][8];

public:
    Board();
    ~Board() = default;

    // Disallow copying to prevent unique_ptr copying issues
    Board(const Board&) = delete;
    Board& operator=(const Board&) = delete;

    // Allow moving
    Board(Board&&) noexcept = default;
    Board& operator=(Board&&) noexcept = default;
    Board clone() const;

    void initialize();
    void display() const;
    
    Piece* getPiece(Position pos) const;
    bool movePiece(Position start, Position end);
    bool isSquareOccupied(Position pos) const;
    bool isPathClear(Position start, Position end) const;
    bool isWithinBounds(Position pos) const;

    // Chess Rule helpers
    Position findKing(Color color) const;
    bool isKingInCheck(Color color) const;
    bool wouldBeInCheck(Position start, Position end, Color color);
    bool hasLegalMoves(Color color);
    void promotePawn(Position pos, char promotionType);
};

#endif // BOARD_H
