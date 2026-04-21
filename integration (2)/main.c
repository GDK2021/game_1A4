#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "header.h"

#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS)

int main(int argc, char *argv[]){
    app app = {0};
    srand(time(NULL));

    app.running   = 1;
    app.mode      = 0;
    app.niveau    = 1;
    app.ticks     = 0;
    app.ticks2    = 0;
    app.p2_active = 0;

    initialisation(&app);
    creation_lot2(&app);
    initChrono(&app);
    creation_joueur(&app);

    char msg_guide[150] = "ZQSD pour bouger, A pour attaquer, Z pour sauter, C pour destruction, M pour mode 2 joueurs";

    while(app.running){
        Uint32 startTicks = SDL_GetTicks();

        gestion_evenements_score(&app, msg_guide);
        animer_plateforme(&app);

        SDL_RenderClear(app.renderer);
        afficher_lot2(&app);
        affichage(&app, 0, 0);
        afficher_chrono(&app);
        guide_lot2(&app, msg_guide);
        SDL_RenderPresent(app.renderer);

        Uint32 frameTicks = SDL_GetTicks() - startTicks;
        if(frameTicks < FRAME_TARGET_TIME)
            SDL_Delay(FRAME_TARGET_TIME - frameTicks);
    }

    quitter(&app);
    return 0;
}
