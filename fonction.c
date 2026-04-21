#include "header.h"

void init(Game *g, app *a)
{
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    a->window = SDL_CreateWindow("SDL Game",SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);

    a->renderer = SDL_CreateRenderer(a->window, -1, SDL_RENDERER_ACCELERATED);

    g->map = NULL;
    g->minimap = NULL;
    g->mask = NULL;

    g->player = (SDL_Rect){100,100,40,40};
    g->camera = (SDL_Rect){0,0,800,600};
    g->minimapRect = (SDL_Rect){600,20,180,120};
}

void loadLevel(Game *g, app *a, int level)
{
    
    if(g->map) SDL_DestroyTexture(g->map);
    if(g->minimap) SDL_DestroyTexture(g->minimap);
    if(g->mask) SDL_FreeSurface(g->mask);

    if(level == 1)
    {
        g->map = IMG_LoadTexture(a->renderer, "map2.png");
        if(!g->map) 
           printf("map NULL\n");
        g->minimap = IMG_LoadTexture(a->renderer, "minimap2.png");
        if(!g->minimap) 
           printf("minimap NULL\n");
        g->mask = IMG_Load("map2bmp.bmp");
        if(!g->mask) 
           printf("mask NULL\n");
    }
    else if(level == 2)
{
    g->map = IMG_LoadTexture(a->renderer, "niv2.png");
    if(!g->map) { printf("map2 error: %s\n", IMG_GetError()); exit(1); }

    g->minimap = IMG_LoadTexture(a->renderer, "mininiv2.png");
    if(!g->minimap) { printf("minimap2 error\n"); exit(1); }

    g->mask = IMG_Load("niv2.bmp");
    if(!g->mask) { printf("mask2 error\n"); exit(1); }
}
}

void updateCamera(Game *g)
{
    g->camera.x = g->player.x - 400;
    g->camera.y = g->player.y - 300;

    if(g->camera.x < 0) g->camera.x = 0;
    if(g->camera.y < 0) g->camera.y = 0;
}

int checkCollision(SDL_Rect A, SDL_Rect B)
{
    if (A.x + A.w < B.x || A.x > B.x + B.w || A.y + A.h < B.y || A.y > B.y + B.h) 
        return 0;
        
    return 1;
}

int collisionMask(SDL_Surface *mask, SDL_Rect p)
{
    if(!mask) return 0;

    SDL_Rect pts[4] = {
        {p.x,p.y},
        {p.x+p.w,p.y},
        {p.x,p.y+p.h},
        {p.x+p.w,p.y+p.h}
    };

    for(int i=0;i<4;i++)
    {
        if(pts[i].x < 0 || pts[i].y < 0 || pts[i].x >= mask->w || pts[i].y >= mask->h)
            continue;

        Uint8 *pixel = (Uint8*)mask->pixels + pts[i].y * mask->pitch + pts[i].x * mask->format->BytesPerPixel;

        Uint32 color;
        memcpy(&color, pixel, mask->format->BytesPerPixel);

        Uint8 r,g,b;
        SDL_GetRGB(color, mask->format, &r,&g,&b);

        if(r < 10 && g < 10 && b < 10)
        {
            
            return 1;
        }
    }


    return 0;
}


void renderGame(app a, Game g,SDL_Rect *walls, int nWalls,Enemy *enemies, int nEnemies,SDL_Rect *movingObs, int nObs)
{
    SDL_RenderClear(a.renderer);

    SDL_RenderCopy(a.renderer, g.map, &g.camera, NULL);


    for(int i=0;i<nWalls;i++)
    {
        SDL_Rect w = walls[i];
        w.x -= g.camera.x;
        w.y -= g.camera.y;

        SDL_SetRenderDrawColor(a.renderer,255,255,255,255);
        SDL_RenderFillRect(a.renderer,&w);
    }

    
    for(int i=0;i<nEnemies;i++)
    {
        SDL_Rect e = enemies[i].rect;
        e.x -= g.camera.x;
        e.y -= g.camera.y;

        SDL_SetRenderDrawColor(a.renderer,0,0,255,255);
        SDL_RenderFillRect(a.renderer,&e);
    }

    
    for(int i=0;i<nObs;i++)
    {
        SDL_Rect o = movingObs[i];
        o.x -= g.camera.x;
        o.y -= g.camera.y;

        SDL_SetRenderDrawColor(a.renderer,0,255,0,255);
        SDL_RenderFillRect(a.renderer,&o);
    }
    float taillx = (float)g.minimapRect.w / 2048;
    float tailly = (float)g.minimapRect.h / 2048;

  
    SDL_Rect p = g.player;
    p.x -= g.camera.x;
    p.y -= g.camera.y;

    SDL_SetRenderDrawColor(a.renderer,255,0,0,255);
    SDL_RenderFillRect(a.renderer,&p);

    
    SDL_RenderCopy(a.renderer, g.minimap, NULL, &g.minimapRect);
    
    SDL_Rect Play = { g.minimapRect.x + g.player.x * taillx, g.minimapRect.y + g.player.y * tailly,  4,4};

    SDL_SetRenderDrawColor(a.renderer,255,0,0,255);
    SDL_RenderFillRect(a.renderer,&Play);
    
    /**/
    for(int i=0;i<nWalls;i++)
    {
        SDL_Rect wal={g.minimapRect.x + walls[i].x *taillx ,g.minimapRect.y + walls[i].y *tailly,6,6};
        SDL_SetRenderDrawColor(a.renderer,255,0,255,255);
        SDL_RenderFillRect(a.renderer,&wal);
     }
    /**/
    for(int i=0;i<nEnemies;i++)
    {
        SDL_Rect enm = {g.minimapRect.x + enemies[i].rect.x * taillx,g.minimapRect.y + enemies[i].rect.y * tailly, 5,5};
        SDL_SetRenderDrawColor(a.renderer,0,0,255,255);
        SDL_RenderFillRect(a.renderer,&enm);
    }
    /**/
    for(int i=0;i<nObs;i++)
   {
    SDL_Rect obg = { g.minimapRect.x + movingObs[i].x * taillx, g.minimapRect.y + movingObs[i].y * tailly,4,4};

    SDL_SetRenderDrawColor(a.renderer,0,255,0,255);
    SDL_RenderFillRect(a.renderer,&obg);
   }
   
    
    SDL_RenderPresent(a.renderer);
}



