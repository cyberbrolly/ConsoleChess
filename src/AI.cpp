#include "AI.h"
#include <limits>

AI::AI(Color color, int depth)
    : aiColor(color),
      humanColor(color == Color::BLACK ? Color::WHITE : Color::BLACK),
      searchDepth(depth) {}

int AI::getPieceValue(PieceType type) {
    switch (type) {
        case PieceType::PAWN:   return 100;
        case PieceType::KNIGHT: return 320;
        case PieceType::BISHOP: return 330;
        case PieceType::ROOK:   return 500;
        case PieceType::QUEEN:  return 900;
        case PieceType::KING:   return 20000;
        default: return 0;
    }
}

int AI::evaluate(Board& board) {
    int score = 0;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Piece* p = board.getPiece({r, c});
            if (!p) continue;
            int val = getPieceValue(p->getType());
            score += (p->getColor() == aiColor) ? val : -val;
        }
    }
    return score;
}

int AI::minimax(Board board, int depth, int alpha, int beta, bool isMaximizing) {
    Color current = isMaximizing ? aiColor : humanColor;

    if (!board.hasLegalMoves(current)) {
        if (board.isKingInCheck(current))
            return isMaximizing ? -100000 : 100000;
        return 0; // Stalemate
    }

    if (depth == 0) return evaluate(board);

    int best = isMaximizing ? std::numeric_limits<int>::min()
                            : std::numeric_limits<int>::max();

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Piece* p = board.getPiece({r, c});
            if (!p || p->getColor() != current) continue;

            for (int tr = 0; tr < 8; ++tr) {
                for (int tc = 0; tc < 8; ++tc) {
                    Position start = {r, c};
                    Position end   = {tr, tc};

                    if (!p->isValidMove(start, end, board)) continue;
                    if (board.wouldBeInCheck(start, end, current)) continue;

                    // Clone and make move on the copy
                    Board copy = board.clone();
                    copy.movePiece(start, end);

                    int score = minimax(std::move(copy), depth - 1, alpha, beta, !isMaximizing);

                    if (isMaximizing) {
                        best = std::max(best, score);
                        alpha = std::max(alpha, best);
                    } else {
                        best = std::min(best, score);
                        beta = std::min(beta, best);
                    }
                    if (beta <= alpha) goto pruned;
                }
            }
        }
    }
    pruned:
    return best;
}

Move AI::getBestMove(Board& board) {
    int bestScore = std::numeric_limits<int>::min();
    Move bestMove = {{-1, -1}, {-1, -1}};

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Piece* p = board.getPiece({r, c});
            if (!p || p->getColor() != aiColor) continue;

            for (int tr = 0; tr < 8; ++tr) {
                for (int tc = 0; tc < 8; ++tc) {
                    Position start = {r, c};
                    Position end   = {tr, tc};

                    if (!p->isValidMove(start, end, board)) continue;
                    if (board.wouldBeInCheck(start, end, aiColor)) continue;

                    Board copy = board.clone();
                    copy.movePiece(start, end);

                    int score = minimax(std::move(copy), searchDepth - 1,
                                        std::numeric_limits<int>::min(),
                                        std::numeric_limits<int>::max(),
                                        false);

                    if (score > bestScore) {
                        bestScore = score;
                        bestMove = {start, end};
                    }
                }
            }
        }
    }
    return bestMove;
}