#ifndef PIECE_H
#define PIECE_H

#include <string>

enum class Color { NONE, WHITE, BLACK };
enum class PieceType { NONE, PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING };

struct Position {
    int row;
    int col;

    bool operator==(const Position& other) const {
        return row == other.row && col == other.col;
    }
};

class Board;

class Piece {
protected:
    Color color;
    PieceType type;

public:
    Piece(Color c, PieceType t) : color(c), type(t) {}
    virtual ~Piece() = default;

    Color getColor() const { return color; }
    PieceType getType() const { return type; }

    virtual bool isValidMove(Position start, Position end, const Board& board) const = 0;
    virtual std::string getSymbol() const = 0;
};

class Pawn : public Piece {
public:
    Pawn(Color c) : Piece(c, PieceType::PAWN) {}
    bool isValidMove(Position start, Position end, const Board& board) const override;
    std::string getSymbol() const override;
};

class Rook : public Piece {
public:
    Rook(Color c) : Piece(c, PieceType::ROOK) {}
    bool isValidMove(Position start, Position end, const Board& board) const override;
    std::string getSymbol() const override;
};

class Knight : public Piece {
public:
    Knight(Color c) : Piece(c, PieceType::KNIGHT) {}
    bool isValidMove(Position start, Position end, const Board& board) const override;
    std::string getSymbol() const override;
};

class Bishop : public Piece {
public:
    Bishop(Color c) : Piece(c, PieceType::BISHOP) {}
    bool isValidMove(Position start, Position end, const Board& board) const override;
    std::string getSymbol() const override;
};

class Queen : public Piece {
public:
    Queen(Color c) : Piece(c, PieceType::QUEEN) {}
    bool isValidMove(Position start, Position end, const Board& board) const override;
    std::string getSymbol() const override;
};

class King : public Piece {
public:
    King(Color c) : Piece(c, PieceType::KING) {}
    bool isValidMove(Position start, Position end, const Board& board) const override;
    std::string getSymbol() const override;
};

#endif // PIECE_H
