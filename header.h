#ifndef HEADER_H
#define HEADER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
} app;

typedef struct {
    SDL_Rect rect;
} Enemy;

typedef struct {
    SDL_Texture *map;
    SDL_Texture *minimap;
    SDL_Surface *mask;

    SDL_Rect player;
    SDL_Rect camera;
    SDL_Rect minimapRect;
    
    int level;
} Game;


void init(Game *g, app *a);
void loadLevel(Game *g, app *a, int level);
void updateCamera(Game *g);

int checkCollision(SDL_Rect A, SDL_Rect B);
int collisionMask(SDL_Surface *mask, SDL_Rect p);

void renderGame(app a, Game g,SDL_Rect *walls, int nWalls, Enemy *enemies, int nEnemies,SDL_Rect *movingObs, int nObs);



#endif
