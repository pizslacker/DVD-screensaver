#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void randomizeColor(Uint8 *r, Uint8 *g, Uint8 *b) {
    *r = 50 + rand() % 206;
    *g = 50 + rand() % 206;
    *b = 50 + rand() % 206;
}

int main(int argc, char* argv[]) {
    int screenWidth = 1280;
    int screenHeight = 1024;

    if (argc >= 3) {
        int parsedW = atoi(argv[1]);
        int parsedH = atoi(argv[2]);
        if (parsedW > 0 && parsedH > 0) {
            screenWidth = parsedW;
            screenHeight = parsedH;
        }
    }

    // Initialize SDL, Image, and TTF
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) return 1;
    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("DVD Screensaver", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_Texture* logoTexture = IMG_LoadTexture(renderer, "DVD_logo.png");
    if (!logoTexture) {
        printf("Missing 'DVD_logo.png'\n");
        return 1;
    }

    int logoW, logoH;
    SDL_QueryTexture(logoTexture, NULL, NULL, &logoW, &logoH);

    // Load Font
    TTF_Font* font = TTF_OpenFont("font.ttf", 28);
    if (!font) {
        printf("Failed to load 'font.ttf'! Make sure it exists. TTF_Error: %s\n", TTF_GetError());
        return 1;
    }

    srand((unsigned int)time(NULL));

    float posX = rand() % (screenWidth - logoW);
    float posY = rand() % (screenHeight - logoH);
    float velX = 3.5f; 
    float velY = 3.5f;
    
    Uint8 r, g, b;
    randomizeColor(&r, &g, &b);

    // Tracker variables
    int cornerHits = 0;
    int updateText = 1;
    SDL_Texture* textTexture = NULL;
    int textW = 0, textH = 0;
    SDL_Color textColor = {255, 255, 255, 255}; // White text

    int quit = 0;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
                quit = 1;
            }
        }

        posX += velX;
        posY += velY;

        int hitX = 0;
        int hitY = 0;

        // Collision logic
        if (posX <= 0) {
            posX = 0; velX = -velX; hitX = 1;
        } else if (posX + logoW >= screenWidth) {
            posX = screenWidth - logoW; velX = -velX; hitX = 1;
        }

        if (posY <= 0) {
            posY = 0; velY = -velY; hitY = 1;
        } else if (posY + logoH >= screenHeight) {
            posY = screenHeight - logoH; velY = -velY; hitY = 1;
        }

        // Check for corner hit (bouncing off X and Y bounds at the exact same time)
        if (hitX || hitY) {
            randomizeColor(&r, &g, &b);
            
            if (hitX && hitY) {
                cornerHits++;
                updateText = 1;
            }
        }

        // Recreate text texture ONLY when the score changes
        if (updateText) {
            if (textTexture) SDL_DestroyTexture(textTexture);
            char textBuf[64];
            snprintf(textBuf, sizeof(textBuf), "Corner Hits: %d", cornerHits);
            
            SDL_Surface* textSurface = TTF_RenderText_Blended(font, textBuf, textColor);
            textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            textW = textSurface->w;
            textH = textSurface->h;
            SDL_FreeSurface(textSurface);
            updateText = 0;
        }

        // Render everything
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw Text
        SDL_Rect textRect = { 20, 20, textW, textH };
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        // Draw Logo
        SDL_SetTextureColorMod(logoTexture, r, g, b);
        SDL_Rect renderQuad = { (int)posX, (int)posY, logoW, logoH };
        SDL_RenderCopy(renderer, logoTexture, NULL, &renderQuad);

        SDL_RenderPresent(renderer);
    }

    // Cleanup
    if (textTexture) SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);
    SDL_DestroyTexture(logoTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}