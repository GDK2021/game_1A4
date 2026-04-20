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
    app->p1.last_attack_time = 0;
    app->p1.move_ticks       = 0;
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
void loadtexture(app *app, const char *framename, SDL_Texture *(*dest)[3])
{
    char filename[100];
    for (int i = 0; i < 3; i++)
    {
        sprintf(filename, "%s%d.png", framename, i + 1);

        SDL_Surface *temp = IMG_Load(filename);
        if (!temp) {
            printf("Missing file: %s\n", filename);
            SDL_Exitwitherror("Erreur chargement image");
        }

        // Use (*dest)[i] so the textures go to the right array (attack, crouch, etc.)
        (*dest)[i] = SDL_CreateTextureFromSurface(app->renderer, temp);
        
        if (i == 0)
            SDL_QueryTexture((*dest)[i], NULL, NULL, &app->p1.srcRect.w, &app->p1.srcRect.h);

        SDL_FreeSurface(temp);
    }
}
void creation_joueur(app *app)
{
    char framename[50];
    sprintf(framename,"src/player_d_");
    loadtexture(app,framename,&app->p1.tx.move_d);

    sprintf(framename,"src/player_g_");
    loadtexture(app,framename,&app->p1.tx.move_g);

    sprintf(framename,"src/crouch_d_");
    loadtexture(app,framename,&app->p1.tx.crouch_d);

    sprintf(framename,"src/crouch_g_");
    loadtexture(app,framename,&app->p1.tx.crouch_g);

    sprintf(framename,"src/attack_d_");
    loadtexture(app,framename,&app->p1.tx.attack_d);

    sprintf(framename,"src/attack_g_");
    loadtexture(app,framename,&app->p1.tx.attack_g);

   

    app->p1.srcRect.x = 0;
    app->p1.srcRect.y = 0;

    app->p1.dstRect.w = app->p1.srcRect.w;
    app->p1.dstRect.h = app->p1.srcRect.h;
    app->p1.dstRect.x = 480;
    app->p1.dstRect.y = 540;

    app->p1.state = standing;

    app->p1.score.font = TTF_OpenFont("src/ka1.ttf", 50);
    if (app->p1.score.font == NULL)
        SDL_Exitwitherror("erreur chargement police");

    app->p1.score.color.r = 0;
    app->p1.score.color.g = 255;
    app->p1.score.color.b = 255;
    app->p1.score.color.a = 255;

    app->p1.score.textSurface = TTF_RenderText_Solid(app->p1.score.font, "1235", app->p1.score.color);
    if (app->p1.score.textSurface == NULL)
        SDL_Exitwitherror("erreur creation surface de txt");

    app->p1.score.textTexture = SDL_CreateTextureFromSurface(app->renderer, app->p1.score.textSurface);

    SDL_FreeSurface(app->p1.score.textSurface);

    SDL_QueryTexture(app->p1.score.textTexture,
                     NULL,
                     NULL,
                     &app->p1.score.textPosition.w,
                     &app->p1.score.textPosition.h);

    app->p1.score.textPosition.x = 1620;
    app->p1.score.textPosition.y = 150;

    app->p1.health.surface = IMG_Load("src/fheart.png");
    if (!app->p1.health.surface)
        SDL_Exitwitherror("Erreur IMG_Load");

    app->p1.health.fheart = SDL_CreateTextureFromSurface(app->renderer, app->p1.health.surface);
    SDL_FreeSurface(app->p1.health.surface);

    app->p1.health.surface = IMG_Load("src/eheart.png");
    if (!app->p1.health.surface)
        SDL_Exitwitherror("Erreur IMG_Load");

    app->p1.health.eheart = SDL_CreateTextureFromSurface(app->renderer, app->p1.health.surface);
    SDL_FreeSurface(app->p1.health.surface);

    // Rect logic - corrected variable names

    app->p1.health.srcRect.x = 0;
    app->p1.health.srcRect.y = 0;

    app->p1.health.srcRect.w = 120;
    app->p1.health.srcRect.h = 112;

    app->p1.health.dstRect.w = 120;
    app->p1.health.dstRect.h = 120;
    app->p1.health.dstRect.x = 1470;
    app->p1.health.dstRect.y = 20;

    // Keep srcRect at 64x64 (do NOT query texture dimensions here)

    app->p1.health.amount = 1;
    app->p1.up = 0;
    app->p1.jump_x = -50.0f;
    app->p1.posy_init = 540;
}

void saut(app *app)
{
    if (app->p1.up == 1)
    {
        // Parabola coefficients from the PDF method
        // c = jump height in pixels (adjust to taste)
        // a = -c / (range/2)^2  =>  -150 / (50^2) = -0.06
        float a = -0.06f;
        float c = 150.0f;

        // Advance relative x position (speed of horizontal travel through arc)
        app->p1.jump_x += 1.5f;

        // Calculate relative y from parabola
        float rel_y = a * (app->p1.jump_x * app->p1.jump_x) + c;

        // Apply to real screen position (subtract because SDL y is inverted)
        app->p1.dstRect.y = app->p1.posy_init - (int)rel_y;

        // When relative x reaches 50 (end of arc), jump is done
        if (app->p1.jump_x >= 50.0f)
        {
            app->p1.up = 0;
            app->p1.jump_x = -50.0f;           // reset for next jump
            app->p1.dstRect.y = app->p1.posy_init; // snap back to ground
            app->p1.state = standing;
        }
    }
}

void afficher_vie(app *app)
{
    // Render full hearts using temporary rect
    SDL_Rect heartRect = app->p1.health.dstRect;
    for (int i = 0; i < app->p1.health.amount; i++)
    {
        SDL_RenderCopy(app->renderer, app->p1.health.fheart, &app->p1.health.srcRect, &heartRect);
        heartRect.x += 150;
    }

    for (int i = 0; i < 3 - app->p1.health.amount; i++)
    {
        SDL_RenderCopy(app->renderer, app->p1.health.eheart, &app->p1.health.srcRect, &heartRect);
        heartRect.x += 150;
    }
}


void gestion_event(app *app, int *x, int *y)
{
    // End attack animation after 300ms, then start cooldown
    if (app->p1.state == attacking_L || app->p1.state == attacking_R)
    {
        if (SDL_GetTicks() - (Uint32)app->p1.move_ticks >= 300)
        {
            app->p1.state            = standing;
            app->p1.last_attack_time = SDL_GetTicks();
        }
    }

    while (SDL_PollEvent(&app->event))
    {
        if (app->event.type == SDL_QUIT)
            app->running = 0;

        SDL_GetMouseState(x, y);

        // FIX 1: clean attack block with continue — no brace mismatch
        if (app->p1.state == attacking_L || app->p1.state == attacking_R)
        {
            if (app->event.type == SDL_KEYDOWN &&
                app->event.key.keysym.sym == SDLK_ESCAPE)
                app->running = 0;
            continue; // skip all other input while attacking
        }

        if (app->event.type == SDL_KEYDOWN)
        {
            if (app->event.key.keysym.sym == SDLK_ESCAPE)
                app->running = 0;

            else if (app->event.key.keysym.sym == SDLK_d)
            {
                if (app->ticks == 0) app->ticks = SDL_GetTicks();
                app->p1.state = walking_R;
            }
            else if (app->event.key.keysym.sym == SDLK_q)
            {
                if (app->ticks == 0) app->ticks = SDL_GetTicks();
                app->p1.state = walking_L;
            }
            else if (app->event.key.keysym.sym == SDLK_s)
                app->p1.state = crouching;
            else if (app->event.key.keysym.sym == SDLK_z)
            {
                if (app->p1.up == 0)
                {
                    app->p1.up        = 1;
                    app->p1.jump_x    = -50.0f;
                    app->p1.posy_init = app->p1.dstRect.y;
                    app->p1.state     = jumping;
                }
            }
            else if (app->event.key.keysym.sym == SDLK_a)
            {
                Uint32 now = SDL_GetTicks();
                // FIX 2: cast both sides to Uint32 to avoid signed/unsigned mismatch
                if (now - (Uint32)app->p1.last_attack_time >= ATTACK_COOLDOWN)
                {
                    app->p1.move_ticks = now;
                    if (app->p1.laststate == walking_R)
                        app->p1.state = attacking_R;
                    else
                        app->p1.state = attacking_L;
                }
            }
            else if (app->event.key.keysym.sym == SDLK_h)
            {
                if (app->p1.health.amount < 3) app->p1.health.amount++;
            }
            else if (app->event.key.keysym.sym == SDLK_k)
            {
                if (app->p1.health.amount > 0) app->p1.health.amount--;
            }
            else if (app->event.key.keysym.sym == SDLK_p)
            {
                FILE *f = fopen("test.txt", "w");
                if (f)
                {
                    fprintf(f, "x: %d\n", app->p1.dstRect.x);
                    fprintf(f, "y: %d\n", app->p1.dstRect.y);
                    fclose(f);
                }
            }
        }

        if (app->event.type == SDL_KEYUP)
        {
            if (app->p1.state != attacking_L && app->p1.state != attacking_R)
            {
                app->p1.laststate = app->p1.state;
                app->p1.state     = standing;
                app->ticks        = 0;
            }
        }
    }
}

void afficher_perso(app *app)
{
    int frame;
    switch (app->p1.state)
    {
    case standing:
        if (app->p1.laststate == walking_L)
            SDL_RenderCopy(app->renderer, app->p1.tx.move_g[0], &app->p1.srcRect, &app->p1.dstRect);
        else
            SDL_RenderCopy(app->renderer, app->p1.tx.move_d[0], &app->p1.srcRect, &app->p1.dstRect);
        break;

    case walking_R:
        if (app->p1.dstRect.x + app->p1.dstRect.w + 10 <= win_w)
            app->p1.dstRect.x += 10;
        frame = ((SDL_GetTicks() - app->ticks) / 100) % 3;
        SDL_RenderCopy(app->renderer, app->p1.tx.move_d[frame], &app->p1.srcRect, &app->p1.dstRect);
        break;

    case walking_L:
        if (app->p1.dstRect.x - 10 >= 0)
            app->p1.dstRect.x -= 10;
        frame = ((SDL_GetTicks() - app->ticks) / 100) % 3;
        SDL_RenderCopy(app->renderer, app->p1.tx.move_g[frame], &app->p1.srcRect, &app->p1.dstRect);
        break;

    case attacking_R:
        frame = ((SDL_GetTicks() - app->p1.move_ticks) / 100) % 3;
        SDL_RenderCopy(app->renderer, app->p1.tx.attack_d[frame], &app->p1.srcRect, &app->p1.dstRect);
        break;

    case attacking_L:
        frame = ((SDL_GetTicks() - app->p1.move_ticks) / 100) % 3;
        SDL_RenderCopy(app->renderer, app->p1.tx.attack_g[frame], &app->p1.srcRect, &app->p1.dstRect);
        break;

    // FIX 3: jumping now renders a texture
    case jumping:
        frame = (SDL_GetTicks() / 150) % 3;
        if (app->p1.laststate == walking_L)
            SDL_RenderCopy(app->renderer, app->p1.tx.move_g[frame], &app->p1.srcRect, &app->p1.dstRect);
        else
            SDL_RenderCopy(app->renderer, app->p1.tx.move_d[frame], &app->p1.srcRect, &app->p1.dstRect);
        break;

    default: break;
    }
}
void affichage(app *app, int x, int y)
{
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);
    afficher_perso(app);
    saut(app);
    // affichage score
    SDL_RenderCopy(app->renderer, app->p1.score.textTexture, NULL, &app->p1.score.textPosition);

    afficher_vie(app);

    SDL_RenderPresent(app->renderer);
}

void quitter(app *app)
{

    // app->cm=-1;
    // free_menu(app,app->cm);
    // Mix_CloseAudio();
    /*
    SDL_DestroyRenderer(app->renderer);
    SDL_DestroyWindow(app->window);
    IMG_Quit();
    SDL_Quit(); */
}

/*
void damage(player* p1, int dmg){
  p1->health-=dmg;
}
*/