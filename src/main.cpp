#define SDL_MAIN_HANDLED
#include "PrintHello.hpp"
#include <SDL.h>
#include <iostream>
#include <Gra.hpp>
#include <vector>
#include <Inicjalizacja.hpp>


int main(int argc, char* argv[])
{
    if (inicjalizacja(argc, argv) != 0)
    {
        std::cout << "Inicjalizacja nie powiod³a siê!" << std::endl;
        return 1;
    }

    vector< vector< int > > grid(WIDTH / TILE_SIZE, vector< int >(HEIGHT / TILE_SIZE, 0));
    Uint32                  lastMoveTime = SDL_GetTicks();

    srand(time(NULL));
    cur    = blocks[rand() % 7];
    rect.w = rect.h     = TILE_SIZE;
    running             = 1;
    static int lastTime = 0;


    while (running)
    {
        lastFrame = SDL_GetTicks();
        if (lastFrame >= (lastTime + 1000))
        {
            lastTime   = lastFrame;
            fps        = frameCount;
            frameCount = 0;
        }

        update(grid, lastMoveTime);
        input(grid);
        render(grid);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}