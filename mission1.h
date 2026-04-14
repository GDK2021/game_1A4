#ifndef MISSION1_H
#define MISSION1_H
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL.h>

#define WALK_FRAMES 36
#define IDLE_FRAMES 36
#define BOW_FRAMES 36
#define DEATH_FRAMES 36

typedef struct {
    SDL_Rect rect;
    int frame;
    int tick;
    int facingRight;
    int moving;
    int minX;
    int maxX;
    int detectionRange;
    int health;
    int alive;
    int dying;
    int deathFrame;
    int stopDistance;
    SDL_Texture *hit[36];
    int hitFrame;
    int isHit;
    int isAttacking;
    int attackFrame;
    int attackCooldown;   // timer between attacks
} Enemy;



typedef struct {
    SDL_Texture *background;
    SDL_Texture *enemyIcon;
    SDL_Texture *icon;       
    SDL_Rect rect;           // Where the map sits on the screen
    int levelWidth;          // 1920
    int padding;             
} Minimap;



typedef struct {
    SDL_Texture *background;
    SDL_Texture *idle[IDLE_FRAMES];
    SDL_Texture *walk[WALK_FRAMES];
    SDL_Texture *bow[BOW_FRAMES];
    SDL_Texture *death[BOW_FRAMES];
    SDL_Texture *aimTarget;
    SDL_Texture *attack1[36];
    SDL_Texture *attack2[36];
    SDL_Texture *attack3[36];
    SDL_Texture *playerHit[36];
    SDL_Texture *healthBarTexture;

    SDL_Rect playerRect;

    int frame;
    int tick;
    int isMoving;
    int facingRight;

    // Bow attack
    int isAttacking;
    int attackFrame;
    int attackFinished;

    // Aim
    int aiming;
    int aimActive;
    int aimHit;
    int mouseX;
    int mouseY;
    
    // Combo system
    int comboStep;        // 0 = none, 1 = attack1, 2 = attack2, 3 = attack3
    int comboFrame;
    int comboTimer;       // time window to chain next attack
    int comboQueued;      // player pressed attack during combo
    Mix_Chunk *combo3;
    Mix_Chunk* bowSound;
    Mix_Chunk* arrowSound;
    Mix_Chunk* hitSound;
    int combo3Played;
    Enemy enemy;
    
    int isPlayerHit;
    float playerHitFrame;
    
    //health
    int health;
    int alive;
    int dying;
    float deathFrame; // Use float for smooth animation speed
    
    Minimap map;
    
    //walk soundeffect
    Mix_Chunk *grassSfx;
    int playerWalkChannel; // To keep track of the specific channel
    int enemyWalkChannel;
    
    
    
} Mission1;

void init_mission1(SDL_Renderer *renderer);
void update_mission1(void);
void render_mission1(SDL_Renderer *renderer);
void cleanup_mission1(void);
void render_minimap(SDL_Renderer *renderer, Minimap *m, SDL_Rect playerRect, int facingRight, SDL_Rect enemyRect, int enemyAlive);

#endif
