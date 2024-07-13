#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define TARGET_FPS 120
#define FIXED_TIMESTEP (1.0 / 120.0)  // Increased update rate

typedef struct {
    float x, y;
    float vx, vy;  // Added velocity
} GameObject;

GameObject current_state = {400, 300, 200, 0};  // Added initial velocity
GameObject previous_state = {400, 300, 200, 0};

void update(float dt) {
    current_state.x += current_state.vx * dt;
    current_state.y += current_state.vy * dt;
    
    if (current_state.x > WINDOW_WIDTH) {
        current_state.x = 0;
    } else if (current_state.x < 0) {
        current_state.x = WINDOW_WIDTH;
    }
    
    if (current_state.y > WINDOW_HEIGHT) {
        current_state.y = 0;
    } else if (current_state.y < 0) {
        current_state.y = WINDOW_HEIGHT;
    }
}

GameObject interpolate(GameObject prev, GameObject curr, float alpha) {
    GameObject result;
    result.x = prev.x + (curr.x - prev.x) * alpha;
    result.y = prev.y + (curr.y - prev.y) * alpha;
    result.vx = prev.vx + (curr.vx - prev.vx) * alpha;
    result.vy = prev.vy + (curr.vy - prev.vy) * alpha;
    return result;
}

void render(SDL_Renderer* renderer, GameObject state) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect rect = {(int)state.x - 25, (int)state.y - 25, 50, 50};
    SDL_RenderFillRect(renderer, &rect);

    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Smooth Method 8 Implementation", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    double deltaTime = 0;
    double accumulator = 0;

    bool running = true;
    SDL_Event e;

    while (running) {
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = (double)((NOW - LAST) / (double)SDL_GetPerformanceFrequency());
        
        // Cap delta time to prevent spiral of death
        if (deltaTime > 0.25) deltaTime = 0.25;
        
        accumulator += deltaTime;

        // Handle events
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
        }

        // Update game state
        while (accumulator >= FIXED_TIMESTEP) {
            previous_state = current_state;
            update(FIXED_TIMESTEP);
            accumulator -= FIXED_TIMESTEP;
        }

        // Interpolate and render
        float alpha = accumulator / FIXED_TIMESTEP;
        GameObject interpolated_state = interpolate(previous_state, current_state, alpha);
        render(renderer, interpolated_state);

        // Frame limiting is now handled by SDL_RENDERER_PRESENTVSYNC
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
