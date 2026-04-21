#ifndef HEADER_H_INCLUDED 
#define HEADER_H_INCLUDED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
 
//les stuctures
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
  int type;//0:fixe 1:mobile 2:destructible
  int direction;//0:imine, 1:isar
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
  SDL_Window *window ;
  SDL_Renderer *renderer ; 
  SDL_Event event;
  int running;
  int mode;
  int niveau;
  background bg;
}app;

//les fonctions
void sdl_erreur(const char* msg,const char* msgr);
void initialisation(app *app);
void creation_lot2(app *app);
void afficher_lot2(app *app);
void animer_plateforme(app *app);
void destruction_plateforme(app *app, SDL_Rect cam_active);
void scrolling(app *app, int direction, int pas, int j);
void initChrono(app *app);
void afficher_chrono(app *app);
void guide_lot2(app *app, char *msg_guide);
void gestion_evenements_score(app *app, char msg_guide[150]);
void quitter_lot2(app *app);

#endif