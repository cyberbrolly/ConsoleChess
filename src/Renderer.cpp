#include "Renderer.h"
#include <iostream>
#include <string>
#include <vector>

Renderer::Renderer()
    : ai(Color::BLACK, 3),
      currentTurn(Color::WHITE),
      isRunning(true),
      selectedPos({-1, -1}),
      hasSelection(false),
      gameOver(false) {}

Renderer::~Renderer() {
    if (font)      TTF_CloseFont(font);
    if (smallFont) TTF_CloseFont(smallFont);
    if (renderer)  SDL_DestroyRenderer(renderer);
    if (window)    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

bool Renderer::init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL Init failed: " << SDL_GetError() << "\n";
        return false;
    }
    if (TTF_Init() != 0) {
        std::cerr << "TTF Init failed: " << TTF_GetError() << "\n";
        return false;
    }

    window = SDL_CreateWindow(
        "ConsoleChess",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_W, WINDOW_H,
        SDL_WINDOW_SHOWN
    );
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;

    font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 36);
    smallFont = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 16);

    return true;
}

void Renderer::drawBoard() {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            bool isLight = (r + c) % 2 == 0;
            if (isLight)
                SDL_SetRenderDrawColor(renderer, 240, 217, 181, 255);
            else
                SDL_SetRenderDrawColor(renderer, 181, 136, 99, 255);

            SDL_Rect rect = {
                BOARD_OFFSET + c * TILE_SIZE,
                BOARD_OFFSET + r * TILE_SIZE,
                TILE_SIZE, TILE_SIZE
            };
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

void Renderer::drawCoordinates() {
    if (!smallFont) return;
    SDL_Color dark  = {100, 70, 40, 255};
    SDL_Color light = {240, 217, 181, 255};

    for (int i = 0; i < 8; ++i) {
        // Rank numbers (8 down to 1) on left side
        std::string rank = std::to_string(8 - i);
        SDL_Color col = (i % 2 == 0) ? dark : light;
        drawSmallText(rank,
            BOARD_OFFSET - 18,
            BOARD_OFFSET + i * TILE_SIZE + TILE_SIZE / 2 - 8,
            col);

        // File letters (a to h) on bottom
        std::string file(1, 'a' + i);
        col = (i % 2 == 0) ? light : dark;
        drawSmallText(file,
            BOARD_OFFSET + i * TILE_SIZE + TILE_SIZE / 2 - 5,
            BOARD_OFFSET + BOARD_SIZE + 4,
            col);
    }
}

void Renderer::drawHighlight(Position pos, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_Rect rect = {
        BOARD_OFFSET + pos.col * TILE_SIZE,
        BOARD_OFFSET + pos.row * TILE_SIZE,
        TILE_SIZE, TILE_SIZE
    };
    SDL_RenderFillRect(renderer, &rect);
}

void Renderer::drawValidMoves() {
    if (!hasSelection) return;
    Piece* p = board.getPiece(selectedPos);
    if (!p) return;

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Position end = {r, c};
            if (p->isValidMove(selectedPos, end, board)
                && !board.wouldBeInCheck(selectedPos, end, currentTurn)) {

                // Dot in center of square
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                bool isCapture = board.isSquareOccupied(end);

                if (isCapture) {
                    // Ring around capture squares
                    SDL_SetRenderDrawColor(renderer, 50, 180, 50, 180);
                    SDL_Rect rect = {
                        BOARD_OFFSET + c * TILE_SIZE,
                        BOARD_OFFSET + r * TILE_SIZE,
                        TILE_SIZE, TILE_SIZE
                    };
                    SDL_RenderDrawRect(renderer, &rect);
                    SDL_Rect inner = {
                        BOARD_OFFSET + c * TILE_SIZE + 3,
                        BOARD_OFFSET + r * TILE_SIZE + 3,
                        TILE_SIZE - 6, TILE_SIZE - 6
                    };
                    SDL_RenderDrawRect(renderer, &inner);
                } else {
                    // Small dot for empty squares
                    SDL_SetRenderDrawColor(renderer, 50, 180, 50, 160);
                    int cx = BOARD_OFFSET + c * TILE_SIZE + TILE_SIZE / 2;
                    int cy = BOARD_OFFSET + r * TILE_SIZE + TILE_SIZE / 2;
                    int radius = 10;
                    for (int dy = -radius; dy <= radius; ++dy)
                        for (int dx = -radius; dx <= radius; ++dx)
                            if (dx*dx + dy*dy <= radius*radius)
                                SDL_RenderDrawPoint(renderer, cx + dx, cy + dy);
                }
            }
        }
    }
}

void Renderer::drawPieces() {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Piece* piece = board.getPiece({r, c});
            if (!piece) continue;

            std::string symbol = piece->getSymbol();
            SDL_Color color = (piece->getColor() == Color::WHITE)
                ? SDL_Color{255, 255, 255, 255}
                : SDL_Color{30,  30,  30,  255};

            int x = BOARD_OFFSET + c * TILE_SIZE + TILE_SIZE / 2 - 16;
            int y = BOARD_OFFSET + r * TILE_SIZE + TILE_SIZE / 2 - 20;
            drawText(symbol, x, y, color);
        }
    }
}

void Renderer::drawStatusBar() {
    // Background bar
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_Rect bar = {0, WINDOW_H - STATUS_H, WINDOW_W, STATUS_H};
    SDL_RenderFillRect(renderer, &bar);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color red   = {220, 60,  60,  255};
    SDL_Color green = {60,  200, 60,  255};

    if (gameOver) {
        drawText(gameOverMsg, 20, WINDOW_H - STATUS_H + 10, red);
    } else {
        std::string status = (currentTurn == Color::WHITE)
            ? "Your turn (White)"
            : "AI is thinking...";
        SDL_Color col = (currentTurn == Color::WHITE) ? green : white;
        drawText(status, 20, WINDOW_H - STATUS_H + 10, col);

        // Check indicator
        if (board.isKingInCheck(currentTurn)) {
            drawText("  CHECK!", 300, WINDOW_H - STATUS_H + 10, red);
        }
    }
}

void Renderer::drawText(const std::string& text, int x, int y, SDL_Color color) {
    if (!font) return;
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dst = {x, y, surface->w, surface->h};
    SDL_FreeSurface(surface);
    if (!texture) return;
    SDL_RenderCopy(renderer, texture, nullptr, &dst);
    SDL_DestroyTexture(texture);
}

void Renderer::drawSmallText(const std::string& text, int x, int y, SDL_Color color) {
    if (!smallFont) return;
    SDL_Surface* surface = TTF_RenderUTF8_Blended(smallFont, text.c_str(), color);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dst = {x, y, surface->w, surface->h};
    SDL_FreeSurface(surface);
    if (!texture) return;
    SDL_RenderCopy(renderer, texture, nullptr, &dst);
    SDL_DestroyTexture(texture);
}

Position Renderer::screenToBoard(int x, int y) const {
    return {
        (y - BOARD_OFFSET) / TILE_SIZE,
        (x - BOARD_OFFSET) / TILE_SIZE
    };
}

void Renderer::handleClick(int mouseX, int mouseY) {
    Position clicked = screenToBoard(mouseX, mouseY);
    if (!board.isWithinBounds(clicked)) return;

    if (!hasSelection) {
        Piece* p = board.getPiece(clicked);
        if (p && p->getColor() == currentTurn) {
            selectedPos  = clicked;
            hasSelection = true;
        }
    } else {
        Piece* p = board.getPiece(clicked);

        // Reselect own piece
        if (p && p->getColor() == currentTurn) {
            selectedPos = clicked;
            return;
        }

        Piece* moving = board.getPiece(selectedPos);
        if (moving
            && moving->isValidMove(selectedPos, clicked, board)
            && !board.wouldBeInCheck(selectedPos, clicked, currentTurn)) {

            board.movePiece(selectedPos, clicked);
            currentTurn  = Color::BLACK;
            hasSelection = false;
            selectedPos  = {-1, -1};
        } else {
            hasSelection = false;
            selectedPos  = {-1, -1};
        }
    }
}

void Renderer::checkGameOver() {
    if (!board.hasLegalMoves(currentTurn)) {
        gameOver = true;
        if (board.isKingInCheck(currentTurn)) {
            gameOverMsg = (currentTurn == Color::WHITE)
                ? "Checkmate! AI wins!"
                : "Checkmate! You win!";
        } else {
            gameOverMsg = "Stalemate! It's a draw.";
        }
    }
}

void Renderer::run() {
    SDL_Event event;

    while (isRunning) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                isRunning = false;

            if (!gameOver
                && event.type == SDL_MOUSEBUTTONDOWN
                && event.button.button == SDL_BUTTON_LEFT
                && currentTurn == Color::WHITE) {
                handleClick(event.button.x, event.button.y);
                checkGameOver();
            }
        }

        // AI turn
        if (!gameOver && currentTurn == Color::BLACK) {
            Move best = ai.getBestMove(board);
            if (best.start.row != -1)
                board.movePiece(best.start, best.end);
            currentTurn = Color::WHITE;
            checkGameOver();
        }

        // Draw everything
        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
        SDL_RenderClear(renderer);

        drawBoard();

        // Highlight selected piece
        if (hasSelection && board.isWithinBounds(selectedPos))
            drawHighlight(selectedPos, 50, 200, 50, 150);

        // Highlight king in check
        if (board.isKingInCheck(currentTurn)) {
            Position kingPos = board.findKing(currentTurn);
            drawHighlight(kingPos, 220, 50, 50, 180);
        }

        drawValidMoves();
        drawPieces();
        drawCoordinates();
        drawStatusBar();

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}