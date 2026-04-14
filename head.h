#ifndef HEAD_H
#define HEAD_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

// Sub-struct for Images
typedef struct {
    SDL_Texture *background;
    SDL_Texture *buttons[10];      // e.g., b1, b1Hover, ...
    SDL_Texture *mouse;
} Images;

// Sub-struct for Audio
typedef struct {
    Mix_Music *music;
    Mix_Chunk *click;
} Audio;

// Sub-struct for Text
typedef struct {
    TTF_Font *font;
} Text;

// Struct for Menu
typedef struct {
    Images img;
    Audio audio;
    Text text;

    SDL_Rect buttonRects[10];   // e.g., b1Rect, b1HoverRect ...
    SDL_Rect mouseRect;
} Menu;

// Struct for Settings
typedef struct {
    Images img;
    Audio audio;
    Text text;

    SDL_Rect buttonRects[5];    // adjust number based on settings buttons
    SDL_Rect mouseRect;
} Settings;
// Function declarations
int init_all(SDL_Window **window, SDL_Renderer **renderer);
int load_assets(SDL_Renderer *r, Menu *menu, Settings *settings);
void render_menu(SDL_Renderer *r, Menu *menu, int mx, int my);
void render_settings(SDL_Renderer *r, Settings *settings, int mx, int my);
void cleanup(SDL_Window *window, SDL_Renderer *r, Menu *menu, Settings *settings);

#endif
