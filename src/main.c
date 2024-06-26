#include <stdio.h>
#include <stdint.h>  // uint8_t
#include <assert.h>  // assert
#include <stdbool.h>  // true
#include <stdlib.h>  // exit, srand, rand
#include <time.h>  // time



#define unreachable assert(false);exit(99);

#ifdef __APPLE__
#include <SDL.h>
#else
#include <SDL/SDL.h>
#endif


#define UP 1
#define RIGHT 2
#define DOWN 3
#define LEFT 4


// A ring buffer.
struct _RingBuf {
    void* firstp;
    void* lastp;
    size_t unitSize;
};
typedef struct _RingBuf RingBuf;


// Initialize a ring buffer.
void ringInit(RingBuf* ringp, size_t unitSize, size_t count) {
    ringp->unitSize = unitSize;
    size_t size = unitSize * count;
    ringp->firstp = malloc(size);
    assert(ringp->firstp != NULL);
    ringp->lastp = (uint8_t*)(ringp->firstp) + size - unitSize;
}


// Return the next slot in a ring buffer.
void* ringNext(RingBuf* ringp, void* currentp) {
    void* nextp = ((uint8_t*)currentp) + ringp->unitSize;
    assert(nextp > currentp);  // check for overflow.
    if (nextp > ringp->lastp) {
        nextp = ringp->firstp;
    }
    return nextp;
}


// Return the previous slot in a ring buffer.
void* ringPrev(RingBuf* ringp, void* currentp) {
    void* prevp = ((uint8_t*)currentp) - ringp->unitSize;
    assert(prevp < currentp);  // check for underflow.
    if (prevp < ringp->firstp) {
        prevp = ringp->lastp;
    }
    return prevp;
}


// Block du corp du serpent
struct _SnakeNode {
    uint8_t x;
    uint8_t y;
};
typedef struct _SnakeNode SnakeNode;


// Block de pomme.
struct _Food {
    uint8_t x;
    uint8_t y;
};
typedef struct _Food Food;

struct _Food2 {
    uint8_t x;
    uint8_t y;
};
typedef struct _Food2 Food2;

struct _Food3 {
    uint8_t x;
    uint8_t y;
};
typedef struct _Food3 Food3;


// The game state.
struct _State {
    uint8_t cellSize;
    uint8_t gridWidth;
    uint8_t gridHeight;

    bool crashed;
    uint8_t direction;

    RingBuf ring;
    SnakeNode* headp;
    SnakeNode* tailp;

    Food food;
    Food2 food2;
    Food3 food3;

    SDL_Surface* screenp;
    uint32_t framePeriod;
    uint32_t lastFrame;

    uint32_t bgColor;
    uint32_t snakeColor;
};
typedef struct _State State;


// Exit the game (terminate the process).
void quit(int status) {
    SDL_Quit();
    exit(status);
}


// Iterate to the next block of the snake body.
// Returns NULL when you run off the end.
SnakeNode* snakeNext(State* statep, SnakeNode* snakep) {
    if (snakep == statep->tailp) {
        return NULL;
    } else {
        return ringNext(&(statep->ring), snakep);
    }
}


// Does the snake head collide with the food block?
bool foodCollidesWithSnake(State* statep) {
    SnakeNode* cursorp = statep->headp;
    while (cursorp != NULL) {
        if (statep->food.x == cursorp->x && statep->food.y == cursorp->y) {
            return true;
        }
        cursorp = snakeNext(statep, cursorp);
    }
    return false;
}


// Respawn the food into a new location which does not collide with the snake.
void respawnFood(State* statep) {
    // TODO: this approach will become non-performant as the snake fills
    // the screen (more and more re-rolls will be required to find an open
    // block).
    while (true) {
        statep->food.x = rand() % statep->gridWidth;
        statep->food.y = rand() % statep->gridHeight;
        if (foodCollidesWithSnake(statep)) {
            continue;
        } else {
            break;
        }
    }
}

void respawnFood2(State* statep) {
    // TODO: this approach will become non-performant as the snake fills
    // the screen (more and more re-rolls will be required to find an open
    // block).
    while (true) {
        statep->food2.x = rand() % statep->gridWidth;
        statep->food2.y = rand() % statep->gridHeight;
        if (foodCollidesWithSnake(statep)) {
            continue;
        } else {
            break;
        }
    }
}

void respawnFood3(State* statep) {
    // TODO: this approach will become non-performant as the snake fills
    // the screen (more and more re-rolls will be required to find an open
    // block).
    while (true) {
        statep->food3.x = rand() % statep->gridWidth;
        statep->food3.y = rand() % statep->gridHeight;
        if (foodCollidesWithSnake(statep)) {
            continue;
        } else {
            break;
        }
    }
}


// Collision entre la tete du serpent et son corp
bool snakeCollidesWithSnake(State* statep) {
    if (statep->headp == statep->tailp) {
        return false;
    }
    SnakeNode* cursorp = snakeNext(statep, statep->headp);
    while (cursorp != NULL) {
        if (statep->headp->x == cursorp->x && statep->headp->y == cursorp->y) {
            return true;
        }
        cursorp = snakeNext(statep, cursorp);
    }
    return false;
}


// Would the snake be out of bounds after advancing the snake head?
bool wouldBeOutOfBounds(State* statep) {
    uint8_t direction = statep->direction;
    SnakeNode* headp = statep->headp;
    switch (direction) {
        case UP:
            if (headp->y == 0) {
                return true;
            }
            break;
        case DOWN:
            if (headp->y == statep->gridHeight - 1) {
                return true;
            }
            break;
        case LEFT:
            if (headp->x == 0) {
                return true;
            }
            break;
        case RIGHT:
            if (headp->x == statep->gridWidth - 1) {
                return true;
            }
            break;
        default:
            unreachable;
    }
    return false;
}


// Advance the snake by one block.
void moveSnake(State* statep) {
    if (wouldBeOutOfBounds(statep)) {
        statep->crashed = true;
    }
    if (statep->crashed) {
        return;
    }

    SnakeNode* newHeadp = ringPrev(&(statep->ring), statep->headp);
    newHeadp->x = statep->headp->x;
    newHeadp->y = statep->headp->y;
    if (statep->direction == UP) {
        newHeadp->y--;
    } else if (statep->direction == DOWN) {
        newHeadp->y++;
    } else if (statep->direction == LEFT) {
        newHeadp->x--;
    } else if (statep->direction == RIGHT) {
        newHeadp->x++;
    } else {
        unreachable;
    }
    statep->headp = newHeadp;

    bool didEat = false;
    int nbEat = 0;
    if (statep->headp->x == statep->food.x && statep->headp->y == statep->food.y) {
        didEat = true;
        respawnFood(statep);
        respawnFood2(statep);
        respawnFood3(statep);
    }
    if (statep->headp->x == statep->food2.x && statep->headp->y == statep->food2.y) {
        didEat = true;
        respawnFood(statep);
        respawnFood2(statep);
        respawnFood3(statep);
    }
    if (statep->headp->x == statep->food3.x && statep->headp->y == statep->food3.y) {
        didEat = true;
        respawnFood(statep);
        respawnFood2(statep);
        respawnFood3(statep);
    }
    if (didEat == false) {
        statep->tailp = ringPrev(&(statep->ring), statep->tailp);
    }

    if (snakeCollidesWithSnake(statep)) {
        statep->crashed = true;
    }
}


// Restart: start a new game.
void restart(State* statep) {
    statep->headp = statep->ring.firstp;
    statep->tailp = statep->headp;
    statep->headp->x = rand() % statep->gridWidth;
    statep->headp->y = rand() % statep->gridHeight;

    respawnFood(statep);

    statep->crashed = false;

    if (statep->headp->x > statep->gridWidth / 2) {
        statep->direction = LEFT;
    } else {
        statep->direction = RIGHT;
    }
}


// Deduce the direction, based on two snake blocks.
uint8_t getDirection(SnakeNode* s1p, SnakeNode* s2p) {
    if (s1p->x < s2p->x && s1p->y == s2p->y) {
        return RIGHT;
    } else if (s2p->x < s1p->x && s1p->y == s2p->y) {
        return LEFT;
    } else if (s1p->x == s2p->x && s1p->y < s2p->y) {
        return DOWN;
    } else if (s1p->x == s2p->x && s2p->y < s1p->y) {
        return UP;
    } else {
        unreachable;
    }
}


uint32_t randomColor(State* statep) {
    uint8_t r = rand() & 0xFF;
    uint8_t g = rand() & 0xFF;
    uint8_t b = rand() & 0xFF;
    return SDL_MapRGB(statep->screenp->format, r, g, b);
}


// Draw the snake.
void drawSnake(State* statep) {
    SnakeNode* cursorp = statep->headp;
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
    uint8_t cellSize = statep->cellSize;

    uint32_t color = statep->snakeColor;

    // draw two nodes at a time, so that the connecting link is also drawn.
    while (cursorp != statep->tailp) {
        if (statep->crashed) {
            color = randomColor(statep);
        }
        SnakeNode* snake2p = ringNext(&(statep->ring), cursorp);
        uint8_t direction = getDirection(cursorp, snake2p);
        // x, y
        switch (direction) {
            case RIGHT:
            case DOWN:
                x = cursorp->x * cellSize;
                y = cursorp->y * cellSize;
                break;
            case LEFT:
            case UP:
                x = snake2p->x * cellSize;
                y = snake2p->y * cellSize;
                break;
            default:
                unreachable;
        }
        // w, h
        switch (direction) {
            case RIGHT:
            case LEFT:
                w = cellSize * 2;
                h = cellSize;
                break;
            case UP:
            case DOWN:
                w = cellSize;
                h = cellSize * 2;
                break;
            default:
                unreachable;
        }
        // inset
        x += 1;
        y += 1;
        w -= 2;
        h -= 2;
        SDL_Rect rect = {x, y, w, h};
        SDL_FillRect(statep->screenp, &rect, color);

        cursorp = snake2p;
        continue;
    }

    // draw the last block.
    if (statep->crashed) {
        color = randomColor(statep);
    }
    x = cursorp->x * cellSize;
    y = cursorp->y * cellSize;
    w = cellSize;
    h = cellSize;
    // inset
    x += 1;
    y += 1;
    w -= 2;
    h -= 2;
    SDL_Rect rect = {x, y, w, h};
    SDL_FillRect(statep->screenp, &rect, color);
}


// Draw the food.
void drawFood(State* statep) {
    int16_t x = statep->food.x * statep->cellSize;
    int16_t y = statep->food.y * statep->cellSize;
    uint16_t w = statep->cellSize;
    uint16_t h = statep->cellSize;
    SDL_Rect rect = {x, y, w, h};
    uint32_t color = randomColor(statep);
    SDL_FillRect(statep->screenp, &rect, color);
}

void drawFood2(State* statep) {
    int16_t x = statep->food2.x * statep->cellSize;
    int16_t y = statep->food2.y * statep->cellSize;
    uint16_t w = statep->cellSize;
    uint16_t h = statep->cellSize;
    SDL_Rect rect = {x, y, w, h};
    uint32_t color = randomColor(statep);
    SDL_FillRect(statep->screenp, &rect, color);
}

void drawFood3(State* statep) {
    int16_t x = statep->food3.x * statep->cellSize;
    int16_t y = statep->food3.y * statep->cellSize;
    uint16_t w = statep->cellSize;
    uint16_t h = statep->cellSize;
    SDL_Rect rect = {x, y, w, h};
    uint32_t color = randomColor(statep);
    SDL_FillRect(statep->screenp, &rect, color);
}



// Draw the background.
void drawBG(State* statep) {
    int16_t x = 0;
    int16_t y = 0;
    uint16_t w = statep->gridWidth * statep->cellSize;
    uint16_t h = statep->gridHeight * statep->cellSize;
    SDL_Rect rect = {x, y, w, h};
    SDL_FillRect(statep->screenp, &rect, statep->bgColor);
}


// Perform all drawing.  Called once per frame.
void draw(State* statep) {
    drawBG(statep);
    drawSnake(statep);
    if (statep->crashed == false) {
        drawFood(statep);
        drawFood2(statep);
        drawFood3(statep);
    }
    SDL_Flip(statep->screenp);
}


// Update the game state.  Called once per frame.
void update(State* statep) {
    SDL_Event event;
    while (SDL_PollEvent(&event) == 1) {
        if (event.type == SDL_QUIT) {
            // the window was closed.
            quit(0);
        } else if (event.type == SDL_KEYDOWN) {
            SDLKey k = event.key.keysym.sym;
            // check if we need to quit.
            if (k == SDLK_ESCAPE || k == SDLK_q) {
                quit(0);
            }
            // if crashed, any key (other than quit) restarts.
            if (statep->crashed == true) {
                restart(statep);
                return;
            }
            // check if we need to change direction.
            // (but only process one direction change per frame).
            if (k == SDLK_UP && statep->direction != DOWN) {
                statep->direction = UP;
                break;
            } else if (k == SDLK_DOWN && statep->direction != UP) {
                statep->direction = DOWN;
                break;
            } else if (k == SDLK_LEFT && statep->direction != RIGHT) {
                statep->direction = LEFT;
                break;
            } else if (k == SDLK_RIGHT && statep->direction != LEFT) {
                statep->direction = RIGHT;
                break;
            }
        }
        continue;
    }
    if (!statep->crashed) {
        moveSnake(statep);
    }
}


// Perform all initialization.
void init(State* statep) {
    srand(time(NULL));

    int ret = SDL_Init(SDL_INIT_VIDEO);
    assert(ret == 0);


    statep->cellSize = 38;
    statep->gridWidth = 35;
    statep->gridHeight = 19;

    int width = statep->gridWidth * statep->cellSize;
    int height = statep->gridHeight * statep->cellSize;
    int bpp = 0;  // use current bits per pixel.
    uint32_t flags = SDL_HWSURFACE;
    statep->screenp = SDL_SetVideoMode(width, height, bpp, flags);
    assert(statep->screenp != NULL);

    uint8_t r = 0;
    uint8_t g = 255;
    uint8_t b = 0;
    statep->snakeColor = SDL_MapRGB(statep->screenp->format, r, g, b);

    statep->framePeriod = 200;  // in milliseconds
    statep->lastFrame = 0;

    size_t unitSize = sizeof(SnakeNode);
    size_t count = statep->gridWidth * statep->gridHeight;
    ringInit(&(statep->ring), unitSize, count);

    r = 255;
    g = 255;
    b = 255;
    statep->bgColor = SDL_MapRGB(statep->screenp->format, r, g, b);

    restart(statep);
}


// The process entry point.
int main(int argc, char** argv) {
    State state;
    init(&state);

    while (true) {
        uint32_t ticks = SDL_GetTicks();
        uint32_t elapsed = ticks - state.lastFrame;
        if (elapsed >= state.framePeriod) {
            update(&state);
            draw(&state);

            if (elapsed > state.framePeriod * 2) {
                // catch up.
                state.lastFrame = ticks;
            } else {
                state.lastFrame += state.framePeriod;
            }
        } else {
            uint32_t remaining = state.framePeriod - elapsed;
            SDL_Delay(remaining);
        }
        continue;
    }

    return 0;
}
