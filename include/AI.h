#ifndef AI_H
#define AI_H
#include "Board.h"

struct Move {
    Position start;
    Position end;
};

class AI {
public:
    AI(Color color, int depth = 3);
    Move getBestMove(Board& board);

private:
    Color aiColor;
    Color humanColor;
    int searchDepth;

    int evaluate(Board& board);
    int minimax(Board board, int depth, int alpha, int beta, bool isMaximizing);
    int getPieceValue(PieceType type);
};

#endif // AI_H