#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include "mission1.h"
#include "functions.h"
#include <SDL2/SDL_mixer.h>
#include "serie.h"
#include <fcntl.h>
#include <unistd.h>

static Mission1 m1;
static int serial_fd = -1;
static int btnLeft = 1;
static int btnRight = 1;
static char serialBuffer[100];

// ----------------------
// INIT
// ----------------------
void init_mission1(SDL_Renderer *renderer)
{
    //mini map
m1.map.background = load_texture(renderer, "minim.png");
    m1.map.icon = load_texture(renderer, "cm.png");
    m1.map.enemyIcon = load_texture(renderer, "em.png");
    
    m1.map.levelWidth = 1920*3; 
    m1.map.padding = 40; 
    m1.map.rect.w = 450*1.2; 
    m1.map.rect.h = 300*1.2; 
    m1.map.rect.x = (1920 - m1.map.rect.w) / 2; 
    m1.map.rect.y = -60;
    char path[256];

    // -------- Enemy --------
    m1.enemy.rect.x = 900;
    m1.enemy.rect.y = 460;
    m1.enemy.rect.w = 400 / 1.5;
    m1.enemy.rect.h = 690 / 1.5;
    m1.enemy.frame = 0;
    m1.enemy.tick = 0;
    m1.enemy.facingRight = 1;
    m1.enemy.moving = 1;
    m1.enemy.minX = 900;
    m1.enemy.maxX = 1200;
    m1.enemy.detectionRange = 400;
    m1.enemy.health = 20;
    m1.enemy.alive = 1;
    m1.enemy.dying = 0;
    m1.enemy.deathFrame = 0;
    m1.enemy.stopDistance = 200;
    m1.combo3Played = 0;
    m1.enemy.isAttacking = 0;
    m1.enemy.attackFrame = 0;
    m1.enemy.attackCooldown = 0;
    
    //----------healthbar---------
    m1.healthBarTexture = load_texture(renderer, "hb.png");

    // -------- Background --------
    m1.background = load_texture(renderer, "m1.png");

    // -------- Idle --------
    for (int i = 0; i < IDLE_FRAMES; i++) {
        snprintf(path, sizeof(path), "idle/frame_%02d_delay-0.05s.gif", i);
        m1.idle[i] = load_texture(renderer, path);
    }

    // -------- Walk --------
    for (int i = 0; i < WALK_FRAMES; i++) {
        snprintf(path, sizeof(path), "walk/frame_%02d_delay-0.07s.gif", i);
        m1.walk[i] = load_texture(renderer, path);
    }

    // -------- Bow --------
    for (int i = 0; i < BOW_FRAMES; i++) {
        snprintf(path, sizeof(path), "bow/frame_%02d_delay-0.05s.gif", i);
        m1.bow[i] = load_texture(renderer, path);
    }

    // -------- Death --------
    for (int i = 0; i < WALK_FRAMES; i++) {
        snprintf(path, sizeof(path), "death/frame_%02d_delay-0.05s.gif", i);
        m1.death[i] = load_texture(renderer, path);
    }

    // -------- Combo attacks --------
    for (int i = 0; i < 36; i++) {
        snprintf(path, sizeof(path), "attack1/frame_%02d_delay-0.04s.gif", i);
        m1.attack1[i] = load_texture(renderer, path);

        snprintf(path, sizeof(path), "attack2/frame_%02d_delay-0.04s.gif", i);
        m1.attack2[i] = load_texture(renderer, path);

        snprintf(path, sizeof(path), "attack3/frame_%02d_delay-0.04s.gif", i);
        m1.attack3[i] = load_texture(renderer, path);
    }

    // -------- Player --------
    m1.playerRect.x = 100;
    m1.playerRect.y = 460;
    m1.playerRect.w = 400 / 1.5;
    m1.playerRect.h = 690 / 1.5;

    m1.frame = 0;
    m1.tick = 0;
    m1.isMoving = 0;
    m1.facingRight = 1;

    // Hit state
    m1.isPlayerHit = 0;
    m1.playerHitFrame = 0;

    // Bow
    m1.isAttacking = 0;
    m1.attackFrame = 0;
    m1.attackFinished = 0;

    // Aim
    m1.aimActive = 0;
    m1.mouseX = 0;
    m1.mouseY = 0;
    m1.aimTarget = load_texture(renderer, "t.png");

    // Combo
    m1.comboStep = 0;
    m1.comboFrame = 0;
    m1.comboQueued = 0;
    
    // SFX
    m1.grassSfx = Mix_LoadWAV("ge.mp3");
    m1.playerWalkChannel = -1;
    m1.enemyWalkChannel = -1;
    m1.hitSound = Mix_LoadWAV("hite.mp3");
    m1.bowSound = Mix_LoadWAV("bowe.mp3");
    m1.combo3 = Mix_LoadWAV("attack3/combo3.mp3");
    m1.arrowSound = Mix_LoadWAV("ae.mp3");
    if (!m1.combo3) {
        printf("Failed to load combo3.mp3! SDL_mixer Error: %s\n", Mix_GetError());
    }

    // Load Player Hit Textures
    for (int i = 0; i < 36; i++) {
        snprintf(path, sizeof(path), "hit/frame_%02d_delay-0.05s.gif", i); // Ensure this path is correct
        m1.playerHit[i] = load_texture(renderer, path);
    }

    // Enemy Hit Textures
    for (int i = 0; i < 36; i++) {
        snprintf(path, sizeof(path), "hit/frame_%02d_delay-0.05s.gif", i);
        m1.enemy.hit[i] = load_texture(renderer, path);
    }
    m1.health = 20;
    m1.alive = 1;
    m1.dying = 0;
    m1.deathFrame = 0;
    
    
    //controller
	serial_fd = serialport_init("/dev/ttyUSB0", 115200);
	fcntl(serial_fd, F_SETFL, O_NONBLOCK);
if (serial_fd == -1) {
    printf("Serial not connected!\n");
}
    
    
}


//mini map
void render_minimap(SDL_Renderer *renderer, Minimap *m, SDL_Rect playerRect, int facingRight, SDL_Rect enemyRect, int enemyAlive) {
    // Safety check
    if (!m->background || !m->icon) return;

    // 1. Draw the Minimap Background (The frame/map itself)
    SDL_RenderCopy(renderer, m->background, NULL, &m->rect);

    // --- POSITIONING LOGIC ---
    int verticalOffset = 15; 
    int pathY = m->rect.y + (m->rect.h / 2) + verticalOffset;

    // trackWidth is the area between the left and right padding
    int trackWidth = m->rect.w - (m->padding * 2);

    // 2. Draw Enemy
    if (enemyAlive && m->enemyIcon) {
        float eProgress = (float)enemyRect.x / (float)m->levelWidth;
        
        // Clamp progress between 0 and 1
        if (eProgress < 0) eProgress = 0;
        if (eProgress > 1) eProgress = 1;

        SDL_Rect eIconDst;
        eIconDst.w = 10;
        eIconDst.h = 10;
        eIconDst.x = m->rect.x + m->padding + (int)(eProgress * trackWidth) - (eIconDst.w / 2);
        
        // Center the ball vertically on the pathY line
        eIconDst.y = pathY - (eIconDst.h / 2); 

        SDL_RenderCopy(renderer, m->enemyIcon, NULL, &eIconDst);
    }

    // 3. Draw Player Icon
    float pProgress = (float)playerRect.x / (float)m->levelWidth;
    
    // Clamp progress
    if (pProgress < 0) pProgress = 0;
    if (pProgress > 1) pProgress = 1;

    SDL_Rect pIconDst;
    pIconDst.w = 10; 
    pIconDst.h = 10; 
    pIconDst.x = m->rect.x + m->padding + (int)(pProgress * trackWidth) - (pIconDst.w / 2);
    
    // Center the character vertically on the pathY line
    pIconDst.y = pathY - (pIconDst.h / 2);

    // Flip the character icon based on movement direction
    SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

    SDL_RenderCopyEx(renderer, m->icon, NULL, &pIconDst, 0, NULL, flip);
}


// ----------------------
// UPDATE
// ----------------------
void update_mission1(void)
{
int n = read(serial_fd, serialBuffer, sizeof(serialBuffer) - 1);
if (n > 0) {
    serialBuffer[n] = '\0';

    if (strstr(serialBuffer, "B1=0")) btnLeft = 0;
    if (strstr(serialBuffer, "B1=1")) btnLeft = 1;

    if (strstr(serialBuffer, "B2=0")) btnRight = 0;
    if (strstr(serialBuffer, "B2=1")) btnRight = 1;
}


    const Uint8 *keystate = SDL_GetKeyboardState(NULL);
    int mouseState = SDL_GetMouseState(&m1.mouseX, &m1.mouseY);

    // -------- Death Logic (Block all other inputs if dead) --------
    if (m1.dying) {
        if (m1.deathFrame < DEATH_FRAMES - 1) {
            m1.deathFrame += 1.0f; // Adjust speed of player falling down
        }
    }

    // -------- Combo Input --------
    // Added m1.alive check
    if (keystate[SDL_SCANCODE_J] && !m1.isPlayerHit && m1.alive) {
        if (m1.comboStep == 0) {
            m1.comboStep = 1;
            m1.comboFrame = 0;
            m1.comboQueued = 0;
        } 
        else if (m1.comboStep < 3 && m1.comboFrame >= 30) {
            m1.comboQueued = 1;
        }
    }

    // -------- Combo Logic --------
    if (m1.comboStep > 0) {
        if (m1.isPlayerHit) {
            m1.comboStep = 0;
            m1.comboFrame = 0;
            m1.comboQueued = 0;
        } else {
            m1.tick++;  
            float ticksPerFrame = 0.0; 
            if (m1.tick >= ticksPerFrame) {
                m1.comboFrame += 2;
                m1.tick = 0; 
                int maxFrames = 36; 
                if (m1.comboFrame >= maxFrames) {
                    if (m1.comboQueued && m1.comboStep < 3) {
                        m1.comboStep++;
                        m1.comboFrame = 0;
                        m1.comboQueued = 0;
                    } else {
                        m1.comboStep = 0;
                        m1.comboFrame = 0;
                        m1.comboQueued = 0;
                    }
                }
            }
        }
    }

// -------- Apply Combo Damage--------
if (m1.enemy.alive && m1.comboStep > 0) {
    // 1. Calculate distance
    int playerCenterX = m1.playerRect.x + m1.playerRect.w / 2;
    int enemyCenterX = m1.enemy.rect.x + m1.enemy.rect.w / 2;
    int distanceToEnemy = abs(playerCenterX - enemyCenterX);

    // 2. Check Facing Direction
    int isEnemyInFront = 0;
    if (m1.facingRight && enemyCenterX > playerCenterX) {
        isEnemyInFront = 1; // Player faces right, enemy is to the right
    } else if (!m1.facingRight && enemyCenterX < playerCenterX) {
        isEnemyInFront = 1; // Player faces left, enemy is to the left
    }

    // 3. Only apply damage if in range AND in front
    if (m1.comboFrame == 10 && distanceToEnemy <= 200 && isEnemyInFront) {
        int damage = 0;
        m1.enemy.isHit = 1;
        m1.enemy.hitFrame = 0;
        m1.enemy.isAttacking = 0;

        switch (m1.comboStep) {
            case 1: damage = 5;  Mix_PlayChannel(-1, m1.hitSound, 0); break;
            case 2: damage = 5;  Mix_PlayChannel(-1, m1.hitSound, 0); break;
            case 3: damage = 10; break; 
        }

        m1.enemy.health -= damage;

        if (m1.enemy.health <= 0) {
            m1.enemy.health = 0;
            m1.enemy.alive = 0;
            m1.enemy.dying = 1;
            m1.enemy.deathFrame = 0;
        }
    }
}

    // -------- Bow Attack --------
    if (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT) && !m1.isPlayerHit && m1.alive) {
        if (!m1.isAttacking && m1.comboStep == 0) {
            m1.isAttacking = 1;
            m1.attackFrame = 0;
            m1.attackFinished = 0;
            m1.aimActive = 1;
            Mix_PlayChannelTimed(-1, m1.bowSound, 0, 1000);
        }
    } else if (m1.isAttacking && !m1.attackFinished) {
        m1.attackFinished = 1;
        m1.aimActive = 0;
    }
    
    if (m1.isPlayerHit && m1.isAttacking) {
        m1.isAttacking = 0;
        m1.aimActive = 0;
    }

    // -------- Movement --------
    m1.isMoving = 0;  
    if (m1.alive && !m1.isAttacking && m1.comboStep == 0 && !m1.isPlayerHit) {
int keyboardLeft  = keystate[SDL_SCANCODE_A];
int keyboardRight = keystate[SDL_SCANCODE_D];

int moveLeft  = keyboardLeft  || (btnLeft  == 0);
int moveRight = keyboardRight || (btnRight == 0);

        if (moveLeft && !moveRight) {
            m1.playerRect.x -= 10;
            m1.isMoving = 1;
            m1.facingRight = 0;
        }
        else if (moveRight && !moveLeft) {
            m1.playerRect.x += 10;
            m1.isMoving = 1;
            m1.facingRight = 1;
        }
    }
    if (m1.alive && m1.isMoving && !m1.isPlayerHit) {
        if (m1.playerWalkChannel == -1 || !Mix_Playing(m1.playerWalkChannel)) {
            m1.playerWalkChannel = Mix_PlayChannel(-1, m1.grassSfx, -1); 
        }
    } else {
        if (m1.playerWalkChannel != -1) {
            Mix_HaltChannel(m1.playerWalkChannel);
            m1.playerWalkChannel = -1;
        }
    }

    if (m1.playerRect.x < 0) m1.playerRect.x = 0;
    if (m1.playerRect.x > 1880 - m1.playerRect.w) m1.playerRect.x = 1880 - m1.playerRect.w;

    // -------- Bow Fire --------
    if (m1.aimActive && !m1.isPlayerHit && m1.alive && ((mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) || keystate[SDL_SCANCODE_K])) {
       Mix_HaltChannel(-1);
       Mix_PlayChannel(-1, m1.arrowSound, 0);
        SDL_Rect aimRect = { m1.mouseX - 16, m1.mouseY - 16, 32, 32 };
        SDL_Rect enemyRect = m1.enemy.rect;

        if (SDL_HasIntersection(&aimRect, &enemyRect) && m1.enemy.alive) {
            m1.enemy.health = 0;
            m1.enemy.isHit = 1;
            m1.enemy.hitFrame = 0;
            m1.enemy.isAttacking = 0;

            if (m1.enemy.health <= 0) {
                m1.enemy.alive = 0;
                m1.enemy.dying = 1;
                m1.enemy.deathFrame = 0;
            }
        }
        m1.aimActive = 0;
        m1.attackFinished = 1;
    }

    // -------- Player Hit Logic --------
    if (m1.isPlayerHit) {
        m1.playerHitFrame += 1.5;
        if (m1.playerHitFrame >= 36) {
            m1.playerHitFrame = 0;
            m1.isPlayerHit = 0;
        }
    }

    // -------- Player animation tick --------
    m1.tick++;
    if (m1.tick >= 0) {
        if (m1.alive && !m1.isPlayerHit && m1.comboStep == 0 && !m1.isAttacking) {
            if (m1.isMoving) m1.frame = (m1.frame + 1) % WALK_FRAMES;
            else m1.frame = (m1.frame + 1) % IDLE_FRAMES;
        }
        else if (m1.isAttacking) {
            if (!m1.attackFinished) {
                if (m1.attackFrame < 25) m1.attackFrame++;
            } else {
                if (m1.attackFrame < BOW_FRAMES - 1) m1.attackFrame++;
                else m1.isAttacking = 0;
            }
        }
        m1.tick = 0;
    }

    // -------- Enemy Hit Animation --------
    if (m1.enemy.isHit) {
        m1.enemy.hitFrame += 1.5;
        if (m1.enemy.hitFrame >= 36) {
            m1.enemy.hitFrame = 0;
            m1.enemy.isHit = 0;
        }
    }

// -------- Enemy AI --------
    if (m1.enemy.alive) {
        int distance = m1.playerRect.x - m1.enemy.rect.x;
        int absDistance = abs(distance);
        int speed = 0;

        // Reset moving flag each frame; AI will set it to 1 if it actually moves
        m1.enemy.moving = 0;

        if (m1.enemy.attackCooldown > 0) m1.enemy.attackCooldown--;

        // 1. Attack Trigger Logic
        if (absDistance <= m1.enemy.stopDistance && !m1.enemy.isHit) {
            if (!m1.enemy.isAttacking && m1.enemy.attackCooldown == 0) {
                m1.enemy.isAttacking = 1;
                m1.enemy.attackFrame = 0;
            }
        }

        // 2. Attack Animation & Damage Logic
        if (m1.enemy.isAttacking) {
            m1.enemy.attackFrame += 2;

            // Damage Frame (Frame 10)
            if (m1.enemy.attackFrame == 10) {
                int playerDistance = abs((m1.enemy.rect.x + m1.enemy.rect.w / 2) - 
                                         (m1.playerRect.x + m1.playerRect.w / 2));

                if (playerDistance <= 200 && m1.alive) {
                    Mix_PlayChannel(-1, m1.hitSound, 0);
                    m1.isPlayerHit = 1;     
                    m1.playerHitFrame = 0; 
                    
                    m1.health -= 3; 
                    if (m1.health <= 0) {
                        m1.health = 0;
                        m1.alive = 0;
                        m1.dying = 1;
                        m1.deathFrame = 0;
                    }
                    m1.comboStep = 0;
                    m1.isAttacking = 0; // Interrupt bow/melee on hit
                }
            }

            if (m1.enemy.attackFrame >= 36) {
                m1.enemy.isAttacking = 0;
                m1.enemy.attackCooldown = 60;
            }
        }

        // 3. Movement Logic (Chase or Patrol)
        if (!m1.enemy.isAttacking && !m1.enemy.isHit) {
            if (absDistance < m1.enemy.detectionRange) {
                // Chase Player
                m1.enemy.facingRight = (distance > 0) ? 1 : 0;
                if (absDistance > m1.enemy.stopDistance) {
                    speed = 10;
                    m1.enemy.rect.x += (distance > 0) ? speed : -speed;
                    m1.enemy.moving = 1;
                }
            } 
            else {
                // Patrol Mode
                speed = 6;
                if (m1.enemy.facingRight) {
                    if (m1.enemy.rect.x < m1.enemy.maxX) { 
                        m1.enemy.rect.x += speed; 
                        m1.enemy.moving = 1; 
                    } else {
                        m1.enemy.facingRight = 0;
                    }
                } else {
                    if (m1.enemy.rect.x > m1.enemy.minX) { 
                        m1.enemy.rect.x -= speed; 
                        m1.enemy.moving = 1; 
                    } else {
                        m1.enemy.facingRight = 1;
                    }
                }
            }
        }

        // 4. Enemy Walking Sound Logic (NEW)
        if (m1.enemy.moving && !m1.enemy.isHit && !m1.enemy.isAttacking) {
            if (m1.enemyWalkChannel == -1 || !Mix_Playing(m1.enemyWalkChannel)) {
                m1.enemyWalkChannel = Mix_PlayChannel(-1, m1.grassSfx, -1); 
            }
        } else {
            if (m1.enemyWalkChannel != -1) {
                Mix_HaltChannel(m1.enemyWalkChannel);
                m1.enemyWalkChannel = -1;
            }
        }

        // 5. Animation Ticks
        m1.enemy.tick++;
        if (m1.enemy.tick >= 0) {
            m1.enemy.tick = 0;
            if (!m1.enemy.isAttacking && !m1.enemy.isHit) {
                if (m1.enemy.moving) m1.enemy.frame = (m1.enemy.frame + 1) % WALK_FRAMES;
                else m1.enemy.frame = (m1.enemy.frame + 1) % IDLE_FRAMES;
            }
        }
    } 
    else if (m1.enemy.dying) {
        // Stop walking sound if dying
        if (m1.enemyWalkChannel != -1) {
            Mix_HaltChannel(m1.enemyWalkChannel);
            m1.enemyWalkChannel = -1;
        }
        if (m1.enemy.deathFrame < WALK_FRAMES - 1) m1.enemy.deathFrame++;
    }
}
void render_mission1(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (m1.background) SDL_RenderCopy(renderer, m1.background, NULL, NULL);

    SDL_RendererFlip playerFlip = m1.facingRight ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RendererFlip enemyFlip  = m1.enemy.facingRight ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    

    // -------- Enemy --------
    if (m1.enemy.alive || m1.enemy.dying) {
        SDL_Texture *enemyTex = NULL;
        if (m1.enemy.dying) enemyTex = m1.death[m1.enemy.deathFrame];
        else if (m1.enemy.isHit) enemyTex = m1.enemy.hit[(int)m1.enemy.hitFrame];
        else if (m1.enemy.isAttacking) enemyTex = m1.attack1[m1.enemy.attackFrame];
        else {
            if (m1.enemy.moving) enemyTex = m1.walk[m1.enemy.frame % WALK_FRAMES];
            else enemyTex = m1.idle[m1.enemy.frame % IDLE_FRAMES];
        }

        if (enemyTex) {
            int w, h;
            SDL_QueryTexture(enemyTex, NULL, NULL, &w, &h);
            SDL_Rect enemyDst;
            float scale = 1.5f;

            if (m1.enemy.dying) {
                enemyDst.w = (int)(w / 1.145f + 10.0f);
                enemyDst.h = (int)(h / 1.05f - 10.5f);
            } else if (m1.enemy.isHit) {
                enemyDst.w = (int)(w / scale);
                enemyDst.h = (int)(h / scale - 5.5f);
            } else if (m1.enemy.isAttacking) {
                enemyDst.w = (int)(w / 1.37);
                enemyDst.h = (int)(h / 1.37);
            } else {
                enemyDst.w = (int)(w / scale);
                enemyDst.h = (int)(h / scale);
            }
            enemyDst.x = m1.enemy.rect.x + (m1.enemy.rect.w - enemyDst.w) / 2;
            enemyDst.y = m1.enemy.rect.y + (m1.enemy.rect.h - enemyDst.h);
            SDL_RenderCopyEx(renderer, enemyTex, NULL, &enemyDst, 0, NULL, enemyFlip);
        }
    }

    // ----------------------
    // -------- Player --------
    // ----------------------
    SDL_Texture *currentTexture = NULL;
    double scaleX = 1.5; // Default scale for Idle/Walk
    double scaleY = 1.5;
    int yOffset = 0;

    // --- Animation State Priority ---
    if (m1.dying) {
        currentTexture = m1.death[(int)m1.deathFrame];
        scaleX = 1.11; 
        scaleY = 1.06; 
        yOffset = 10; 
    }
    else if (m1.isPlayerHit) {
        currentTexture = m1.playerHit[(int)m1.playerHitFrame];
        scaleX = 1.5; scaleY = 1.54;
    }
    else if (m1.comboStep == 1) {
        currentTexture = m1.attack1[m1.comboFrame];
        scaleX = 1.37; scaleY = 1.37;
    }
    else if (m1.comboStep == 2) {
        currentTexture = m1.attack2[m1.comboFrame];
        scaleX = 1.37; scaleY = 1.37;
    }
    else if (m1.comboStep == 3) {
        currentTexture = m1.attack3[m1.comboFrame];
        scaleX = 1.15; scaleY = 1.16; 
        yOffset = 90; 
    }
    else if (m1.isAttacking) {
        currentTexture = m1.bow[m1.attackFrame];
        scaleX = 1.517; scaleY = 1.39; 
        yOffset = 37;
    }
    else if (m1.isMoving) {
        currentTexture = m1.walk[m1.frame];
        scaleX = 1.5; scaleY = 1.5;
    }
    else {
        currentTexture = m1.idle[m1.frame];
        scaleX = 1.5; scaleY = 1.5;
    }

    if (m1.comboStep == 3 && !m1.combo3Played) {
        Mix_PlayChannel(-1, m1.combo3, 0);
        m1.combo3Played = 1;
    }
    if (m1.comboStep != 3) m1.combo3Played = 0;

    if (currentTexture) {
        int w, h;
        SDL_QueryTexture(currentTexture, NULL, NULL, &w, &h);
        SDL_Rect dst;
        dst.w = (int)(w / scaleX);
        dst.h = (int)(h / scaleY);
        if (m1.isMoving && !m1.dying) dst.h -= 6;
        dst.x = m1.playerRect.x + (m1.playerRect.w - dst.w) / 2;
        dst.y = m1.playerRect.y + (m1.playerRect.h - dst.h) + yOffset;
        SDL_RenderCopyEx(renderer, currentTexture, NULL, &dst, 0, NULL, playerFlip);
    }

    if (m1.aimActive && m1.aimTarget && m1.alive) {
        SDL_Rect aimDst = { m1.mouseX - 16, m1.mouseY - 16, 32, 32 };
        SDL_RenderCopy(renderer, m1.aimTarget, NULL, &aimDst);
    }

    // --- HUD Health Bars (Floating above heads) ---
    if (m1.healthBarTexture) {
        // 1. Player Health Bar
        if (m1.alive || m1.dying) {
            // Width: 250 (bigger), Height: 30
            int pBarWidth = (int)((m1.health / 100.0) * 250); 
            SDL_Rect pHealthRect = { 
                m1.playerRect.x + (m1.playerRect.w / 2) - 2, // Center it over player
                m1.playerRect.y - 40,                         // 40 pixels above head
                pBarWidth, 
                30                                            // Taller bar
            };
            SDL_RenderCopy(renderer, m1.healthBarTexture, NULL, &pHealthRect);
        }

        // 2. Enemy Health Bar
        if (m1.enemy.alive || m1.enemy.dying) {
            // Width: 250, Height: 30
            int eBarWidth = (int)((m1.enemy.health / 100.0) * 250);
            SDL_Rect eHealthRect = { 
                m1.enemy.rect.x + (m1.enemy.rect.w / 2) - 2, // Center it over enemy
                m1.enemy.rect.y - 40,                          // 40 pixels above head
                eBarWidth, 
                30 
            };
            SDL_RenderCopy(renderer, m1.healthBarTexture, NULL, &eHealthRect);
        }
    }
    //---------map------------
    render_minimap(renderer, &m1.map, m1.playerRect, m1.facingRight, m1.enemy.rect, m1.enemy.alive);

    SDL_RenderPresent(renderer);
}
void cleanup_mission1(void) {
    // -------- Textures --------
    if (m1.map.background) SDL_DestroyTexture(m1.map.background);
    if (m1.map.icon) SDL_DestroyTexture(m1.map.icon);
    if (m1.map.enemyIcon) SDL_DestroyTexture(m1.map.enemyIcon);
    if (m1.background) SDL_DestroyTexture(m1.background);

    for (int i = 0; i < IDLE_FRAMES; i++)
        if (m1.idle[i]) SDL_DestroyTexture(m1.idle[i]);

    for (int i = 0; i < WALK_FRAMES; i++)
        if (m1.walk[i]) SDL_DestroyTexture(m1.walk[i]);

    for (int i = 0; i < BOW_FRAMES; i++)
        if (m1.bow[i]) SDL_DestroyTexture(m1.bow[i]);

    for (int i = 0; i < WALK_FRAMES; i++)
        if (m1.death[i]) SDL_DestroyTexture(m1.death[i]);

    for (int i = 0; i < 36; i++) {
        if (m1.attack1[i]) SDL_DestroyTexture(m1.attack1[i]);
        if (m1.attack2[i]) SDL_DestroyTexture(m1.attack2[i]);
        if (m1.attack3[i]) SDL_DestroyTexture(m1.attack3[i]);
        if (m1.playerHit[i]) SDL_DestroyTexture(m1.playerHit[i]);
        if (m1.enemy.hit[i]) SDL_DestroyTexture(m1.enemy.hit[i]);
    }

    if (m1.aimTarget) SDL_DestroyTexture(m1.aimTarget);
    if (m1.healthBarTexture) SDL_DestroyTexture(m1.healthBarTexture);

    // -------- SOUND EFFECTS (NEW) --------
    if (m1.grassSfx) Mix_FreeChunk(m1.grassSfx);
    if (m1.hitSound) Mix_FreeChunk(m1.hitSound);
    if (m1.bowSound) Mix_FreeChunk(m1.bowSound);
    if (m1.combo3) Mix_FreeChunk(m1.combo3);
    if (m1.arrowSound) Mix_FreeChunk(m1.arrowSound);

    // Stop any playing channels (safe cleanup)
    Mix_HaltChannel(-1);

    // -------- SERIAL --------
    if (serial_fd != -1) {
        close(serial_fd);
        serial_fd = -1;
    }
}
