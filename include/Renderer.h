#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "Board.h"
#include "AI.h"
#include <string>

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool init();
    void run();

private:
    SDL_Window*   window     = nullptr;
    SDL_Renderer* renderer   = nullptr;
    TTF_Font*     font       = nullptr;
    TTF_Font*     smallFont  = nullptr;

    Board board;
    AI    ai;
    Color currentTurn;
    bool  isRunning;
    bool  gameOver;
    std::string gameOverMsg;

    Position selectedPos;
    bool     hasSelection;

    void drawBoard();
    void drawCoordinates();
    void drawPieces();
    void drawHighlight(Position pos, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    void drawValidMoves();
    void drawStatusBar();
    void drawText(const std::string& text, int x, int y, SDL_Color color);
    void drawSmallText(const std::string& text, int x, int y, SDL_Color color);
    void handleClick(int mouseX, int mouseY);
    void checkGameOver();
    Position screenToBoard(int x, int y) const;

    static const int TILE_SIZE  = 72;
    static const int BOARD_SIZE = TILE_SIZE * 8;
    static const int BOARD_OFFSET = 24;
    static const int STATUS_H   = 50;
    static const int WINDOW_W   = BOARD_SIZE + BOARD_OFFSET + 10;
    static const int WINDOW_H   = BOARD_SIZE + BOARD_OFFSET + STATUS_H + 10;
};

#endif // RENDERER_H