#ifndef GRA_HPP
#define GRA_HPP

#define SDL_MAIN_HANDLED
#include <iostream>
#include <string>
#include <SDL.h>
#include <vector>


#define WIDTH 220
#define HEIGHT 340
#define TILE_SIZE 20

SDL_Rect rect;

using namespace std;
bool running;

SDL_Renderer* renderer;
SDL_Window*   window;

int  frameCount, timerFPS, lastFrame, fps;
bool moveLeft, moveRight, moveUp, moveDown;
int  level = 1;

struct block
{
    SDL_Color kolor;
    bool active;
};

struct shape
{
    SDL_Color color;
    bool matrix[4][4];
    double x, y;
    int    size;
};

shape blocks[7] = {{{255, 165, 0},
                    {{0, 0, 1, 0}, // L BLOCK
                     {1, 1, 1, 0},
                     {0, 0, 0, 0},
                     {0, 0, 0, 0}},
                    5,4,3},
                   {{255, 0, 0}, // Z BLOCK
                   {{1, 1, 0, 0}, 
                    {0, 1, 1, 0}, 
                    {0, 0, 0, 0},   
                    {0, 0, 0, 0}},
                    5, 4,3},
                   {{224, 255, 255}, // I BLOCK
                   {{1, 1, 1, 1}, 
                    {0, 0, 0, 0},  
                    {0, 0, 0, 0}, 
                    {0, 0, 0, 0}},
                    5,4,4},
                   {{0, 0, 255}, // J BLOCK
                   {{1, 0, 0, 0}, 
                    {1, 1, 1, 0}, 
                    {0, 0, 0, 0}, 
                    {0, 0, 0, 0}},
                    5,4,3},
                   {{255, 255, 0}, // O BLOCK
                   {{1, 1, 0, 0}, 
                    {1, 1, 0, 0}, 
                    {0, 0, 0, 0}, 
                    {0, 0, 0, 0}},
                    5,4,2},
                   {{0, 0, 255}, // S BLOCK
                   {{0, 1, 1, 0}, 
                    {1, 1, 0, 0}, 
                    {0, 0, 0, 0}, 
                    {0, 0, 0, 0}},
                    5,4,3},
                   {{128, 0, 128}, // T BLOCK
                   {{0, 1, 0, 0}, 
                    {1, 1, 1, 0}, 
                    {0, 0, 0, 0}, 
                    {0, 0, 0, 0}},
                    5,4,3}}, cur;

shape rotateMatrix(shape s) //transponowanie i odwracanie macierzy
{
    shape tmp = s;

    for (int i = 0; i < s.size; i++)
    {
        for (int j = 0; j < s.size; j++)
        {
            tmp.matrix[i][j] = s.matrix[j][i];
        }
    }

    for (int i = 0; i < s.size / 2; i++)
    {
        for (int j = 0; j < s.size; j++)
        {
            bool t                        = tmp.matrix[i][j];
            tmp.matrix[i][j]              = tmp.matrix[s.size - i - 1][j];
            tmp.matrix[s.size - i - 1][j] = t;
        }
    }

    return tmp;
}

// Funkcja do wyœwietlania macierzy
void displayMatrix(shape s)
{
    for (int i = 0; i < s.size; i++)
    {
        for (int j = 0; j < s.size; j++)
        {
            cout << s.matrix[i][j] << " ";
        }
        cout << endl;
    }
}

void draw(shape s)
{
    for (int i = 0; i < s.size; i++)
    {
        for (int j = 0; j < s.size; j++)
        {
            if (s.matrix[i][j])
            {
                rect.x = (s.x + i) * TILE_SIZE;
                rect.y = (s.y + j) * TILE_SIZE;
                SDL_SetRenderDrawColor(renderer, s.color.r, s.color.g, s.color.b, 255);
                SDL_RenderFillRect(renderer, &rect);
                SDL_SetRenderDrawColor(renderer, 219, 219, 219, 255);
                SDL_RenderDrawRect(renderer, &rect);
            }
        }
    }
}

bool canMove(shape& s, int dx, int dy, const vector< vector< int > >& grid)
{
    for (int i = 0; i < s.size; i++)
    {
        for (int j = 0; j < s.size; j++)
        {
            if (s.matrix[i][j]) //sprawdzenie aktywnoœci danych kwadratów w figurze, je¿eli pole w macierzy ==0, nie trzeba sprawdzaæ
            {
                int newX = s.x + i + dx;
                int newY = s.y + j + dy;

                // Sprawdzanie granic planszy
                if (newX < 0 || newX >= grid.size() || newY < 0 || newY >= grid[0].size())
                    return false;

                // Sprawdzanie kolizje z innymi klockami
                if (grid[newX][newY] != 0)
                    return false;
            }
        }
    }
    return true;
}

bool canRotate(shape& s, const vector< vector< int > >& grid)
{
    shape tmp = rotateMatrix(s); //symulowana obrócona figura

    for (int i = 0; i < tmp.size; i++)
    {
        for (int j = 0; j < tmp.size; j++)
        {
            if (tmp.matrix[i][j]) // Sprawdzamy tylko aktywne czêœci klocka
            {
                int newX = s.x + i;
                int newY = s.y + j;

                // SprawdŸ, czy pozycja jest poza granicami planszy
                if (newX < 0 || newX >= grid.size() || newY < 0 || newY >= grid[0].size())
                    return false;

                // SprawdŸ kolizje z innymi blokami
                if (grid[newX][newY] != 0)
                    return false;
            }
        }
    }

    return true; // Obrót jest mo¿liwy
}

bool canPlaceBlock(const shape& s, const vector< vector< int > >& grid)
{
    for (int i = 0; i < s.size; i++)
    {
        for (int j = 0; j < s.size; j++)
        {
            if (s.matrix[i][j]) // Sprawdzamy tylko aktywne czêœci klocka
            {
                int x = s.x + i;
                int y = s.y + j;

                // SprawdŸ, czy miejsce na planszy jest zajête
                if (x < 0 || x >= grid.size() || y < 0 || y >= grid[0].size() || grid[x][y] != 0)
                {
                    return false; // Kolizja lub brak miejsca
                }
            }
        }
    }
    return true; // Mo¿na umieœciæ klocek
}


void move(shape& s, int dx, int dy, const vector< vector< int > >& grid)
{
    if (canMove(s, dx, dy, grid))
    {
        s.x += dx;
        s.y += dy;
    }
}

void placeBlock(const shape& s, vector< vector< int > >& grid)
{
    for (int i = 0; i < s.size; i++)
    {
        for (int j = 0; j < s.size; j++)
        {
            if (s.matrix[i][j]) // Jeœli jest czêœæ klocka
            {
                int x      = s.x + i;
                int y      = s.y + j;
                grid[x][y] = 1; // Zablokuj miejsce na planszy
            }
        }
    }
}

void clearFullRows(vector< vector< int > >& grid)
{

    for (int j = 0; j < grid[0].size(); j++) // Przechodzimy przez wszystkie rzêdy
    {
        bool full = true;
        for (int i = 0; i < grid.size(); i++)
        {
            if (grid[i][j] == 0) // Jeœli znajdziemy puste pole, rz¹d nie jest pe³ny
            {
                full = false;
                break;
            }
        }

        if (full) // Jeœli rz¹d jest pe³ny
        {
            // Usuñ pe³ny rz¹d
            for (int k = j; k > 0; k--) // Przesuñ wszystkie rzêdy powy¿ej w dó³
            {
                for (int i = 0; i < grid.size(); i++)
                {
                    grid[i][k] = grid[i][k - 1];
                }
            }

            // Wyzeruj pierwszy rz¹d (na górze planszy)
            for (int i = 0; i < grid.size(); i++)
            {
                grid[i][0] = 0;
            }

            // SprawdŸ ponownie ten sam rz¹d po przesuniêciu, poniewa¿ rzêdy mog¹ siê na siebie nak³adaæ
            j--;
            level += 1;
            std::cout << "Twoj poziom: " << level << std::endl;
        }
    }
}


void update(vector< vector< int > >& grid, Uint32& lastMoveTime)
{
    Uint32 currentTime = SDL_GetTicks(); // Pobiera czas w milisekundach
    int    speed       = 1000;
    //std ::cout << "Level to: " << level << std::endl;

    if (level <= 4) {
        speed = 1000;
    }
    if (level > 4 && level <= 8) {
        speed = 800;
    }
    if (level > 8 && level <= 12)
    {
        speed = 600;
    }
    if (level > 12 && level <= 16)
    {
        speed = 400;
    }
    if (16 < level && level <= 20)
    {
        speed = 300;
    }
    if (level > 20) {
        speed = 250;
    }
    // Jeœli minê³a 1 sekunda od ostatniego przesuniêcia
    if (currentTime - lastMoveTime >= speed)
    {
        std::cout << "Predkosc gry: " << 1250 - speed << std::endl;

        if (canMove(cur, 0, 1, grid))
        {
            move(cur, 0, 1, grid); // Przesuniêcie klocka w dó³ wraz z czasem
        }
        else
        {
            placeBlock(cur, grid);      // Zablokuj klocek na planszy
            clearFullRows(grid);
            cur   = blocks[rand() % 7]; // Stwórz nowy klocek
            cur.x = 5;                  // Ustawienie startowej pozycji nowego klocka
            cur.y = 0;

            if (!canPlaceBlock(cur, grid))
            {
                running = false; // Koñczymy grê
                std::cout << "Gra skonczona! Ostateczny poziom to: " << level << std::endl;
                return;
            }
        }

        lastMoveTime = currentTime; // Zaktualizowanie czasu ostatniego przesuniêcia
    }

}

void input(const vector< vector< int > >& grid)
{
    moveUp = moveDown = moveLeft = moveRight = false;
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
            running = false;
        switch (e.type)
        {
        case SDL_KEYDOWN: // DOWN
            switch (e.key.keysym.sym)
            {
            case SDLK_LEFT:
                move(cur, -1, 0, grid); // Ruch w lewo
                break;
            case SDLK_RIGHT:
                move(cur, 1, 0, grid); // Ruch w prawo
                break;
            case SDLK_UP:
                if (canRotate(cur, grid))
                    cur = rotateMatrix(cur); // Obrót
                break;
            case SDLK_DOWN:
                move(cur, 0, 1, grid); // Ruch w dó³
                break;
            case SDLK_ESCAPE:
                running = false;
                break;
            }
        }
    }
}

void render(const vector< vector< int > >& grid)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Rysowanie wszystkich zablokowanych klocków na planszy
    for (int i = 0; i < grid.size(); i++)
    {
        for (int j = 0; j < grid[i].size(); j++)
        {
            if (grid[i][j] != 0) // Jeœli na tym miejscu jest blok
            {
                rect.x = i * TILE_SIZE;
                rect.y = j * TILE_SIZE;
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Kolor zablokowanego klocka
                SDL_RenderFillRect(renderer, &rect);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderDrawRect(renderer, &rect);
            }
        }
    }

    for (int i = 0; i < WIDTH; i += TILE_SIZE)
    {
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Szary kolor
        SDL_RenderDrawLine(renderer, i, 0, i, HEIGHT);
    }
    for (int j = 0; j < HEIGHT; j += TILE_SIZE)
    {
        SDL_RenderDrawLine(renderer, 0, j, WIDTH, j);
    }
    // Rysowanie bie¿¹cego klocka
    draw(cur);

    SDL_RenderPresent(renderer);
}

#endif 