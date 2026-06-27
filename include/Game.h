#ifndef GAME_H
#define GAME_H
#include "Board.h"
#include "AI.h"
#include <string>

class Game {
private:
    Board board;
    AI ai;
    Color currentTurn;
    bool isRunning;

    Position parsePosition(const std::string& input) const;
    bool isValidTurnMove(Position start, Position end) const;
    void switchTurn();
    void handleAITurn();

public:
    Game();
    void play();
};
#endif // GAME_H