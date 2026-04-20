#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "header.h"

#define win_w 1920
#define win_h 1080

int main(int argc, char *argv[])
{
    app app = {0};
    initialisation(&app);
    creation_joueur(&app);

    /* ── Player 1 init ── */
    app.p1.health.amount = 3;
    app.p1.state         = standing;
    app.p1.laststate     = walking_R;
    app.ticks            = 0;

    /* ── Player 2 (spawns on M key press) ── */
    app.p2_active = 0;
    app.ticks2    = 0;

    app.running = 1;

    int x = 0, y = 0;

    while (app.running)
    {
        gestion_event(&app, &x, &y);
        affichage(&app, x, y);
        SDL_Delay(16);
    }

    SDL_Quit();
    return 0;
}