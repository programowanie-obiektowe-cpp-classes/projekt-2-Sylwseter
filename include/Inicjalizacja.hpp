#ifndef INICJALIZACJA_HPP
#define INICJALIZACJA_HPP

#include <iostream>
#include <string>
#include <SDL.h>
#include <vector>

int inicjalizacja(int argc, char* argv[])
{

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        std::cout << "Failed at SDL_Init()" << std::endl;
    if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) < 0)
        std::cout << "Failed at SDL_CreateWindowAndRenderer()" << std::endl;
    SDL_SetWindowTitle(window, "Tetris");

    std::cout << "Twoj poziom: 1" << std::endl;

    return 0;
}
#endif