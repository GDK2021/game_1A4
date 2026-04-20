#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED
typedef struct{
  SDL_Texture *move_d [3];
  SDL_Texture *move_g [3];
  SDL_Texture *attack_d [3];
  SDL_Texture *attack_g [3];
  SDL_Texture *jump_d [3];
  SDL_Texture *jump_g [3];
  SDL_Texture *crouch_d [3];
  SDL_Texture *crouch_g [3];
  SDL_Texture *crouch_walking_d [3];
  SDL_Texture *crouch_walking_g [3];
  SDL_Texture *run_d [3];
  SDL_Texture *run_g [3];
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
  standing,  // 0
  walking_L, // 1
  walking_R, // 2
  runing_L,  // 3
  runing_R,  // 4
  jumping,   // 5
  crouching, // 6
  attacking_R,  // 7
  attacking_L  // 8
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
  int ticks;
  SDL_Window *window ;
  SDL_Renderer *renderer ;
  SDL_Event event;
  player p1;
  int running;
}app;






void SDL_Exitwitherror(const char *msg);
void quitter(app* app);
void affichage(app* app, int x, int y);
void gestion_event(app* app,int* x, int* y);
void creation_joueur(app *app);
void initialisation(app* app);
void afficher_perso(app* app);
void afficher_vie(app* app);
void saut(app *app);
void loadtexture(app *app, const char *framename, SDL_Texture *(*dest)[3]);
#endif // HEADER_H_INCLUDED