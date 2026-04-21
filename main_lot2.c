#include "header_lot2.h"
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS)
 
 
int main(int argc, char *argv[]){

    app app = {0};
    srand(time(NULL));

    app.running = 1;
    app.mode = 0;
    app.niveau = 1;
    char msg_guide[150];
    
    initialisation(&app);
    creation_lot2(&app);
    initChrono(&app);
    while (app.running){
        Uint32 startTicks = SDL_GetTicks();

        gestion_evenements_score(&app, msg_guide);
        animer_plateforme(&app);
        SDL_RenderClear(app.renderer);
        afficher_lot2(&app);
        afficher_chrono(&app);
        guide_lot2(&app, msg_guide);
        SDL_RenderPresent(app.renderer);
        //FPS limite
        Uint32 frameTicks = SDL_GetTicks() - startTicks;
        if (frameTicks < FRAME_TARGET_TIME) {
            SDL_Delay(FRAME_TARGET_TIME - frameTicks);
        }
    }

    quitter_lot2(&app);
    return 0;
}