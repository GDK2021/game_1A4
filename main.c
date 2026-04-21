#include "header.h"

int main()
{
    app a;
    Game g;

    init(&g,&a);

    int level = 1;
    loadLevel(&g,&a,level);

    SDL_Rect walls[3] = { {530,300,300,200}, {1100,400,200,200}, {900,700,300,100} };

    Enemy enemies[2] = { {700,600,40,40}, {1520,600,40,40} };

    SDL_Rect movingObs[2] = {  {500,400,60,60}, {800,600,60,60} };

    int dir[2] = {1,-1};

    int score = 100;
    int vie = 5;

    SDL_Event ev;
    int run = 1;

    while(run)
    {
        while(SDL_PollEvent(&ev))
        {
            if(ev.type == SDL_QUIT)
                run = 0;

            if(ev.type == SDL_KEYDOWN)
            {
                int ox = g.player.x;
                int oy = g.player.y;

                if(ev.key.keysym.sym == SDLK_RIGHT) g.player.x += 10;
                if(ev.key.keysym.sym == SDLK_LEFT)  g.player.x -= 10;
                if(ev.key.keysym.sym == SDLK_UP)    g.player.y -= 10;
                if(ev.key.keysym.sym == SDLK_DOWN)  g.player.y += 10;

                /* COLLISION MASK */
                if(level == 1)
                {
                    if(collisionMask(g.mask, g.player))
                    {
                        g.player.x = ox;
                        g.player.y = oy;
                    }
                }
                
                /* WALLS */
                if(level == 1)
                {
                    for(int i=0;i<3;i++)
                        if(checkCollision(g.player, walls[i]))
                        {
                            g.player.x = ox;
                            g.player.y = oy;
                        }
                }

                /* ENEMIES */
                if(level == 1)
                {
                    for(int i=0;i<2;i++)
                        if(checkCollision(g.player, enemies[i].rect))
                        {
                            vie--;
                            score--;
                            printf("vie :%d score :%d ",vie,score);
                        }
                }

                /* LEVEL 2 */
                if(level == 2)
                {
                    for(int i=0;i<2;i++)
                        if(checkCollision(g.player, movingObs[i]))
                        {
                            printf("GAME OVER niveau 2\n");
                            run = 0;
                        }
                }

                /* PASSAGE NIVEAU */
                if(vie <= 0 || score < 0)
                {
                    if(level == 1)
                    {
                        level = 2;
                        loadLevel(&g,&a,level);

                        g.player.x = 100;
                        g.player.y = 100;

                        vie = 5;
                        score = 10;
                    }
                    else run = 0;
                }

                updateCamera(&g);
            }
        }

        /* MOUVEMENT OBSTACLES */
        if(level == 2)
        {
            for(int i=0;i<2;i++)
            {
                movingObs[i].x += dir[i]*2;
                if(movingObs[i].x > 1000) dir[i] = -1;
                if(movingObs[i].x < 400)  dir[i] = 1;
            }
        }

        renderGame(a,g,walls,3,enemies,2,movingObs,2);
    }

    SDL_DestroyTexture(g.map);
    SDL_DestroyTexture(g.minimap);
    SDL_FreeSurface(g.mask);

    SDL_DestroyRenderer(a.renderer);
    SDL_DestroyWindow(a.window);

    IMG_Quit();
    SDL_Quit();
    
    return 0;
}
