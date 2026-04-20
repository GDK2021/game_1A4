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
#define ATTACK_COOLDOWN 500
/*
menu principal 0
menu option    1
menu joueur    2/3
menu quiz      4/5
menu score     6
menu save      7/8
*/

void SDL_Exitwitherror(const char *msg)
{
    SDL_Log("erreur : %s > %s\n", msg, SDL_GetError());
    SDL_Quit();
    exit(EXIT_FAILURE);
}

void initialisation(app *app)
{
    // Basic SDL Init

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        SDL_Exitwitherror("SDL_Init fail");
    }

    // Mixer Init
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
        SDL_Exitwitherror("initialisation son echouee");

    if (TTF_Init() == -1)
        SDL_Exitwitherror("initialisation du text echouee ");

    // Logic control
    app->running = 1;

    srand(time(NULL));

    // Window Creation
    app->window = SDL_CreateWindow(
        "Atelier SDL2 - Scenario 1",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        win_w, win_h,
        SDL_WINDOW_FULLSCREEN);

    if (!app->window)
        SDL_Exitwitherror("Erreur SDL_CreateWindow");

    // Renderer Creation - Use app->window, not 'window'
    app->renderer = SDL_CreateRenderer(app->window, -1, SDL_RENDERER_ACCELERATED);
    if (!app->renderer)
        SDL_Exitwitherror("Erreur Renderer");

    // Image Init
    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG)))
        SDL_Exitwitherror("Erreur image");
}

void gestion_event(app *app)
{
    int attacking = (app->p1.state == attacking_L || app->p1.state == attacking_R);

    if (attacking)
    {
        if (SDL_GetTicks() - app->p1.move_ticks >= 300)
        {
            app->p1.state = standing;
            app->p1.last_attack_time = SDL_GetTicks();
        }
    }

    while (SDL_PollEvent(&app->event))
    {
        if (app->event.type == SDL_QUIT)
            app->running = 0;

        if (app->event.type == SDL_KEYDOWN)
        {
            if (app->event.key.keysym.sym == SDLK_g)
            {
                lancer_enigme(app);
            }
        }
    }
}




void affichage(app *app)
{
return;
}







void quitter(app *app){
    return;
}