#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "header.h"

#define win_w           1920
#define win_h           1080
#define fen_w           1920
#define fen_h           1080
#define bg_h            2500
#define ATTACK_COOLDOWN 500
#define PLAYER_START_Y  (win_h - 450)
#define SCROLL_BORDER   100

void SDL_Exitwitherror(const char *msg){
    SDL_Log("erreur : %s > %s\n", msg, SDL_GetError());
    SDL_Quit();
    exit(EXIT_FAILURE);
}

void loadtexture(app *a, const char *framename, SDL_Texture *(*dest)[3]){
    char filename[100];
    for(int i = 0; i < 3; i++){
        sprintf(filename, "%s%d.png", framename, i + 1);
        SDL_Surface *temp = IMG_Load(filename);
        if(!temp){ printf("Missing: %s\n", filename); SDL_Exitwitherror("IMG_Load"); }
        (*dest)[i] = SDL_CreateTextureFromSurface(a->renderer, temp);
        if(i == 0) SDL_QueryTexture((*dest)[i], NULL, NULL, &a->p1.srcRect.w, &a->p1.srcRect.h);
        SDL_FreeSurface(temp);
    }
}

void loadtexture2(app *a, const char *framename, SDL_Texture *(*dest)[3]){
    char filename[100];
    for(int i = 0; i < 3; i++){
        sprintf(filename, "%s%d.png", framename, i + 1);
        SDL_Surface *temp = IMG_Load(filename);
        if(!temp){ printf("Missing: %s\n", filename); SDL_Exitwitherror("IMG_Load"); }
        (*dest)[i] = SDL_CreateTextureFromSurface(a->renderer, temp);
        if(i == 0) SDL_QueryTexture((*dest)[i], NULL, NULL, &a->p2.srcRect.w, &a->p2.srcRect.h);
        SDL_FreeSurface(temp);
    }
}

void creation_joueur(app *a){
    char fn[50];
    sprintf(fn, "src/player_d_");  loadtexture(a, fn, &a->p1.tx.move_d);
    sprintf(fn, "src/player_g_");  loadtexture(a, fn, &a->p1.tx.move_g);
    sprintf(fn, "src/attack_d_");  loadtexture(a, fn, &a->p1.tx.attack_d);
    sprintf(fn, "src/attack_g_");  loadtexture(a, fn, &a->p1.tx.attack_g);

    a->p1.srcRect.x = 0; a->p1.srcRect.y = 0;
    a->p1.dstRect.w = a->p1.srcRect.w;
    a->p1.dstRect.h = a->p1.srcRect.h;
    a->p1.dstRect.x = 200;
    a->p1.dstRect.y = PLAYER_START_Y;
    a->p1.posy_init = PLAYER_START_Y;
    a->p1.state     = standing;
    a->p1.laststate = walking_R;

    a->p1.score.font = TTF_OpenFont("src/ka1.ttf", 30);
    if(!a->p1.score.font) SDL_Exitwitherror("font p1");
    a->p1.score.color = (SDL_Color){0, 255, 255, 255};
    a->p1.score.textSurface = TTF_RenderText_Solid(a->p1.score.font, "0000", a->p1.score.color);
    if(!a->p1.score.textSurface) SDL_Exitwitherror("surface score p1");
    a->p1.score.textTexture = SDL_CreateTextureFromSurface(a->renderer, a->p1.score.textSurface);
    SDL_FreeSurface(a->p1.score.textSurface);
    SDL_QueryTexture(a->p1.score.textTexture, NULL, NULL,
        &a->p1.score.textPosition.w, &a->p1.score.textPosition.h);

    a->p1.health.surface = IMG_Load("src/fheart.png");
    if(!a->p1.health.surface) SDL_Exitwitherror("fheart");
    a->p1.health.fheart = SDL_CreateTextureFromSurface(a->renderer, a->p1.health.surface);
    SDL_FreeSurface(a->p1.health.surface);
    a->p1.health.surface = IMG_Load("src/eheart.png");
    if(!a->p1.health.surface) SDL_Exitwitherror("eheart");
    a->p1.health.eheart = SDL_CreateTextureFromSurface(a->renderer, a->p1.health.surface);
    SDL_FreeSurface(a->p1.health.surface);

    int heart_w = 60, heart_h = 56, heart_gap = 8;
    int total_w = 3 * heart_w + 2 * heart_gap;
    int hearts_x = 20;
    int hearts_y = 15;

    a->p1.health.srcRect = (SDL_Rect){0, 0, 120, 112};
    a->p1.health.dstRect = (SDL_Rect){hearts_x, hearts_y, heart_w, heart_h};
    a->p1.health.amount  = 3;

    a->p1.score.textPosition.x = hearts_x + (total_w - a->p1.score.textPosition.w) / 2;
    a->p1.score.textPosition.y = hearts_y + heart_h + 5;

    a->p1.up     = 0;
    a->p1.jump_x = -50.0f;
}

void creation_joueur2(app *a){
    a->p2.tx      = a->p1.tx;
    a->p2.srcRect = a->p1.srcRect;
    a->p2.dstRect.w = a->p2.srcRect.w;
    a->p2.dstRect.h = a->p2.srcRect.h;
    a->p2.dstRect.x = 200;
    a->p2.dstRect.y = PLAYER_START_Y;
    a->p2.posy_init = PLAYER_START_Y;
    a->p2.state     = standing;
    a->p2.laststate = walking_L;

    a->p2.score.font = TTF_OpenFont("src/ka1.ttf", 30);
    if(!a->p2.score.font) SDL_Exitwitherror("font p2");
    a->p2.score.color = (SDL_Color){255, 100, 100, 255};
    a->p2.score.textSurface = TTF_RenderText_Solid(a->p2.score.font, "0000", a->p2.score.color);
    if(!a->p2.score.textSurface) SDL_Exitwitherror("surface score p2");
    a->p2.score.textTexture = SDL_CreateTextureFromSurface(a->renderer, a->p2.score.textSurface);
    SDL_FreeSurface(a->p2.score.textSurface);
    SDL_QueryTexture(a->p2.score.textTexture, NULL, NULL,
        &a->p2.score.textPosition.w, &a->p2.score.textPosition.h);

    a->p2.health.fheart  = a->p1.health.fheart;
    a->p2.health.eheart  = a->p1.health.eheart;

    int heart_w2 = 60, heart_h2 = 56, heart_gap2 = 8;
    int total_w2 = 3 * heart_w2 + 2 * heart_gap2;
    int hearts_x2 = (fen_w / 2) - total_w2 - 20;
    int hearts_y2 = 15;

    a->p2.health.srcRect = (SDL_Rect){0, 0, 120, 112};
    a->p2.health.dstRect = (SDL_Rect){hearts_x2, hearts_y2, heart_w2, heart_h2};
    a->p2.health.amount  = 3;

    a->p2.score.textPosition.x = hearts_x2 + (total_w2 - a->p2.score.textPosition.w) / 2;
    a->p2.score.textPosition.y = hearts_y2 + heart_h2 + 5;

    a->p2.up               = 0;
    a->p2.jump_x           = -50.0f;
    a->p2.last_attack_time = 0;
    a->p2.move_ticks       = 0;
}

void saut(app *a){
    if(!a->p1.up) return;
    float rel_y = -0.06f * (a->p1.jump_x * a->p1.jump_x) + 150.0f;
    a->p1.jump_x += 1.5f;
    a->p1.dstRect.y = a->p1.posy_init - (int)rel_y;
    if(a->p1.jump_x >= 50.0f){
        a->p1.up = 0; a->p1.jump_x = -50.0f;
        a->p1.dstRect.y = a->p1.posy_init;
        a->p1.state = standing;
    }
}

void saut2(app *a){
    if(!a->p2.up) return;
    float rel_y = -0.06f * (a->p2.jump_x * a->p2.jump_x) + 150.0f;
    a->p2.jump_x += 1.5f;
    a->p2.dstRect.y = a->p2.posy_init - (int)rel_y;
    if(a->p2.jump_x >= 50.0f){
        a->p2.up = 0; a->p2.jump_x = -50.0f;
        a->p2.dstRect.y = a->p2.posy_init;
        a->p2.state = standing;
    }
}

void afficher_vie(app *a){
    int heart_w = a->p1.health.dstRect.w;
    int heart_gap = 8;
    SDL_Rect hr = a->p1.health.dstRect;
    for(int i = 0; i < a->p1.health.amount; i++){
        SDL_RenderCopy(a->renderer, a->p1.health.fheart, &a->p1.health.srcRect, &hr);
        hr.x += heart_w + heart_gap;
    }
    for(int i = 0; i < 3 - a->p1.health.amount; i++){
        SDL_RenderCopy(a->renderer, a->p1.health.eheart, &a->p1.health.srcRect, &hr);
        hr.x += heart_w + heart_gap;
    }
}

void afficher_vie2(app *a){
    int heart_w = a->p2.health.dstRect.w;
    int heart_gap = 8;
    SDL_Rect hr = a->p2.health.dstRect;
    for(int i = 0; i < a->p2.health.amount; i++){
        SDL_RenderCopy(a->renderer, a->p2.health.fheart, &a->p2.health.srcRect, &hr);
        hr.x += heart_w + heart_gap;
    }
    for(int i = 0; i < 3 - a->p2.health.amount; i++){
        SDL_RenderCopy(a->renderer, a->p2.health.eheart, &a->p2.health.srcRect, &hr);
        hr.x += heart_w + heart_gap;
    }
}

void gestion_event_joueurs(app *a){
    if(a->p1.state == attacking_L || a->p1.state == attacking_R){
        if(SDL_GetTicks() - (Uint32)a->p1.move_ticks >= 300){
            a->p1.state = standing; a->p1.last_attack_time = SDL_GetTicks();
        }
    }
    if(a->p2_active && (a->p2.state == attacking_L || a->p2.state == attacking_R)){
        if(SDL_GetTicks() - (Uint32)a->p2.move_ticks >= 300){
            a->p2.state = standing; a->p2.last_attack_time = SDL_GetTicks();
        }
    }

    if(a->event.type == SDL_KEYDOWN){
        SDL_Keycode sym = a->event.key.keysym.sym;
        if(sym == SDLK_ESCAPE || sym == SDLK_m) return;

        if(a->p1.state != attacking_L && a->p1.state != attacking_R){
            if(sym == SDLK_d){
                if(!a->ticks) a->ticks = SDL_GetTicks();
                a->p1.state = walking_R;
            } else if(sym == SDLK_q){
                if(!a->ticks) a->ticks = SDL_GetTicks();
                a->p1.state = walking_L;
            } else if(sym == SDLK_z){
                if(!a->p1.up){
                    a->p1.up = 1; a->p1.jump_x = -50.0f;
                    a->p1.posy_init = a->p1.dstRect.y;
                    a->p1.state = jumping;
                }
            } else if(sym == SDLK_a){
                Uint32 now = SDL_GetTicks();
                if(now - (Uint32)a->p1.last_attack_time >= ATTACK_COOLDOWN){
                    a->p1.move_ticks = now;
                    a->p1.state = (a->p1.laststate == walking_R) ? attacking_R : attacking_L;
                }
            } else if(sym == SDLK_h){ if(a->p1.health.amount < 3) a->p1.health.amount++; }
            else if(sym == SDLK_j){   if(a->p1.health.amount > 0) a->p1.health.amount--; }
        }

        if(a->p2_active && a->p2.state != attacking_L && a->p2.state != attacking_R){
            if(sym == SDLK_RIGHT){
                if(!a->ticks2) a->ticks2 = SDL_GetTicks();
                a->p2.state = walking_R;
            } else if(sym == SDLK_LEFT){
                if(!a->ticks2) a->ticks2 = SDL_GetTicks();
                a->p2.state = walking_L;
            } else if(sym == SDLK_UP){
                if(!a->p2.up){
                    a->p2.up = 1; a->p2.jump_x = -50.0f;
                    a->p2.posy_init = a->p2.dstRect.y;
                    a->p2.state = jumping;
                }
            } else if(sym == SDLK_k){
                Uint32 now = SDL_GetTicks();
                if(now - (Uint32)a->p2.last_attack_time >= ATTACK_COOLDOWN){
                    a->p2.move_ticks = now;
                    a->p2.state = (a->p2.laststate == walking_R) ? attacking_R : attacking_L;
                }
            }
        }
    }

    if(a->event.type == SDL_KEYUP){
        SDL_Keycode sym = a->event.key.keysym.sym;
        if((sym == SDLK_d || sym == SDLK_q || sym == SDLK_z) &&
           a->p1.state != attacking_L && a->p1.state != attacking_R){
            a->p1.laststate = a->p1.state; a->p1.state = standing; a->ticks = 0;
        }
        if(a->p2_active && (sym == SDLK_RIGHT || sym == SDLK_LEFT || sym == SDLK_UP) &&
           a->p2.state != attacking_L && a->p2.state != attacking_R){
            a->p2.laststate = a->p2.state; a->p2.state = standing; a->ticks2 = 0;
        }
    }
}

void afficher_perso(app *a){
    int cam_w = a->bg.cam1.w;
    int frame;
    switch(a->p1.state){
    case standing:
        SDL_RenderCopy(a->renderer,
            (a->p1.laststate == walking_L) ? a->p1.tx.move_g[0] : a->p1.tx.move_d[0],
            &a->p1.srcRect, &a->p1.dstRect);
        break;
    case walking_R:
        if(a->p1.dstRect.x + a->p1.dstRect.w < cam_w - SCROLL_BORDER)
            a->p1.dstRect.x += 10;
        frame = ((SDL_GetTicks() - a->ticks) / 100) % 3;
        SDL_RenderCopy(a->renderer, a->p1.tx.move_d[frame], &a->p1.srcRect, &a->p1.dstRect);
        break;
    case walking_L:
        if(a->p1.dstRect.x > SCROLL_BORDER)
            a->p1.dstRect.x -= 10;
        frame = ((SDL_GetTicks() - a->ticks) / 100) % 3;
        SDL_RenderCopy(a->renderer, a->p1.tx.move_g[frame], &a->p1.srcRect, &a->p1.dstRect);
        break;
    case attacking_R:
        frame = ((SDL_GetTicks() - a->p1.move_ticks) / 100) % 3;
        SDL_RenderCopy(a->renderer, a->p1.tx.attack_d[frame], &a->p1.srcRect, &a->p1.dstRect);
        break;
    case attacking_L:
        frame = ((SDL_GetTicks() - a->p1.move_ticks) / 100) % 3;
        SDL_RenderCopy(a->renderer, a->p1.tx.attack_g[frame], &a->p1.srcRect, &a->p1.dstRect);
        break;
    case jumping:
        frame = (SDL_GetTicks() / 150) % 3;
        SDL_RenderCopy(a->renderer,
            (a->p1.laststate == walking_L) ? a->p1.tx.move_g[frame] : a->p1.tx.move_d[frame],
            &a->p1.srcRect, &a->p1.dstRect);
        break;
    default: break;
    }
}

void afficher_perso2(app *a){
    if(!a->p2_active) return;
    int cam_w = a->bg.cam2.w;
    int frame;
    for(int i = 0; i < 3; i++){
        SDL_SetTextureColorMod(a->p2.tx.move_d[i],   100, 100, 255);
        SDL_SetTextureColorMod(a->p2.tx.move_g[i],   100, 100, 255);
        SDL_SetTextureColorMod(a->p2.tx.attack_d[i], 100, 100, 255);
        SDL_SetTextureColorMod(a->p2.tx.attack_g[i], 100, 100, 255);
    }
    switch(a->p2.state){
    case standing:
        SDL_RenderCopy(a->renderer,
            (a->p2.laststate == walking_L) ? a->p2.tx.move_g[0] : a->p2.tx.move_d[0],
            &a->p2.srcRect, &a->p2.dstRect);
        break;
    case walking_R:
        if(a->p2.dstRect.x + a->p2.dstRect.w < cam_w - SCROLL_BORDER)
            a->p2.dstRect.x += 10;
        frame = ((SDL_GetTicks() - a->ticks2) / 100) % 3;
        SDL_RenderCopy(a->renderer, a->p2.tx.move_d[frame], &a->p2.srcRect, &a->p2.dstRect);
        break;
    case walking_L:
        if(a->p2.dstRect.x > SCROLL_BORDER)
            a->p2.dstRect.x -= 10;
        frame = ((SDL_GetTicks() - a->ticks2) / 100) % 3;
        SDL_RenderCopy(a->renderer, a->p2.tx.move_g[frame], &a->p2.srcRect, &a->p2.dstRect);
        break;
    case attacking_R:
        frame = ((SDL_GetTicks() - a->p2.move_ticks) / 100) % 3;
        SDL_RenderCopy(a->renderer, a->p2.tx.attack_d[frame], &a->p2.srcRect, &a->p2.dstRect);
        break;
    case attacking_L:
        frame = ((SDL_GetTicks() - a->p2.move_ticks) / 100) % 3;
        SDL_RenderCopy(a->renderer, a->p2.tx.attack_g[frame], &a->p2.srcRect, &a->p2.dstRect);
        break;
    case jumping:
        frame = (SDL_GetTicks() / 150) % 3;
        SDL_RenderCopy(a->renderer,
            (a->p2.laststate == walking_L) ? a->p2.tx.move_g[frame] : a->p2.tx.move_d[frame],
            &a->p2.srcRect, &a->p2.dstRect);
        break;
    default: break;
    }
    for(int i = 0; i < 3; i++){
        SDL_SetTextureColorMod(a->p2.tx.move_d[i],   255, 255, 255);
        SDL_SetTextureColorMod(a->p2.tx.move_g[i],   255, 255, 255);
        SDL_SetTextureColorMod(a->p2.tx.attack_d[i], 255, 255, 255);
        SDL_SetTextureColorMod(a->p2.tx.attack_g[i], 255, 255, 255);
    }
}

void affichage(app *a, int x, int y){
    SDL_Rect viewport_droite = {fen_w / 2, 0, fen_w / 2, fen_h};

    afficher_perso(a);
    saut(a);
    afficher_vie(a);
    SDL_RenderCopy(a->renderer, a->p1.score.textTexture, NULL, &a->p1.score.textPosition);

    if(a->p2_active){
        SDL_RenderSetViewport(a->renderer, &viewport_droite);
        afficher_perso2(a);
        saut2(a);
        afficher_vie2(a);
        SDL_RenderCopy(a->renderer, a->p2.score.textTexture, NULL, &a->p2.score.textPosition);
        SDL_RenderSetViewport(a->renderer, NULL);
    }
}
