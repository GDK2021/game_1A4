#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

typedef struct{
    int state;
    SDL_Texture *texture;
    SDL_Surface *surface;
    SDL_Rect srcRect;
    SDL_Rect dstRect;
}image;

typedef struct{
    TTF_Font *font;
    SDL_Surface *textSurface;
    SDL_Texture *textTexture;
    SDL_Rect textPosition;
    SDL_Color color;
}texte;

typedef struct{
    Uint32 tmp_debut;
    Uint32 tmp_actuel;
    texte t_chrono;
}chrono;

typedef struct{
    SDL_Rect cadre;
    texte instruction;
    int afficher;
}guide;

typedef struct{
    SDL_Rect pos;
    int pos_init;
    SDL_Texture *texture;
    int type;
    int direction;
    int active;
}plateforme;

typedef struct{
    image img;
    SDL_Rect cam1;
    SDL_Rect cam2;
    chrono c;
    guide g;
    plateforme plt[50];
    int nbplt;
}background;

typedef struct{
    SDL_Texture *move_d[3];
    SDL_Texture *move_g[3];
    SDL_Texture *attack_d[3];
    SDL_Texture *attack_g[3];
    SDL_Texture *jump_d[3];
    SDL_Texture *jump_g[3];
    SDL_Texture *run_d[3];
    SDL_Texture *run_g[3];
}textures;

typedef struct{
    SDL_Surface *surface;
    SDL_Texture *fheart;
    SDL_Texture *eheart;
    SDL_Rect srcRect;
    SDL_Rect dstRect;
    int amount;
}health;

typedef struct{
    TTF_Font *font;
    SDL_Surface *textSurface;
    SDL_Texture *textTexture;
    SDL_Rect textPosition;
    SDL_Color color;
}score;

typedef enum {
    standing,
    walking_L,
    walking_R,
    runing_L,
    runing_R,
    jumping,
    attacking_R,
    attacking_L
}state_enum;

typedef struct{
    state_enum state;
    state_enum laststate;
    textures tx;
    SDL_Surface *surface;
    SDL_Rect srcRect;
    SDL_Rect dstRect;
    score score;
    health health;
    int velocity;
    int move_ticks;
    int last_attack_time;
    int up;
    float jump_x;
    int posy_init;
}player;

typedef struct{
    SDL_Window   *window;
    SDL_Renderer *renderer;
    SDL_Event     event;
    int running;
    int mode;
    int niveau;
    background bg;
    int ticks;
    int ticks2;
    player p1;
    player p2;
    int p2_active;
}app;

void sdl_erreur(const char *msg, const char *msgr);
void initialisation(app *app);
void creation_lot2(app *app);
void afficher_lot2(app *app);
void animer_plateforme(app *app);
void destruction_plateforme(app *app, SDL_Rect cam_active);
void scrolling(app *app, int direction, int pas, int j);
void sync_cam_y(app *app, int jouer);
void initChrono(app *app);
void afficher_chrono(app *app);
void guide_lot2(app *app, char *msg_guide);
void gestion_evenements_score(app *app, char msg_guide[150]);
void detruire_joueur2(app *app);
void quitter(app *app);

void SDL_Exitwitherror(const char *msg);
void affichage(app *app, int x, int y);
void gestion_event_joueurs(app *app);
void creation_joueur(app *app);
void creation_joueur2(app *app);
void afficher_perso(app *app);
void afficher_perso2(app *app);
void afficher_vie(app *app);
void afficher_vie2(app *app);
void saut(app *app);
void saut2(app *app);
void loadtexture(app *app, const char *framename, SDL_Texture *(*dest)[3]);
void loadtexture2(app *app, const char *framename, SDL_Texture *(*dest)[3]);

#endif
