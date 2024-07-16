#include <SDL2/SDL.h>
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Surface* screenSurface = NULL;

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("Event Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    screenSurface = SDL_GetWindowSurface(window);

    int running = 1;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    break;
                case SDL_MOUSEMOTION:
                    printf("Mouse position: (%d, %d)\n", event.motion.x, event.motion.y);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    printf("Mouse button pressed at (%d, %d)\n", event.button.x, event.button.y);
                    break;
                // You can handle other events here
            }
        }

        // Clear screen to blue
        SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0, 0, 255));
        SDL_UpdateWindowSurface(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
