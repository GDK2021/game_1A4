#include "functions.h"
#include "head.h"
#include <stdio.h>

/* ========================================================= */
/*                    TEXTURE LOADER                         */
/* ========================================================= */
SDL_Texture *load_texture(SDL_Renderer *renderer, const char *path)
{
    SDL_Surface *surface = IMG_Load(path);
    if (!surface)
    {
        printf("IMG_Load failed for %s: %s\n", path, IMG_GetError());
        return NULL;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture)
        printf("SDL_CreateTextureFromSurface failed: %s\n", SDL_GetError());

    return texture;
}

/* ========================================================= */
/*                    INIT SDL                               */
/* ========================================================= */
int init_all(SDL_Window **window, SDL_Renderer **renderer)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return 0;
    }

    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) == 0)
    {
        printf("IMG_Init failed: %s\n", IMG_GetError());
        return 0;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) < 0)
    {
        printf("Mix_OpenAudio failed: %s\n", Mix_GetError());
        return 0;
    }

    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);

    *window = SDL_CreateWindow("The Cold Revenge",
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               1920, 1080,
                               SDL_WINDOW_SHOWN);

    if (!*window)
    {
        printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
        return 0;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!*renderer)
    {
        printf("SDL_CreateRenderer failed: %s\n", SDL_GetError());
        return 0;
    }

    SDL_SetRenderDrawBlendMode(*renderer, SDL_BLENDMODE_BLEND);
    SDL_ShowCursor(SDL_DISABLE);

    if (TTF_Init() == -1)
    {
        printf("TTF_Init failed: %s\n", TTF_GetError());
        return 0;
    }

    return 1;
}

/* ========================================================= */
/*                    LOAD ASSETS                            */
/* ========================================================= */
int load_assets(SDL_Renderer *renderer, Menu *menu, Settings *settings)
{
    int w, h;

    // ------------------- MENU -------------------
    menu->img.background = load_texture(renderer, "background.png");

    const char *menu_btn_paths[10] = {
        "play.png", "play_hover.png",
        "settings.png", "settings_hover.png",
        "score.png", "score_hover.png",
        "story.png", "story_hover.png",
        "quit.png", "quit_hover.png"
    };

    for (int i = 0; i < 10; i++)
        menu->img.buttons[i] = load_texture(renderer, menu_btn_paths[i]);

    menu->buttonRects[0] = (SDL_Rect){50, 400, 250, 120};
    menu->buttonRects[1] = (SDL_Rect){50, 540, 250, 120};
    menu->buttonRects[2] = (SDL_Rect){50, 680, 250, 120};
    menu->buttonRects[3] = (SDL_Rect){50, 820, 250, 120};
    menu->buttonRects[4] = (SDL_Rect){1620, 910, 250, 120};

    menu->img.mouse = load_texture(renderer, "mouse.png");
    SDL_QueryTexture(menu->img.mouse, NULL, NULL, &w, &h);
    menu->mouseRect.w = w / 10;
    menu->mouseRect.h = h / 10;
    SDL_SetTextureBlendMode(menu->img.mouse, SDL_BLENDMODE_BLEND);

    menu->audio.music = Mix_LoadMUS("sounds.mp3");
    menu->audio.click = Mix_LoadWAV("ce.mp3");
    menu->text.font = TTF_OpenFont("arial.ttf", 32);

    // ------------------- SETTINGS -------------------
    settings->img.background = load_texture(renderer, "bks.png");

    const char *settings_btn_paths[10] = {
        "bmm.png", "bmmc.png",
        "bi.png", "bic.png",
        "bd.png", "bdc.png",
        "bfs.png", "bfsc.png",
        "bn.png", "bnc.png"
    };

    for (int i = 0; i < 10; i++)
        settings->img.buttons[i] = load_texture(renderer, settings_btn_paths[i]);

    SDL_QueryTexture(settings->img.buttons[2], NULL, NULL, &w, &h);
    settings->buttonRects[1] = (SDL_Rect){1300, 270, w / 12, h / 12};

    SDL_QueryTexture(settings->img.buttons[4], NULL, NULL, &w, &h);
    settings->buttonRects[2] = (SDL_Rect){1300, 430, w / 12, h / 12};

    SDL_QueryTexture(settings->img.buttons[6], NULL, NULL, &w, &h);
    settings->buttonRects[3] = (SDL_Rect){1300, 700, w / 12, h / 12};

    SDL_QueryTexture(settings->img.buttons[8], NULL, NULL, &w, &h);
    settings->buttonRects[4] = (SDL_Rect){1300, 860, w / 12, h / 12};

    SDL_QueryTexture(settings->img.buttons[0], NULL, NULL, &w, &h);
    settings->buttonRects[0] = (SDL_Rect){100, 850, w / 17, h / 17};

    // Share mouse texture and font
    settings->img.mouse = menu->img.mouse;
    settings->mouseRect.w = menu->mouseRect.w;
    settings->mouseRect.h = menu->mouseRect.h;
    settings->text.font = menu->text.font;
    settings->audio.music = Mix_LoadMUS("theme.mp3");
    settings->audio.click = menu->audio.click;

    return 1;
}

/* ========================================================= */
/*                    RENDER FUNCTIONS                       */
/* ========================================================= */
static void render_buttons_with_text(SDL_Renderer *renderer, SDL_Texture **buttons, SDL_Rect *buttonRects, int mx, int my, TTF_Font *font)
{
    SDL_Color white = {255, 255, 255, 255};
    const char *btn_texts[5] = {"Play", "Settings", "Score", "Story", "Quit"};

    for (int i = 0; i < 5; i++)
    {
        SDL_Texture *tex = SDL_PointInRect(&(SDL_Point){mx, my}, &buttonRects[i])
                            ? buttons[i * 2 + 1]
                            : buttons[i * 2];
        SDL_RenderCopy(renderer, tex, NULL, &buttonRects[i]);

        // Render centered text
        SDL_Surface *surf = TTF_RenderText_Solid(font, btn_texts[i], white);
        SDL_Texture *text_tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_Rect textRect;
        textRect.w = surf->w;
        textRect.h = surf->h;
        textRect.x = buttonRects[i].x + (buttonRects[i].w - surf->w) / 2;
        textRect.y = buttonRects[i].y + (buttonRects[i].h - surf->h) / 2-10;
        SDL_RenderCopy(renderer, text_tex, NULL, &textRect);
        SDL_FreeSurface(surf);
        SDL_DestroyTexture(text_tex);
    }
}

void render_menu(SDL_Renderer *renderer, Menu *menu, int mx, int my)
{
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, menu->img.background, NULL, NULL);

    render_buttons_with_text(renderer, menu->img.buttons, menu->buttonRects, mx, my, menu->text.font);

    menu->mouseRect.x = mx - menu->mouseRect.w / 2;
    menu->mouseRect.y = my - menu->mouseRect.h / 2;
    SDL_RenderCopy(renderer, menu->img.mouse, NULL, &menu->mouseRect);

    SDL_RenderPresent(renderer);
}

void render_settings(SDL_Renderer *renderer, Settings *settings, int mx, int my)
{
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, settings->img.background, NULL, NULL);

    // Render buttons without text
    for (int i = 0; i < 5; i++)
    {
        SDL_Texture *tex = SDL_PointInRect(&(SDL_Point){mx, my}, &settings->buttonRects[i])
                            ? settings->img.buttons[i * 2 + 1]
                            : settings->img.buttons[i * 2];
        SDL_RenderCopy(renderer, tex, NULL, &settings->buttonRects[i]);
    }

    // Render mouse
    settings->mouseRect.x = mx - settings->mouseRect.w / 2;
    settings->mouseRect.y = my - settings->mouseRect.h / 2;
    SDL_RenderCopy(renderer, settings->img.mouse, NULL, &settings->mouseRect);

    SDL_RenderPresent(renderer);
}

/* ========================================================= */
/*                    CLEANUP                                 */
/* ========================================================= */
void cleanup(SDL_Window *window, SDL_Renderer *renderer, Menu *menu, Settings *settings)
{
    Mix_HaltMusic();

    if (menu->audio.music) Mix_FreeMusic(menu->audio.music);
    if (settings->audio.music) Mix_FreeMusic(settings->audio.music);
    if (menu->audio.click) Mix_FreeChunk(menu->audio.click);

    if (menu->img.background) SDL_DestroyTexture(menu->img.background);
    if (settings->img.background) SDL_DestroyTexture(settings->img.background);
    if (menu->img.mouse) SDL_DestroyTexture(menu->img.mouse);

    for (int i = 0; i < 10; i++)
    {
        if (menu->img.buttons[i]) SDL_DestroyTexture(menu->img.buttons[i]);
        if (settings->img.buttons[i]) SDL_DestroyTexture(settings->img.buttons[i]);
    }

    if (menu->text.font) TTF_CloseFont(menu->text.font);

    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);

    Mix_CloseAudio();
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
}
