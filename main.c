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
    app.p1.health.amount = 3;
    app.p1.state=standing;
    app.ticks = 0;
    app.p1.laststate = walking_R;
    app.running = 1;
    // Mix_PlayChannel(-1, app.son, 0);
    //  Coordonnées souris
    int x = 0, y = 0;
    // ==============================
    // Boucle principale
    // ==============================

    while (app.running)
    {

        // ==============================
        // Gestion des événements
        // ==============================

        gestion_event(&app, &x, &y);

        affichage(&app, x, y);
        SDL_Delay(16);
    }

    // quitter(&app);
    SDL_Quit();

    return 0;
}