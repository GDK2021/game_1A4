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
    app.running = 1;
    
    // ==============================
    // Boucle principale
    // ==============================

    while (app.running)
    {

        // ==============================
        // Gestion des événements
        // ==============================

        gestion_event(&app);

        affichage(&app);

        SDL_Delay(16);
    }

    quitter(&app);
    
    return 0;
}
