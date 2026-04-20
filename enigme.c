#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

int chargerEnigmes(Enigme t[], int max, char *nomFichier) {
    FILE *f = fopen(nomFichier, "r");
    if (!f) { printf("Impossible d'ouvrir %s\n", nomFichier); return 0; }
    int i = 0;
    while (i < max && fgets(t[i].question, 200, f)) {
        t[i].question[strcspn(t[i].question, "\n")] = 0;
        for (int j = 0; j < 3; j++) {
            if (fgets(t[i].rep[j], 100, f))
                t[i].rep[j][strcspn(t[i].rep[j], "\n")] = 0;
        }
        if (fscanf(f, "%d\n", &t[i].bonne_rep) != 1)
            t[i].bonne_rep = 1;
        t[i].deja_vu = 0;
        i++;
    }
    fclose(f);
    return i;
}

void afficherTexte(SDL_Renderer *renderer, char *texte, int x, int y, TTF_Font *font, SDL_Color color) {
    SDL_Surface *surface = TTF_RenderText_Blended(font, texte, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dst = {x, y, surface->w, surface->h};
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_DestroyTexture(texture);
}

void afficherEnigme(SDL_Renderer *renderer, Enigme e, TTF_Font *font) {
    SDL_Color white = {255, 255, 255, 255};
    afficherTexte(renderer, e.question, 50, 50, font, white);
    afficherTexte(renderer, e.rep[0], 70, 150, font, white);
    afficherTexte(renderer, e.rep[1], 70, 200, font, white);
    afficherTexte(renderer, e.rep[2], 70, 250, font, white);
}

int verifier(Enigme e, int choix) {
    return (choix == e.bonne_rep);
}

int aleatoire(int max) {
    return rand() % max;
}

void lancer_enigme(app *app) {
    TTF_Font *font = TTF_OpenFont("arial.ttf", 24);
    if (!font) { SDL_Log("Erreur font\n"); return; }

    Enigme tab[MAX_QUESTIONS];
    int n = chargerEnigmes(tab, MAX_QUESTIONS, "questions.txt");
    if (n < 10) { TTF_CloseFont(font); return; }

    int score = 0, vies = 3, niveau = 1;
    int questions_posees = 0;

    while (questions_posees < 10 && vies > 0) {
        int index;
        do { index = aleatoire(n); } while (tab[index].deja_vu);
        tab[index].deja_vu = 1;

        int choix = 0;
        Uint32 startTime = SDL_GetTicks();

        while (!choix && (SDL_GetTicks() - startTime < TEMPS_MAX)) {
            SDL_SetRenderDrawColor(app->renderer, 0, 0, 100, 255);
            SDL_RenderClear(app->renderer);
            afficherEnigme(app->renderer, tab[index], font);

            Uint32 elapsed = SDL_GetTicks() - startTime;
            SDL_Rect bar = {50, 400, 600 - (int)(600.0 * elapsed / TEMPS_MAX), 20};
            SDL_SetRenderDrawColor(app->renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(app->renderer, &bar);

            char info[100];
            sprintf(info, "Score: %d | Vies: %d | Niveau: %d", score, vies, niveau);
            SDL_Color white = {255, 255, 255, 255};
            afficherTexte(app->renderer, info, 50, 500, font, white);
            SDL_RenderPresent(app->renderer);

            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) app->running = 0;
                if (e.type == SDL_KEYDOWN) {
                    switch (e.key.keysym.sym) {
                        case SDLK_1: case SDLK_KP_1: choix = 1; break;
                        case SDLK_2: case SDLK_KP_2: choix = 2; break;
                        case SDLK_3: case SDLK_KP_3: choix = 3; break;
                    }
                }
            }
        }

        if (choix == 0 || !verifier(tab[index], choix)) vies--;
        else score += 10;
        questions_posees++;
        if (questions_posees % 5 == 0) niveau++;
    }

    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);
    char finalScore[100];
    sprintf(finalScore, "Score final: %d | Niveau: %d", score, niveau);
    SDL_Color white = {255, 255, 255, 255};
    afficherTexte(app->renderer, finalScore, 200, 300, font, white);
    SDL_RenderPresent(app->renderer);
    SDL_Delay(3000);
    TTF_CloseFont(font);
}