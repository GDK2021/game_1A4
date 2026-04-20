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

/* ─────────────────────────────────────────────
   Error helper
───────────────────────────────────────────── */
void SDL_Exitwitherror(const char *msg)
{
    SDL_Log("erreur : %s > %s\n", msg, SDL_GetError());
    SDL_Quit();
    exit(EXIT_FAILURE);
}

/* ─────────────────────────────────────────────
   Initialisation
───────────────────────────────────────────── */
void initialisation(app *app)
{
    app->p1.last_attack_time = 0;
    app->p1.move_ticks       = 0;
    app->p2.last_attack_time = 0;
    app->p2.move_ticks       = 0;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
        SDL_Exitwitherror("SDL_Init fail");

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
        SDL_Exitwitherror("initialisation son echouee");

    if (TTF_Init() == -1)
        SDL_Exitwitherror("initialisation du text echouee ");

    app->running = 1;
    srand(time(NULL));

    app->window = SDL_CreateWindow(
        "Atelier SDL2 - Scenario 1",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        win_w, win_h,
        SDL_WINDOW_FULLSCREEN);

    if (!app->window)
        SDL_Exitwitherror("Erreur SDL_CreateWindow");

    app->renderer = SDL_CreateRenderer(app->window, -1, SDL_RENDERER_ACCELERATED);
    if (!app->renderer)
        SDL_Exitwitherror("Erreur Renderer");

    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG)))
        SDL_Exitwitherror("Erreur image");
}

/* ─────────────────────────────────────────────
   Texture loaders  (p1 / p2 variants)
───────────────────────────────────────────── */
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
        (*dest)[i] = SDL_CreateTextureFromSurface(app->renderer, temp);
        if (i == 0)
            SDL_QueryTexture((*dest)[i], NULL, NULL,
                             &app->p1.srcRect.w, &app->p1.srcRect.h);
        SDL_FreeSurface(temp);
    }
}

/* p2 version – writes dimensions into p2.srcRect */
void loadtexture2(app *app, const char *framename, SDL_Texture *(*dest)[3])
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
        (*dest)[i] = SDL_CreateTextureFromSurface(app->renderer, temp);
        if (i == 0)
            SDL_QueryTexture((*dest)[i], NULL, NULL,
                             &app->p2.srcRect.w, &app->p2.srcRect.h);
        SDL_FreeSurface(temp);
    }
}

/* ─────────────────────────────────────────────
   Player 1 creation
───────────────────────────────────────────── */
void creation_joueur(app *app)
{
    char framename[50];
    sprintf(framename, "src/player_d_");  loadtexture(app, framename, &app->p1.tx.move_d);
    sprintf(framename, "src/player_g_");  loadtexture(app, framename, &app->p1.tx.move_g);
    sprintf(framename, "src/crouch_d_");  loadtexture(app, framename, &app->p1.tx.crouch_d);
    sprintf(framename, "src/crouch_g_");  loadtexture(app, framename, &app->p1.tx.crouch_g);
    sprintf(framename, "src/attack_d_");  loadtexture(app, framename, &app->p1.tx.attack_d);
    sprintf(framename, "src/attack_g_");  loadtexture(app, framename, &app->p1.tx.attack_g);

    app->p1.srcRect.x = 0;
    app->p1.srcRect.y = 0;

    app->p1.dstRect.w = app->p1.srcRect.w;
    app->p1.dstRect.h = app->p1.srcRect.h;
    app->p1.dstRect.x = 480;
    app->p1.dstRect.y = 540;

    app->p1.state = standing;

    /* Score */
    app->p1.score.font = TTF_OpenFont("src/ka1.ttf", 50);
    if (!app->p1.score.font)
        SDL_Exitwitherror("erreur chargement police");

    app->p1.score.color = (SDL_Color){0, 255, 255, 255};

    app->p1.score.textSurface = TTF_RenderText_Solid(
        app->p1.score.font, "1235", app->p1.score.color);
    if (!app->p1.score.textSurface)
        SDL_Exitwitherror("erreur creation surface de txt");

    app->p1.score.textTexture =
        SDL_CreateTextureFromSurface(app->renderer, app->p1.score.textSurface);
    SDL_FreeSurface(app->p1.score.textSurface);

    SDL_QueryTexture(app->p1.score.textTexture, NULL, NULL,
                     &app->p1.score.textPosition.w, &app->p1.score.textPosition.h);

    app->p1.score.textPosition.x = 1620;
    app->p1.score.textPosition.y = 150;

    /* Health */
    app->p1.health.surface = IMG_Load("src/fheart.png");
    if (!app->p1.health.surface) SDL_Exitwitherror("Erreur IMG_Load fheart p1");
    app->p1.health.fheart = SDL_CreateTextureFromSurface(app->renderer, app->p1.health.surface);
    SDL_FreeSurface(app->p1.health.surface);

    app->p1.health.surface = IMG_Load("src/eheart.png");
    if (!app->p1.health.surface) SDL_Exitwitherror("Erreur IMG_Load eheart p1");
    app->p1.health.eheart = SDL_CreateTextureFromSurface(app->renderer, app->p1.health.surface);
    SDL_FreeSurface(app->p1.health.surface);

    app->p1.health.srcRect = (SDL_Rect){0, 0, 120, 112};
    app->p1.health.dstRect = (SDL_Rect){1470, 20, 120, 120};
    app->p1.health.amount  = 3;

    app->p1.up        = 0;
    app->p1.jump_x    = -50.0f;
    app->p1.posy_init = 540;
}

/* ─────────────────────────────────────────────
   Player 2 creation  (reuses p1 textures, 
   spawns on the right side, tinted blue)
───────────────────────────────────────────── */
void creation_joueur2(app *app)
{
    /* Reuse p1 texture pointers – no extra memory needed */
    app->p2.tx = app->p1.tx;

    app->p2.srcRect = app->p1.srcRect;

    app->p2.dstRect.w = app->p2.srcRect.w;
    app->p2.dstRect.h = app->p2.srcRect.h;
    app->p2.dstRect.x = 1200;   /* spawn on the right */
    app->p2.dstRect.y = 540;

    app->p2.state     = standing;
    app->p2.laststate = walking_L;   /* faces left (toward p1) */

    /* Score */
    app->p2.score.font = TTF_OpenFont("src/ka1.ttf", 50);
    if (!app->p2.score.font)
        SDL_Exitwitherror("erreur chargement police p2");

    app->p2.score.color = (SDL_Color){255, 100, 100, 255};  /* red tint */

    app->p2.score.textSurface = TTF_RenderText_Solid(
        app->p2.score.font, "0000", app->p2.score.color);
    if (!app->p2.score.textSurface)
        SDL_Exitwitherror("erreur surface txt p2");

    app->p2.score.textTexture =
        SDL_CreateTextureFromSurface(app->renderer, app->p2.score.textSurface);
    SDL_FreeSurface(app->p2.score.textSurface);

    SDL_QueryTexture(app->p2.score.textTexture, NULL, NULL,
                     &app->p2.score.textPosition.w, &app->p2.score.textPosition.h);

    app->p2.score.textPosition.x = 200;
    app->p2.score.textPosition.y = 150;

    /* Health – reuse same textures as p1 */
    app->p2.health.fheart = app->p1.health.fheart;
    app->p2.health.eheart = app->p1.health.eheart;
    app->p2.health.srcRect = (SDL_Rect){0, 0, 120, 112};
    app->p2.health.dstRect = (SDL_Rect){20, 20, 120, 120};
    app->p2.health.amount  = 3;

    app->p2.up        = 0;
    app->p2.jump_x    = -50.0f;
    app->p2.posy_init = 540;

    app->p2.last_attack_time = 0;
    app->p2.move_ticks       = 0;
}

/* ─────────────────────────────────────────────
   Jump physics  (one per player)
───────────────────────────────────────────── */
void saut(app *app)
{
    if (app->p1.up == 1)
    {
        float a = -0.06f, c = 150.0f;
        app->p1.jump_x += 1.5f;
        float rel_y = a * (app->p1.jump_x * app->p1.jump_x) + c;
        app->p1.dstRect.y = app->p1.posy_init - (int)rel_y;
        if (app->p1.jump_x >= 50.0f)
        {
            app->p1.up        = 0;
            app->p1.jump_x    = -50.0f;
            app->p1.dstRect.y = app->p1.posy_init;
            app->p1.state     = standing;
        }
    }
}

void saut2(app *app)
{
    if (app->p2.up == 1)
    {
        float a = -0.06f, c = 150.0f;
        app->p2.jump_x += 1.5f;
        float rel_y = a * (app->p2.jump_x * app->p2.jump_x) + c;
        app->p2.dstRect.y = app->p2.posy_init - (int)rel_y;
        if (app->p2.jump_x >= 50.0f)
        {
            app->p2.up        = 0;
            app->p2.jump_x    = -50.0f;
            app->p2.dstRect.y = app->p2.posy_init;
            app->p2.state     = standing;
        }
    }
}

/* ─────────────────────────────────────────────
   Health rendering
───────────────────────────────────────────── */
void afficher_vie(app *app)
{
    SDL_Rect heartRect = app->p1.health.dstRect;
    for (int i = 0; i < app->p1.health.amount; i++)
    {
        SDL_RenderCopy(app->renderer, app->p1.health.fheart,
                       &app->p1.health.srcRect, &heartRect);
        heartRect.x += 150;
    }
    for (int i = 0; i < 3 - app->p1.health.amount; i++)
    {
        SDL_RenderCopy(app->renderer, app->p1.health.eheart,
                       &app->p1.health.srcRect, &heartRect);
        heartRect.x += 150;
    }
}

void afficher_vie2(app *app)
{
    SDL_Rect heartRect = app->p2.health.dstRect;
    for (int i = 0; i < app->p2.health.amount; i++)
    {
        SDL_RenderCopy(app->renderer, app->p2.health.fheart,
                       &app->p2.health.srcRect, &heartRect);
        heartRect.x += 150;
    }
    for (int i = 0; i < 3 - app->p2.health.amount; i++)
    {
        SDL_RenderCopy(app->renderer, app->p2.health.eheart,
                       &app->p2.health.srcRect, &heartRect);
        heartRect.x += 150;
    }
}

/* ─────────────────────────────────────────────
   Character rendering
───────────────────────────────────────────── */
void afficher_perso(app *app)
{
    int frame;
    switch (app->p1.state)
    {
    case standing:
        if (app->p1.laststate == walking_L)
            SDL_RenderCopy(app->renderer, app->p1.tx.move_g[0],
                           &app->p1.srcRect, &app->p1.dstRect);
        else
            SDL_RenderCopy(app->renderer, app->p1.tx.move_d[0],
                           &app->p1.srcRect, &app->p1.dstRect);
        break;

    case walking_R:
        if (app->p1.dstRect.x + app->p1.dstRect.w + 10 <= win_w)
            app->p1.dstRect.x += 10;
        frame = ((SDL_GetTicks() - app->ticks) / 100) % 3;
        SDL_RenderCopy(app->renderer, app->p1.tx.move_d[frame],
                       &app->p1.srcRect, &app->p1.dstRect);
        break;

    case walking_L:
        if (app->p1.dstRect.x - 10 >= 0)
            app->p1.dstRect.x -= 10;
        frame = ((SDL_GetTicks() - app->ticks) / 100) % 3;
        SDL_RenderCopy(app->renderer, app->p1.tx.move_g[frame],
                       &app->p1.srcRect, &app->p1.dstRect);
        break;

    case attacking_R:
        frame = ((SDL_GetTicks() - app->p1.move_ticks) / 100) % 3;
        SDL_RenderCopy(app->renderer, app->p1.tx.attack_d[frame],
                       &app->p1.srcRect, &app->p1.dstRect);
        break;

    case attacking_L:
        frame = ((SDL_GetTicks() - app->p1.move_ticks) / 100) % 3;
        SDL_RenderCopy(app->renderer, app->p1.tx.attack_g[frame],
                       &app->p1.srcRect, &app->p1.dstRect);
        break;

    case jumping:
        frame = (SDL_GetTicks() / 150) % 3;
        if (app->p1.laststate == walking_L)
            SDL_RenderCopy(app->renderer, app->p1.tx.move_g[frame],
                           &app->p1.srcRect, &app->p1.dstRect);
        else
            SDL_RenderCopy(app->renderer, app->p1.tx.move_d[frame],
                           &app->p1.srcRect, &app->p1.dstRect);
        break;

    default: break;
    }
}

/* Player 2 – rendered with a blue colour mod to distinguish visually */
void afficher_perso2(app *app)
{
    if (!app->p2_active) return;

    int frame;
    /* Apply blue tint to all p2 renders */
    SDL_SetTextureColorMod(app->p2.tx.move_d[0],   100, 100, 255);
    SDL_SetTextureColorMod(app->p2.tx.move_d[1],   100, 100, 255);
    SDL_SetTextureColorMod(app->p2.tx.move_d[2],   100, 100, 255);
    SDL_SetTextureColorMod(app->p2.tx.move_g[0],   100, 100, 255);
    SDL_SetTextureColorMod(app->p2.tx.move_g[1],   100, 100, 255);
    SDL_SetTextureColorMod(app->p2.tx.move_g[2],   100, 100, 255);
    SDL_SetTextureColorMod(app->p2.tx.attack_d[0], 100, 100, 255);
    SDL_SetTextureColorMod(app->p2.tx.attack_d[1], 100, 100, 255);
    SDL_SetTextureColorMod(app->p2.tx.attack_d[2], 100, 100, 255);
    SDL_SetTextureColorMod(app->p2.tx.attack_g[0], 100, 100, 255);
    SDL_SetTextureColorMod(app->p2.tx.attack_g[1], 100, 100, 255);
    SDL_SetTextureColorMod(app->p2.tx.attack_g[2], 100, 100, 255);

    switch (app->p2.state)
    {
    case standing:
        if (app->p2.laststate == walking_L)
            SDL_RenderCopy(app->renderer, app->p2.tx.move_g[0],
                           &app->p2.srcRect, &app->p2.dstRect);
        else
            SDL_RenderCopy(app->renderer, app->p2.tx.move_d[0],
                           &app->p2.srcRect, &app->p2.dstRect);
        break;

    case walking_R:
        if (app->p2.dstRect.x + app->p2.dstRect.w + 10 <= win_w)
            app->p2.dstRect.x += 10;
        frame = ((SDL_GetTicks() - app->ticks2) / 100) % 3;
        SDL_RenderCopy(app->renderer, app->p2.tx.move_d[frame],
                       &app->p2.srcRect, &app->p2.dstRect);
        break;

    case walking_L:
        if (app->p2.dstRect.x - 10 >= 0)
            app->p2.dstRect.x -= 10;
        frame = ((SDL_GetTicks() - app->ticks2) / 100) % 3;
        SDL_RenderCopy(app->renderer, app->p2.tx.move_g[frame],
                       &app->p2.srcRect, &app->p2.dstRect);
        break;

    case attacking_R:
        frame = ((SDL_GetTicks() - app->p2.move_ticks) / 100) % 3;
        SDL_RenderCopy(app->renderer, app->p2.tx.attack_d[frame],
                       &app->p2.srcRect, &app->p2.dstRect);
        break;

    case attacking_L:
        frame = ((SDL_GetTicks() - app->p2.move_ticks) / 100) % 3;
        SDL_RenderCopy(app->renderer, app->p2.tx.attack_g[frame],
                       &app->p2.srcRect, &app->p2.dstRect);
        break;

    case jumping:
        frame = (SDL_GetTicks() / 150) % 3;
        if (app->p2.laststate == walking_L)
            SDL_RenderCopy(app->renderer, app->p2.tx.move_g[frame],
                           &app->p2.srcRect, &app->p2.dstRect);
        else
            SDL_RenderCopy(app->renderer, app->p2.tx.move_d[frame],
                           &app->p2.srcRect, &app->p2.dstRect);
        break;

    default: break;
    }

    /* Reset colour mod so p1 textures are unaffected */
    SDL_SetTextureColorMod(app->p2.tx.move_d[0],   255, 255, 255);
    SDL_SetTextureColorMod(app->p2.tx.move_d[1],   255, 255, 255);
    SDL_SetTextureColorMod(app->p2.tx.move_d[2],   255, 255, 255);
    SDL_SetTextureColorMod(app->p2.tx.move_g[0],   255, 255, 255);
    SDL_SetTextureColorMod(app->p2.tx.move_g[1],   255, 255, 255);
    SDL_SetTextureColorMod(app->p2.tx.move_g[2],   255, 255, 255);
    SDL_SetTextureColorMod(app->p2.tx.attack_d[0], 255, 255, 255);
    SDL_SetTextureColorMod(app->p2.tx.attack_d[1], 255, 255, 255);
    SDL_SetTextureColorMod(app->p2.tx.attack_d[2], 255, 255, 255);
    SDL_SetTextureColorMod(app->p2.tx.attack_g[0], 255, 255, 255);
    SDL_SetTextureColorMod(app->p2.tx.attack_g[1], 255, 255, 255);
    SDL_SetTextureColorMod(app->p2.tx.attack_g[2], 255, 255, 255);
}

/* ─────────────────────────────────────────────
   Event handling
───────────────────────────────────────────── */
void gestion_event(app *app, int *x, int *y)
{
    /* ── P1: end attack animation ── */
    if (app->p1.state == attacking_L || app->p1.state == attacking_R)
    {
        if (SDL_GetTicks() - (Uint32)app->p1.move_ticks >= 300)
        {
            app->p1.state            = standing;
            app->p1.last_attack_time = SDL_GetTicks();
        }
    }

    /* ── P2: end attack animation ── */
    if (app->p2_active &&
        (app->p2.state == attacking_L || app->p2.state == attacking_R))
    {
        if (SDL_GetTicks() - (Uint32)app->p2.move_ticks >= 300)
        {
            app->p2.state            = standing;
            app->p2.last_attack_time = SDL_GetTicks();
        }
    }

    while (SDL_PollEvent(&app->event))
    {
        if (app->event.type == SDL_QUIT)
            app->running = 0;

        SDL_GetMouseState(x, y);

        /* ───── KEYDOWN ───── */
        if (app->event.type == SDL_KEYDOWN)
        {
            SDL_Keycode sym = app->event.key.keysym.sym;

            /* Global: ESC quits */
            if (sym == SDLK_ESCAPE)
            {
                app->running = 0;
                continue;
            }

            /* ── Spawn P2 on M ── */
            if (sym == SDLK_m && !app->p2_active)
            {
                creation_joueur2(app);
                app->p2_active = 1;
                continue;
            }

            /* ════ Player 1 controls (WASD / A to attack) ════ */
            if (app->p1.state != attacking_L && app->p1.state != attacking_R)
            {
                if (sym == SDLK_d)
                {
                    if (app->ticks == 0) app->ticks = SDL_GetTicks();
                    app->p1.state = walking_R;
                }
                else if (sym == SDLK_q)
                {
                    if (app->ticks == 0) app->ticks = SDL_GetTicks();
                    app->p1.state = walking_L;
                }
                else if (sym == SDLK_s)
                    app->p1.state = crouching;
                else if (sym == SDLK_z)
                {
                    if (app->p1.up == 0)
                    {
                        app->p1.up        = 1;
                        app->p1.jump_x    = -50.0f;
                        app->p1.posy_init = app->p1.dstRect.y;
                        app->p1.state     = jumping;
                    }
                }
                else if (sym == SDLK_a)
                {
                    Uint32 now = SDL_GetTicks();
                    if (now - (Uint32)app->p1.last_attack_time >= ATTACK_COOLDOWN)
                    {
                        app->p1.move_ticks = now;
                        app->p1.state = (app->p1.laststate == walking_R)
                                        ? attacking_R : attacking_L;
                    }
                }
                /* Debug helpers */
                else if (sym == SDLK_h)
                {
                    if (app->p1.health.amount < 3) app->p1.health.amount++;
                }
                else if (sym == SDLK_k)
                {
                    if (app->p1.health.amount > 0) app->p1.health.amount--;
                }
                else if (sym == SDLK_p)
                {
                    FILE *f = fopen("test.txt", "w");
                    if (f) {
                        fprintf(f, "x: %d\n", app->p1.dstRect.x);
                        fprintf(f, "y: %d\n", app->p1.dstRect.y);
                        fclose(f);
                    }
                }
            }

            /* ════ Player 2 controls (arrow keys + K to attack) ════ */
            if (app->p2_active &&
                app->p2.state != attacking_L && app->p2.state != attacking_R)
            {
                if (sym == SDLK_RIGHT)
                {
                    if (app->ticks2 == 0) app->ticks2 = SDL_GetTicks();
                    app->p2.state = walking_R;
                }
                else if (sym == SDLK_LEFT)
                {
                    if (app->ticks2 == 0) app->ticks2 = SDL_GetTicks();
                    app->p2.state = walking_L;
                }
                else if (sym == SDLK_DOWN)
                    app->p2.state = crouching;
                else if (sym == SDLK_UP)
                {
                    if (app->p2.up == 0)
                    {
                        app->p2.up        = 1;
                        app->p2.jump_x    = -50.0f;
                        app->p2.posy_init = app->p2.dstRect.y;
                        app->p2.state     = jumping;
                    }
                }
                else if (sym == SDLK_k)
                {
                    Uint32 now = SDL_GetTicks();
                    if (now - (Uint32)app->p2.last_attack_time >= ATTACK_COOLDOWN)
                    {
                        app->p2.move_ticks = now;
                        app->p2.state = (app->p2.laststate == walking_R)
                                        ? attacking_R : attacking_L;
                    }
                }
            }
        } /* end KEYDOWN */

        /* ───── KEYUP ───── */
        if (app->event.type == SDL_KEYUP)
        {
            SDL_Keycode sym = app->event.key.keysym.sym;

            /* P1 key release */
            if ((sym == SDLK_d || sym == SDLK_q || sym == SDLK_s || sym == SDLK_z) &&
                app->p1.state != attacking_L && app->p1.state != attacking_R)
            {
                app->p1.laststate = app->p1.state;
                app->p1.state     = standing;
                app->ticks        = 0;
            }

            /* P2 key release */
            if (app->p2_active &&
                (sym == SDLK_RIGHT || sym == SDLK_LEFT ||
                 sym == SDLK_DOWN  || sym == SDLK_UP) &&
                app->p2.state != attacking_L && app->p2.state != attacking_R)
            {
                app->p2.laststate = app->p2.state;
                app->p2.state     = standing;
                app->ticks2       = 0;
            }
        }
    } /* end event loop */
}

/* ─────────────────────────────────────────────
   Main render
───────────────────────────────────────────── */
void affichage(app *app, int x, int y)
{
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);

    afficher_perso(app);
    if (app->p2_active) afficher_perso2(app);

    saut(app);
    if (app->p2_active) saut2(app);

    /* Scores */
    SDL_RenderCopy(app->renderer, app->p1.score.textTexture,
                   NULL, &app->p1.score.textPosition);
    if (app->p2_active)
        SDL_RenderCopy(app->renderer, app->p2.score.textTexture,
                       NULL, &app->p2.score.textPosition);

    afficher_vie(app);
    if (app->p2_active) afficher_vie2(app);

    SDL_RenderPresent(app->renderer);
}

/* ─────────────────────────────────────────────
   Cleanup
───────────────────────────────────────────── */
void quitter(app *app)
{
    /* extend as needed */
}