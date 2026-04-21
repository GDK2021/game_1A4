#include "header_lot2.h" 
#include <SDL2/SDL.h> 
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <string.h>
#include <time.h>
//kober el fentr 
#define fen_w 1920
#define fen_h 1080
//kober el bg
#define bg_w 5000
#define bg_h 2500
//kober les plateforme
#define plt_w 100
#define plt_h 40



void sdl_erreur(const char *msg, const char *msgr){
  printf("Erreur!!!!: %s\n Le probleme: %s\n", msg, msgr);
  SDL_Quit();
}

void initialisation(app *app){
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0){
    sdl_erreur("Errur init SDL init",SDL_GetError());
    quitter_lot2(app);
  }

  if(TTF_Init() == -1){
    sdl_erreur("Erreur init TTF texte",TTF_GetError());
    quitter_lot2(app);
  }

  if((!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))){
    sdl_erreur("Erreur IMG init",IMG_GetError());
    quitter_lot2(app);
  }
  app->window = NULL;
  app->window = SDL_CreateWindow("Cold Revenge-Score",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              fen_w,
                              fen_h,
                              SDL_WINDOW_FULLSCREEN
  );
  if(app->window == NULL){
      sdl_erreur("Erreur creation window",SDL_GetError());
      quitter_lot2(app);
  }

  app->renderer = NULL;
  app->renderer = SDL_CreateRenderer(app->window, -1, SDL_RENDERER_ACCELERATED);
  if(app->renderer == NULL){
      sdl_erreur("Erreur creation renderer",SDL_GetError());
      quitter_lot2(app);
  }
}

void creation_lot2(app *app){
  // taswiret el bg
  app->bg.img.surface = NULL;
  app->bg.img.surface = IMG_Load("src/images/bg_jeu.png");
  if (app->bg.img.surface == NULL){
    sdl_erreur("Echec chargement de background", IMG_GetError());
  }
  app->bg.img.texture = NULL;
  app->bg.img.texture = SDL_CreateTextureFromSurface(app->renderer, app->bg.img.surface);
  if (app->bg.img.texture == NULL){
    sdl_erreur("Echec chargement de background", SDL_GetError());
  }
  SDL_FreeSurface(app->bg.img.surface);
  // el camerawet el zouz
  app->bg.cam1.x = 0;
  app->bg.cam1.y = 0;
  app->bg.cam1.h = fen_h;
  app->bg.cam1.w = fen_w;
  app->bg.cam2.x = 0;
  app->bg.cam2.y = 0;
  app->bg.cam2.h = fen_h;
  app->bg.cam2.w = fen_w / 2;

  // le guide:
  app->bg.g.afficher = 0;
  app->bg.g.instruction.font = NULL;
  app->bg.g.instruction.font = TTF_OpenFont("src/font/arial.ttf", 45);
  if (app->bg.g.instruction.font == NULL)  {
    sdl_erreur("Echec font guide", TTF_GetError());
  }
  app->bg.g.instruction.color.r = 255;
  app->bg.g.instruction.color.g = 255;
  app->bg.g.instruction.color.b = 255;
  app->bg.g.instruction.color.a = 255;
  // le cadre du guide
  app->bg.g.cadre.w = 600;
  app->bg.g.cadre.h = 100;
  app->bg.g.cadre.x = (fen_w / 2) - (app->bg.g.cadre.w / 2);
  app->bg.g.cadre.y = (fen_h / 2) - (app->bg.g.cadre.h / 2);

  //les plateforme
  SDL_Surface *img_plt = NULL;
  img_plt = IMG_Load("src/images/plateforme.png");
  SDL_Surface *img_plt_d = NULL;
  img_plt_d = IMG_Load("src/images/plateforme_d.png");
  if (img_plt == NULL || img_plt_d == NULL){
    sdl_erreur("Echec chargement de plateforme png", IMG_GetError());
  }
  SDL_Texture *texture_plt = NULL;
  texture_plt = SDL_CreateTextureFromSurface(app->renderer, img_plt);
  SDL_Texture *texture_plt_d = NULL;
  texture_plt_d = SDL_CreateTextureFromSurface(app->renderer, img_plt_d);
  if (texture_plt == NULL || texture_plt_d == NULL){
    sdl_erreur("Echec chargement de texture platforme", SDL_GetError());
  }
  SDL_FreeSurface(img_plt);
  SDL_FreeSurface(img_plt_d);
//nombre mta3 les platefome
  if(app->niveau == 1){
    app->bg.nbplt = 40;
  }else{
    app->bg.nbplt = 45;
  }

  for(int i = 0; i < app->bg.nbplt; i++){
    app->bg.plt[i].pos.h = plt_h;
    app->bg.plt[i].pos.w = plt_w;
    app->bg.plt[i].pos.x = rand() % (bg_w - plt_w);
    app->bg.plt[i].pos.y = rand() % (bg_h - plt_h);
    app->bg.plt[i].direction = i % 2; 
    app->bg.plt[i].active = 1;
    if(i < 20){//nb platforme fixe
      app->bg.plt[i].texture = texture_plt;
      app->bg.plt[i].type = 0;
    }else if(i <35){//nb plateform yetharkou
      app->bg.plt[i].texture = texture_plt;
      app->bg.plt[i].type = 1;
      app->bg.plt[i].pos_init = app->bg.plt[i].pos.x;
    }else{
      app->bg.plt[i].texture = texture_plt_d;
      app->bg.plt[i].type = 2;
    }
  }
}

void afficher_lot2(app *app) {
    if (app->mode == 0) {
      // Affichage du background (wehed yalab)
      SDL_RenderSetViewport(app->renderer, NULL);
      SDL_RenderCopy(app->renderer, app->bg.img.texture, &app->bg.cam1, NULL);
        
      for (int i = 0; i < app->bg.nbplt; i++) {
        if (app->bg.plt[i].active == 1) {
          SDL_Rect dest;
          dest.h = app->bg.plt[i].pos.h;
          dest.w = app->bg.plt[i].pos.w;
          // Position relative à la caméra 1
          dest.x = app->bg.plt[i].pos.x - app->bg.cam1.x;
          dest.y = app->bg.plt[i].pos.y - app->bg.cam1.y;
               
          SDL_RenderCopy(app->renderer, app->bg.plt[i].texture, NULL, &dest);
        }
      }
    }
    else if (app->mode == 1) {//zouz yalabou
      SDL_Rect viewport_gauche = {0, 0, fen_w / 2, fen_h};
      SDL_Rect viewport_droite = {fen_w / 2, 0, fen_w / 2, fen_h};

      // --- JOUEUR À GAUCHE ---
      SDL_RenderSetViewport(app->renderer, &viewport_gauche);
      SDL_RenderCopy(app->renderer, app->bg.img.texture, &app->bg.cam1, NULL);
       
      for (int i = 0; i < app->bg.nbplt; i++) {
        if (app->bg.plt[i].active == 1) {
          SDL_Rect dest;
          dest.h = app->bg.plt[i].pos.h;
          dest.w = app->bg.plt[i].pos.w;
          dest.x = app->bg.plt[i].pos.x - app->bg.cam1.x;
          dest.y = app->bg.plt[i].pos.y - app->bg.cam1.y;
          SDL_RenderCopy(app->renderer, app->bg.plt[i].texture, NULL, &dest);
        }
      }
      // --- JOUEUR À DROITE ---
      SDL_RenderSetViewport(app->renderer, &viewport_droite);
      SDL_RenderCopy(app->renderer, app->bg.img.texture, &app->bg.cam2, NULL);
      
      for (int i = 0; i < app->bg.nbplt; i++) {
        if (app->bg.plt[i].active == 1) {
          SDL_Rect dest;
          dest.h = app->bg.plt[i].pos.h;
          dest.w = app->bg.plt[i].pos.w;
          dest.x = app->bg.plt[i].pos.x - app->bg.cam2.x;
          dest.y = app->bg.plt[i].pos.y - app->bg.cam2.y;
          SDL_RenderCopy(app->renderer, app->bg.plt[i].texture, NULL, &dest);
        }
      }

      SDL_RenderSetViewport(app->renderer, NULL);
    }
}

void scrolling(app *app, int direction, int pas, int jouer){
  SDL_Rect *cam;
  if (jouer == 1){
    cam = &app->bg.cam1;
  }
  else if (jouer == 2){
    cam = &app->bg.cam2;
  }
  switch (direction){
  case 1: // droite
    cam->x += pas;
    if (cam->x > (bg_w - cam->w)){
      cam->x = (bg_w - cam->w);
    }
    break;
  case 2: // gauche
    cam->x -= pas;
    if (cam->x < 0){
      cam->x = 0;
    }
    break;
  case 3: // haut
    cam->y -= pas;
    if (cam->y < 0){
      cam->y = 0;
    }
    break;
  case 4: // bas
    cam->y += pas;
    if (cam->y > (bg_h - cam->h)){
      cam->y = (bg_h - cam->h);
    }
    break;
  }
}

void initChrono(app *app){
  app->bg.c.tmp_debut = SDL_GetTicks();
  app->bg.c.tmp_actuel = 0;
  app->bg.c.t_chrono.font = NULL;
  app->bg.c.t_chrono.font = TTF_OpenFont("src/font/arial.ttf", 25);
  if (app->bg.c.t_chrono.font == NULL)
  {
    sdl_erreur("Echec font chrono", TTF_GetError());
  }
  app->bg.c.t_chrono.color.r = 255;
  app->bg.c.t_chrono.color.g = 255;
  app->bg.c.t_chrono.color.b = 255;
  app->bg.c.t_chrono.color.a = 185;

  app->bg.c.t_chrono.textPosition.x = fen_w - 150;
  app->bg.c.t_chrono.textPosition.y = 30;
}

void afficher_chrono(app *app){
  app->bg.c.tmp_actuel = SDL_GetTicks() - app->bg.c.tmp_debut;
  int total_sec = app->bg.c.tmp_actuel / 1000;
  char tmp_txt[20];
  sprintf(tmp_txt, "%02d:%02d", (total_sec / 60), (total_sec % 60));

  app->bg.c.t_chrono.textSurface = NULL;
  app->bg.c.t_chrono.textSurface = TTF_RenderText_Blended(app->bg.c.t_chrono.font, tmp_txt, app->bg.c.t_chrono.color);
  if (app->bg.c.t_chrono.textSurface == NULL)
  {
    sdl_erreur("Echec de chargement du chrono", TTF_GetError());
  }

  app->bg.c.t_chrono.textTexture = NULL;
  app->bg.c.t_chrono.textTexture = SDL_CreateTextureFromSurface(app->renderer, app->bg.c.t_chrono.textSurface);
  if (app->bg.c.t_chrono.textTexture == NULL)
  {
    sdl_erreur("Echec chargement texture chrono", SDL_GetError());
  }
  SDL_FreeSurface(app->bg.c.t_chrono.textSurface);

  SDL_QueryTexture(app->bg.c.t_chrono.textTexture, NULL, NULL, &app->bg.c.t_chrono.textPosition.w, &app->bg.c.t_chrono.textPosition.h);

  SDL_RenderCopy(app->renderer, app->bg.c.t_chrono.textTexture, NULL, &app->bg.c.t_chrono.textPosition);
  SDL_DestroyTexture(app->bg.c.t_chrono.textTexture);
}

void guide_lot2(app *app, char *msg_guide){
  if (app->event.type == SDL_KEYDOWN && app->event.key.keysym.sym == SDLK_g)
  {
    app->bg.g.afficher = !app->bg.g.afficher;
  }
  if (app->bg.g.afficher)
  { // 1:afficher et 0:pas afficher
    // le mesage du guide:
    app->bg.g.instruction.textSurface = NULL;
    app->bg.g.instruction.textSurface = TTF_RenderText_Blended_Wrapped(app->bg.g.instruction.font, msg_guide, app->bg.g.instruction.color,1880);
    if (app->bg.g.instruction.textSurface == NULL)
    {
      sdl_erreur("Echec surface guide", TTF_GetError());
    }

    app->bg.g.instruction.textTexture = NULL;
    app->bg.g.instruction.textTexture = SDL_CreateTextureFromSurface(app->renderer, app->bg.g.instruction.textSurface);
    if (app->bg.g.instruction.textTexture == NULL)
    {
      sdl_erreur("Echec texture guide", SDL_GetError());
    }
    SDL_FreeSurface(app->bg.g.instruction.textSurface);

    SDL_QueryTexture(app->bg.g.instruction.textTexture, NULL, NULL, &app->bg.g.instruction.textPosition.w, &app->bg.g.instruction.textPosition.h);

    app->bg.g.instruction.textPosition.x = (fen_w / 2) - (app->bg.g.instruction.textPosition.w / 2);
    app->bg.g.instruction.textPosition.y = 18;

    // le cadre eli bech yethat fih el message:
    app->bg.g.cadre.h = app->bg.g.instruction.textPosition.h + 20;
    app->bg.g.cadre.w = app->bg.g.instruction.textPosition.w + 20;
    app->bg.g.cadre.x = app->bg.g.instruction.textPosition.x - 15;
    app->bg.g.cadre.y = app->bg.g.instruction.textPosition.y - 15;
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 190);
    SDL_RenderFillRect(app->renderer, &app->bg.g.cadre);
    //
    SDL_RenderCopy(app->renderer, app->bg.g.instruction.textTexture, NULL, &app->bg.g.instruction.textPosition);

    SDL_DestroyTexture(app->bg.g.instruction.textTexture);
  }
}

void quitter_lot2(app *app){
  SDL_DestroyTexture(app->bg.img.texture);
  if(app->bg.nbplt > 0){
    SDL_DestroyTexture(app->bg.plt[0].texture);
  }
  TTF_CloseFont(app->bg.c.t_chrono.font);
  TTF_CloseFont(app->bg.g.instruction.font);
  SDL_DestroyRenderer(app->renderer);
  SDL_DestroyWindow(app->window);
  IMG_Quit();
  TTF_Quit();
  SDL_Quit();
}

void gestion_evenements_score(app *app, char msg_guide[150]){
  while (SDL_PollEvent(&app->event)){
    if (app->event.type == SDL_QUIT){
      app->running = 0;
    }
    if (app->event.type == SDL_KEYDOWN && app->event.key.keysym.sym == SDLK_m){
      app->mode = !app->mode;
      if(app->mode == 1){//nkoso el cam mta jouer 1 ala thnin
        app->bg.cam1.w = fen_w / 2;
      }else{
        app->bg.cam1.w = fen_w;
      }
    }
    if (app->event.type == SDL_KEYDOWN){
      if (app->event.key.keysym.sym == SDLK_ESCAPE){
        app->running = 0;
      }
      if (app->mode == 0){
        strcpy(msg_guide, "ZQSD pour bouger et C pour destruction");
        switch (app->event.key.keysym.sym){
        case SDLK_d:
          if(app->p1.destRect.x >= app->bg.cam1.w * 0.75){
            scrolling(app, 1, 15, 1);
          }
          break;
        case SDLK_q:
          if(app->p1.destRect.x <= app->bg.cam1.w * 0.25){
            scrolling(app, 2, 15, 1);
          }
          break;
        case SDLK_z:
          scrolling(app, 3, 10, 1);
          break;
        case SDLK_s:
          scrolling(app, 4, 5, 1);
          break;
        case SDLK_c:
          destruction_plateforme(app, app->bg.cam1);
          break;
        }
      }
      else if (app->mode == 1){
        strcpy(msg_guide, "Les Fleches et N pour destruction pour le joueur a droite / ZQSD et C pour destruction pour le joueur a gauche");
        switch (app->event.key.keysym.sym){
        //jouer isar
        case SDLK_RIGHT:
          if (app->p2.destRect.x >= app->bg.cam2.w * 0.75){
            scrolling(app, 1, 15, 2);
          }
          break;
        case SDLK_LEFT:
          if (app->p2.destRect.x <= app->bg.cam2.w * 0.25){
            scrolling(app, 2, 15, 2);
          }
          break;
        case SDLK_UP:
          scrolling(app, 3, 10, 2);
          break;
        case SDLK_DOWN:
          scrolling(app, 4, 5, 2);
          break;
        case SDLK_n:
          destruction_plateforme(app, app->bg.cam2);
          break;
        //jouer imine
        case SDLK_d:
          if(app->p1.destRect.x >= app->bg.cam1.w * 0.75){
            scrolling(app, 1, 15, 1);
          }
          break;
        case SDLK_q:
          if(app->p1.destRect.x <= app->bg.cam1.w * 0.25){
            scrolling(app, 2, 15, 1);
          }  
          break;
        case SDLK_z:
          scrolling(app, 3, 10, 1);
          break;
        case SDLK_s:
          scrolling(app, 4, 5, 1);
          break;
        case SDLK_c:
          destruction_plateforme(app, app->bg.cam1);
          break;
        }
      }
      guide_lot2(app, msg_guide);
    }
  }
}

void animer_plateforme(app *app){
  for(int i = 0; i < app->bg.nbplt; i++){
    if(app->bg.plt[i].type == 1){//plat mobile
      if(app->bg.plt[i].direction == 0){
        app->bg.plt[i].pos.x += 1;
      }else{
        app->bg.plt[i].pos.x -= 1;
      }

      if(app->bg.plt[i].pos.x >= app->bg.plt[i].pos_init + 85){
        app->bg.plt[i].direction = 1;
      }
      else if(app->bg.plt[i].pos.x <= app->bg.plt[i].pos_init - 85){
        app->bg.plt[i].direction = 0;
      }
    }
  }
}

void destruction_plateforme(app *app, SDL_Rect cam_active){
  int nb_rest_d = 0;
  for(int i=0; i < app->bg.nbplt; i++){
    if(app->bg.plt[i].type == 2 && app->bg.plt[i].active == 1){
      nb_rest_d += 1;
      if( (app->bg.plt[i].pos.x >= cam_active.x && app->bg.plt[i].pos.x <= cam_active.x + cam_active.w) 
      && 
      (app->bg.plt[i].pos.y >= cam_active.y && app->bg.plt[i].pos.y <= cam_active.y + cam_active.h) ){
        app->bg.plt[i].active = 0;
      }
    }
  }
  if(nb_rest_d <= 1){
    app->niveau = 2;
    creation_lot2(app);
  }
}
