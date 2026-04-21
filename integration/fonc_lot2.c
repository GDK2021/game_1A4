#include "header.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <string.h>
#include <time.h>

#define fen_w 1920
#define fen_h 1080
#define bg_w  5000
#define bg_h  2500
#define plt_w 100
#define plt_h 40
#define SCROLL_BORDER 100

void sdl_erreur(const char *msg, const char *msgr){
    printf("Erreur: %s  %s\n", msg, msgr);
    SDL_Quit();
}

void initialisation(app *a){
    a->p1.last_attack_time = 0;
    a->p1.move_ticks       = 0;
    a->p2.last_attack_time = 0;
    a->p2.move_ticks       = 0;

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0){
        sdl_erreur("SDL_Init", SDL_GetError()); quitter(a);
    }
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
        sdl_erreur("Mix_OpenAudio", Mix_GetError()); quitter(a);
    }
    if(TTF_Init() == -1){
        sdl_erreur("TTF_Init", TTF_GetError()); quitter(a);
    }
    if(!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) & (IMG_INIT_PNG | IMG_INIT_JPG))){
        sdl_erreur("IMG_Init", IMG_GetError()); quitter(a);
    }
    a->window = SDL_CreateWindow("Cold Revenge",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        fen_w, fen_h, SDL_WINDOW_FULLSCREEN);
    if(!a->window){ sdl_erreur("CreateWindow", SDL_GetError()); quitter(a); }

    a->renderer = SDL_CreateRenderer(a->window, -1, SDL_RENDERER_ACCELERATED);
    if(!a->renderer){ sdl_erreur("CreateRenderer", SDL_GetError()); quitter(a); }
}

void creation_lot2(app *a){
    a->bg.img.surface = IMG_Load("src/images/bg_jeu.png");
    if(!a->bg.img.surface) sdl_erreur("bg_jeu.png", IMG_GetError());
    a->bg.img.texture = SDL_CreateTextureFromSurface(a->renderer, a->bg.img.surface);
    if(!a->bg.img.texture) sdl_erreur("texture bg", SDL_GetError());
    SDL_FreeSurface(a->bg.img.surface);

    a->bg.cam1.x = 0; a->bg.cam1.y = bg_h - fen_h;
    a->bg.cam1.h = fen_h; a->bg.cam1.w = fen_w;
    a->bg.cam2.x = 0; a->bg.cam2.y = bg_h - fen_h;
    a->bg.cam2.h = fen_h; a->bg.cam2.w = fen_w / 2;

    a->bg.g.afficher = 0;
    a->bg.g.instruction.font = TTF_OpenFont("src/font/arial.ttf", 45);
    if(!a->bg.g.instruction.font) sdl_erreur("font guide", TTF_GetError());
    a->bg.g.instruction.color = (SDL_Color){255, 255, 255, 255};
    a->bg.g.cadre.w = 600; a->bg.g.cadre.h = 100;
    a->bg.g.cadre.x = (fen_w / 2) - 300;
    a->bg.g.cadre.y = (fen_h / 2) - 50;

    SDL_Surface *img_plt   = IMG_Load("src/images/plateforme.png");
    SDL_Surface *img_plt_d = IMG_Load("src/images/plateforme_d.png");
    if(!img_plt || !img_plt_d) sdl_erreur("plateforme.png", IMG_GetError());
    SDL_Texture *tex_plt   = SDL_CreateTextureFromSurface(a->renderer, img_plt);
    SDL_Texture *tex_plt_d = SDL_CreateTextureFromSurface(a->renderer, img_plt_d);
    if(!tex_plt || !tex_plt_d) sdl_erreur("texture plateforme", SDL_GetError());
    SDL_FreeSurface(img_plt);
    SDL_FreeSurface(img_plt_d);

    a->bg.nbplt = (a->niveau == 1) ? 40 : 45;

    for(int i = 0; i < a->bg.nbplt; i++){
        a->bg.plt[i].pos.h = plt_h;
        a->bg.plt[i].pos.w = plt_w;
        a->bg.plt[i].pos.x = rand() % (bg_w - plt_w);
        a->bg.plt[i].pos.y = (bg_h - fen_h) + rand() % fen_h;
        a->bg.plt[i].direction = i % 2;
        a->bg.plt[i].active = 1;
        if(i < 20){
            a->bg.plt[i].texture = tex_plt;
            a->bg.plt[i].type    = 0;
        } else if(i < 35){
            a->bg.plt[i].texture  = tex_plt;
            a->bg.plt[i].type     = 1;
            a->bg.plt[i].pos_init = a->bg.plt[i].pos.x;
        } else {
            a->bg.plt[i].texture = tex_plt_d;
            a->bg.plt[i].type    = 2;
        }
    }
}

void afficher_lot2(app *a){
    if(a->mode == 0){
        SDL_RenderSetViewport(a->renderer, NULL);
        SDL_RenderCopy(a->renderer, a->bg.img.texture, &a->bg.cam1, NULL);
        for(int i = 0; i < a->bg.nbplt; i++){
            if(!a->bg.plt[i].active) continue;
            SDL_Rect d = {
                a->bg.plt[i].pos.x - a->bg.cam1.x,
                a->bg.plt[i].pos.y - a->bg.cam1.y,
                plt_w, plt_h
            };
            if(d.x + plt_w < 0 || d.x > fen_w) continue;
            if(d.y + plt_h < 0 || d.y > fen_h) continue;
            SDL_RenderCopy(a->renderer, a->bg.plt[i].texture, NULL, &d);
        }
    } else {
        SDL_Rect vg = {0,         0, fen_w/2, fen_h};
        SDL_Rect vd = {fen_w / 2, 0, fen_w/2, fen_h};

        SDL_RenderSetViewport(a->renderer, &vg);
        SDL_RenderCopy(a->renderer, a->bg.img.texture, &a->bg.cam1, NULL);
        for(int i = 0; i < a->bg.nbplt; i++){
            if(!a->bg.plt[i].active) continue;
            SDL_Rect d = {
                a->bg.plt[i].pos.x - a->bg.cam1.x,
                a->bg.plt[i].pos.y - a->bg.cam1.y,
                plt_w, plt_h
            };
            if(d.x + plt_w < 0 || d.x > fen_w/2) continue;
            if(d.y + plt_h < 0 || d.y > fen_h)   continue;
            SDL_RenderCopy(a->renderer, a->bg.plt[i].texture, NULL, &d);
        }

        SDL_RenderSetViewport(a->renderer, &vd);
        SDL_RenderCopy(a->renderer, a->bg.img.texture, &a->bg.cam2, NULL);
        for(int i = 0; i < a->bg.nbplt; i++){
            if(!a->bg.plt[i].active) continue;
            SDL_Rect d = {
                a->bg.plt[i].pos.x - a->bg.cam2.x,
                a->bg.plt[i].pos.y - a->bg.cam2.y,
                plt_w, plt_h
            };
            if(d.x + plt_w < 0 || d.x > fen_w/2) continue;
            if(d.y + plt_h < 0 || d.y > fen_h)   continue;
            SDL_RenderCopy(a->renderer, a->bg.plt[i].texture, NULL, &d);
        }

        SDL_RenderSetViewport(a->renderer, NULL);
    }
}

void scrolling(app *a, int direction, int pas, int jouer){
    SDL_Rect *cam = (jouer == 1) ? &a->bg.cam1 : (jouer == 2) ? &a->bg.cam2 : NULL;
    if(!cam) return;
    switch(direction){
    case 1: cam->x += pas; if(cam->x > bg_w - cam->w) cam->x = bg_w - cam->w; break;
    case 2: cam->x -= pas; if(cam->x < 0)              cam->x = 0;             break;
    case 3: cam->y -= pas; if(cam->y < 0)              cam->y = 0;             break;
    case 4: cam->y += pas; if(cam->y > bg_h - cam->h)  cam->y = bg_h - cam->h; break;
    }
}

void sync_cam_y(app *a, int jouer){
    SDL_Rect *cam;
    int py, posy_init;
    if(jouer == 1){
        cam       = &a->bg.cam1;
        py        = a->p1.dstRect.y;
        posy_init = a->p1.posy_init;
    } else {
        cam       = &a->bg.cam2;
        py        = a->p2.dstRect.y;
        posy_init = a->p2.posy_init;
    }

    int sol_cam_y = bg_h - fen_h;
    int delta     = posy_init - py;

    cam->y = sol_cam_y - delta;

    if(cam->y < 0)             cam->y = 0;
    if(cam->y > bg_h - cam->h) cam->y = bg_h - cam->h;
}

void initChrono(app *a){
    a->bg.c.tmp_debut  = SDL_GetTicks();
    a->bg.c.tmp_actuel = 0;
    a->bg.c.t_chrono.font = TTF_OpenFont("src/font/arial.ttf", 25);
    if(!a->bg.c.t_chrono.font) sdl_erreur("font chrono", TTF_GetError());
    a->bg.c.t_chrono.color = (SDL_Color){0, 0, 0, 255};
    a->bg.c.t_chrono.textPosition.x = fen_w - 150;
    a->bg.c.t_chrono.textPosition.y = fen_h - 50;
}

void afficher_chrono(app *a){
    a->bg.c.tmp_actuel = SDL_GetTicks() - a->bg.c.tmp_debut;
    int total_sec = a->bg.c.tmp_actuel / 1000;
    char tmp_txt[20];
    sprintf(tmp_txt, "%02d:%02d", total_sec / 60, total_sec % 60);

    a->bg.c.t_chrono.textSurface = TTF_RenderText_Blended(
        a->bg.c.t_chrono.font, tmp_txt, a->bg.c.t_chrono.color);
    if(!a->bg.c.t_chrono.textSurface) sdl_erreur("surface chrono", TTF_GetError());
    a->bg.c.t_chrono.textTexture = SDL_CreateTextureFromSurface(
        a->renderer, a->bg.c.t_chrono.textSurface);
    if(!a->bg.c.t_chrono.textTexture) sdl_erreur("texture chrono", SDL_GetError());
    SDL_FreeSurface(a->bg.c.t_chrono.textSurface);
    SDL_QueryTexture(a->bg.c.t_chrono.textTexture, NULL, NULL,
        &a->bg.c.t_chrono.textPosition.w, &a->bg.c.t_chrono.textPosition.h);
    a->bg.c.t_chrono.textPosition.x = fen_w - a->bg.c.t_chrono.textPosition.w - 20;
    a->bg.c.t_chrono.textPosition.y = fen_h - a->bg.c.t_chrono.textPosition.h - 10;
    SDL_RenderCopy(a->renderer, a->bg.c.t_chrono.textTexture,
        NULL, &a->bg.c.t_chrono.textPosition);
    SDL_DestroyTexture(a->bg.c.t_chrono.textTexture);
}

void guide_lot2(app *a, char *msg_guide){
    if(a->event.type == SDL_KEYDOWN && a->event.key.keysym.sym == SDLK_g)
        a->bg.g.afficher = !a->bg.g.afficher;
    if(!a->bg.g.afficher) return;

    a->bg.g.instruction.textSurface = TTF_RenderText_Blended_Wrapped(
        a->bg.g.instruction.font, msg_guide, a->bg.g.instruction.color, 1880);
    if(!a->bg.g.instruction.textSurface) sdl_erreur("surface guide", TTF_GetError());
    a->bg.g.instruction.textTexture = SDL_CreateTextureFromSurface(
        a->renderer, a->bg.g.instruction.textSurface);
    if(!a->bg.g.instruction.textTexture) sdl_erreur("texture guide", SDL_GetError());
    SDL_FreeSurface(a->bg.g.instruction.textSurface);
    SDL_QueryTexture(a->bg.g.instruction.textTexture, NULL, NULL,
        &a->bg.g.instruction.textPosition.w, &a->bg.g.instruction.textPosition.h);
    a->bg.g.instruction.textPosition.x = (fen_w/2) - (a->bg.g.instruction.textPosition.w/2);
    a->bg.g.instruction.textPosition.y = 18;
    a->bg.g.cadre.h = a->bg.g.instruction.textPosition.h + 20;
    a->bg.g.cadre.w = a->bg.g.instruction.textPosition.w + 20;
    a->bg.g.cadre.x = a->bg.g.instruction.textPosition.x - 15;
    a->bg.g.cadre.y = a->bg.g.instruction.textPosition.y - 15;
    SDL_SetRenderDrawColor(a->renderer, 0, 0, 0, 190);
    SDL_RenderFillRect(a->renderer, &a->bg.g.cadre);
    SDL_RenderCopy(a->renderer, a->bg.g.instruction.textTexture,
        NULL, &a->bg.g.instruction.textPosition);
    SDL_DestroyTexture(a->bg.g.instruction.textTexture);
}

void quitter(app *a){
    if(a->bg.img.texture)           SDL_DestroyTexture(a->bg.img.texture);
    if(a->bg.nbplt > 0 && a->bg.plt[0].texture) SDL_DestroyTexture(a->bg.plt[0].texture);
    if(a->bg.c.t_chrono.font)       TTF_CloseFont(a->bg.c.t_chrono.font);
    if(a->bg.g.instruction.font)    TTF_CloseFont(a->bg.g.instruction.font);
    if(a->p1.score.font)            TTF_CloseFont(a->p1.score.font);
    if(a->p1.score.textTexture)     SDL_DestroyTexture(a->p1.score.textTexture);
    if(a->p1.health.fheart)         SDL_DestroyTexture(a->p1.health.fheart);
    if(a->p1.health.eheart)         SDL_DestroyTexture(a->p1.health.eheart);
    if(a->p2_active){
        if(a->p2.score.font)        TTF_CloseFont(a->p2.score.font);
        if(a->p2.score.textTexture) SDL_DestroyTexture(a->p2.score.textTexture);
    }
    SDL_DestroyRenderer(a->renderer);
    SDL_DestroyWindow(a->window);
    Mix_CloseAudio();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

void detruire_joueur2(app *a){
    if(a->p2.score.font)        { TTF_CloseFont(a->p2.score.font);             a->p2.score.font        = NULL; }
    if(a->p2.score.textTexture) { SDL_DestroyTexture(a->p2.score.textTexture); a->p2.score.textTexture = NULL; }
    a->p2_active = 0;
}

void gestion_evenements_score(app *a, char msg_guide[150]){
    while(SDL_PollEvent(&a->event)){
        gestion_event_joueurs(a);

        if(a->event.type == SDL_QUIT) a->running = 0;

        if(a->event.type == SDL_KEYDOWN && a->event.key.keysym.sym == SDLK_m){
            a->mode = !a->mode;
            if(a->mode == 1){
                a->bg.cam1.w = fen_w / 2;
                if(!a->p2_active){ creation_joueur2(a); a->p2_active = 1; }
            } else {
                a->bg.cam1.w = fen_w;
                if(a->p2_active) detruire_joueur2(a);
            }
        }

        if(a->event.type == SDL_KEYDOWN){
            if(a->event.key.keysym.sym == SDLK_ESCAPE) a->running = 0;

            if(a->mode == 0){
                strcpy(msg_guide, "ZQSD pour bouger, A pour attaquer, Z pour sauter, C pour destruction, M pour mode 2 joueurs");
                switch(a->event.key.keysym.sym){
                case SDLK_d:
                    if(a->p1.dstRect.x + a->p1.dstRect.w >= a->bg.cam1.w - SCROLL_BORDER)
                        scrolling(a, 1, 15, 1);
                    break;
                case SDLK_q:
                    if(a->p1.dstRect.x <= SCROLL_BORDER)
                        scrolling(a, 2, 15, 1);
                    break;
                case SDLK_c:
                    destruction_plateforme(a, a->bg.cam1);
                    break;
                }
            } else {
                strcpy(msg_guide, "Fleches+K pour joueur droite / ZQSD+A pour joueur gauche / C ou N pour destruction");
                switch(a->event.key.keysym.sym){
                case SDLK_RIGHT:
                    if(a->p2.dstRect.x + a->p2.dstRect.w >= a->bg.cam2.w - SCROLL_BORDER)
                        scrolling(a, 1, 15, 2);
                    break;
                case SDLK_LEFT:
                    if(a->p2.dstRect.x <= SCROLL_BORDER)
                        scrolling(a, 2, 15, 2);
                    break;
                case SDLK_n:
                    destruction_plateforme(a, a->bg.cam2);
                    break;
                case SDLK_d:
                    if(a->p1.dstRect.x + a->p1.dstRect.w >= a->bg.cam1.w - SCROLL_BORDER)
                        scrolling(a, 1, 15, 1);
                    break;
                case SDLK_q:
                    if(a->p1.dstRect.x <= SCROLL_BORDER)
                        scrolling(a, 2, 15, 1);
                    break;
                case SDLK_c:
                    destruction_plateforme(a, a->bg.cam1);
                    break;
                }
            }
            guide_lot2(a, msg_guide);
        }
    }

    sync_cam_y(a, 1);
    if(a->p2_active) sync_cam_y(a, 2);
}

void animer_plateforme(app *a){
    for(int i = 0; i < a->bg.nbplt; i++){
        if(a->bg.plt[i].type != 1) continue;
        if(a->bg.plt[i].direction == 0) a->bg.plt[i].pos.x += 1;
        else                            a->bg.plt[i].pos.x -= 1;
        if(a->bg.plt[i].pos.x >= a->bg.plt[i].pos_init + 85)  a->bg.plt[i].direction = 1;
        else if(a->bg.plt[i].pos.x <= a->bg.plt[i].pos_init - 85) a->bg.plt[i].direction = 0;
    }
}

void destruction_plateforme(app *a, SDL_Rect cam_active){
    int nb_rest_d = 0;
    int world_left   = cam_active.x;
    int world_right  = cam_active.x + cam_active.w;
    int world_top    = cam_active.y;
    int world_bottom = cam_active.y + cam_active.h;
    for(int i = 0; i < a->bg.nbplt; i++){
        if(a->bg.plt[i].type != 2 || !a->bg.plt[i].active) continue;
        nb_rest_d++;
        int px = a->bg.plt[i].pos.x;
        int py = a->bg.plt[i].pos.y;
        if(px >= world_left && px + plt_w <= world_right
        && py >= world_top  && py + plt_h <= world_bottom)
            a->bg.plt[i].active = 0;
    }
    if(nb_rest_d <= 1){ a->niveau = 2; creation_lot2(a); }
}
