#include "head.h"
#include "mission1.h"
#include "functions.h"

// Global volume
static int currentVolume = 64;
static int isFullscreen = 0;

// Change volume
void changeVolume(int amount)
{
    currentVolume += amount;
    if (currentVolume > 128) currentVolume = 128;
    if (currentVolume < 0) currentVolume = 0;

    Mix_VolumeMusic(currentVolume);
}

int main(void)
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Event e;

    Menu menu = {0};
    Settings settings = {0};

    Mix_Music *mission1_music = NULL;

    int state = 0;       // 0 = MENU, 1 = SETTINGS, 2 = MISSION1
    int prevState = 0;

    int running = 1;
    int mx = 0, my = 0;

    // Initialize SDL
    if (!init_all(&window, &renderer)) return 1;

    // Load assets
    if (!load_assets(renderer, &menu, &settings))
    {
        cleanup(window, renderer, &menu, &settings);
        return 1;
    }

    // Load mission1 music
    mission1_music = Mix_LoadMUS("mission1.mp3");
    if (!mission1_music)
    {
        printf("Failed to load mission1 music: %s\n", Mix_GetError());
    }

    // Start menu music
    if (menu.audio.music) Mix_PlayMusic(menu.audio.music, -1);
    Mix_VolumeMusic(currentVolume);

    // Optional: fullscreen
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    isFullscreen = 1;

    while (running)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                running = 0;
            }

            if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_ESCAPE)
                {
                    if (state == 1) // SETTINGS
                    {
                        Mix_PlayChannel(-1, settings.audio.click, 0);
                        state = 0;
                    }
                    else if (state == 2) // MISSION1
                    {
                        // We'll handle ESC inside mission loop
                    }
                    else // MENU
                    {
                        running = 0;
                    }
                }

                if (state == 0 && e.key.keysym.sym == SDLK_s) // MENU -> SETTINGS
                {
                    Mix_PlayChannel(-1, menu.audio.click, 0);
                    state = 1;
                }
                if (state == 0 && e.key.keysym.sym == SDLK_p) // MENU -> play
                {
                    Mix_PlayChannel(-1, menu.audio.click, 0);
                    state = 2;
                }

                if (state == 1) // SETTINGS
                {
                    if (e.key.keysym.sym == SDLK_PLUS || e.key.keysym.sym == SDLK_EQUALS)
                        changeVolume(8);
                    if (e.key.keysym.sym == SDLK_MINUS)
                        changeVolume(-8);
                }
            }

            if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                SDL_Point p = { e.button.x, e.button.y };

                if (state == 0) // MENU
                {
                    if (SDL_PointInRect(&p, &menu.buttonRects[0])) // Play Mission1
                    {
                        Mix_PlayChannel(-1, menu.audio.click, 0);
                        state = 2; // Enter mission
                    }

                    if (SDL_PointInRect(&p, &menu.buttonRects[1])) // Settings
                    {
                        Mix_PlayChannel(-1, menu.audio.click, 0);
                        state = 1;
                    }

                    if (SDL_PointInRect(&p, &menu.buttonRects[4])) // Quit
                        running = 0;
                }
                else if (state == 1) // SETTINGS
                {
                    if (SDL_PointInRect(&p, &settings.buttonRects[0])) // Back
                    {
                        Mix_PlayChannel(-1, settings.audio.click, 0);
                        state = 0;
                    }
                    if (SDL_PointInRect(&p, &settings.buttonRects[1])) // Vol +
                        changeVolume(8);
                    if (SDL_PointInRect(&p, &settings.buttonRects[2])) // Vol -
                        changeVolume(-8);
                    if (SDL_PointInRect(&p, &settings.buttonRects[3])) // Fullscreen
                    {
                        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                        isFullscreen = 1;
                    }
                    if (SDL_PointInRect(&p, &settings.buttonRects[4])) // Windowed
                    {
                        SDL_SetWindowFullscreen(window, 0);
                        isFullscreen = 0;
                    }
                }
            }
        }

        SDL_GetMouseState(&mx, &my);

        // Music switching when changing state
        if (state != prevState)
        {
            Mix_HaltMusic();

            if (state == 0 && menu.audio.music)
                Mix_PlayMusic(menu.audio.music, -1);
            else if (state == 1 && settings.audio.music)
                Mix_PlayMusic(settings.audio.music, -1);
            // Mission1 music will be started inside its loop
        }

        prevState = state;

        // Render or handle states
        if (state == 0)
        {
            render_menu(renderer, &menu, mx, my);
        }
        else if (state == 1)
        {
            render_settings(renderer, &settings, mx, my);
        }
        else if (state == 2) // MISSION1
        {
            // Start mission1 music
            if (mission1_music) Mix_PlayMusic(mission1_music, -1);

            int missionRunning = 1;
            init_mission1(renderer);

            while (missionRunning)
            {
                SDL_Event me;
                while (SDL_PollEvent(&me))
                {
                    if (me.type == SDL_QUIT)
                    {
                        missionRunning = 0;
                        running = 0; // exit program
                    }

                    if (me.type == SDL_KEYDOWN)
                    {
                        if (me.key.keysym.sym == SDLK_ESCAPE)
                        {
                            cleanup_mission1();
                            missionRunning = 0;
                            state = 0; // back to menu
                        }
                    }

                    if (me.type == SDL_MOUSEBUTTONDOWN)
                    {
                        // handle mission1 mouse events if needed
                    }
                }

                update_mission1();
                render_mission1(renderer);

                SDL_Delay(16); // ~60 FPS
            }

            Mix_HaltMusic(); // stop mission music
        }

        SDL_Delay(16); // ~60 FPS
    }

    // Cleanup
    if (mission1_music) Mix_FreeMusic(mission1_music);
    cleanup(window, renderer, &menu, &settings);

    return 0;
}
