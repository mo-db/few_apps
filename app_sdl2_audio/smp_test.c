#include "SDL2/SDL.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

#if defined(__GNUC__) || defined(__clang__)
static void panic_and_abort(const char *title, const char *text) __attribute__((noreturn));
#endif

static void panic_and_abort(const char *title, const char *text)
{
    fprintf(stderr, "PANIC: %s ... %s\n", title, text);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, text, NULL);
    SDL_Quit();
    exit(1);
}

int main(int argc, char **argv)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1) {
        panic_and_abort("SDL_Init failed", SDL_GetError());
    }

    window = SDL_CreateWindow("Hello SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
    if (!window) {
        panic_and_abort("SDL_CreateWindow failed", SDL_GetError());
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        panic_and_abort("SDL_CreateRenderer failed", SDL_GetError());
    }

    SDL_bool keep_going = SDL_TRUE;
    while (keep_going) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    keep_going = SDL_FALSE;
                    break;
            }
        }

        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderPresent(renderer);

    }

    SDL_Quit();
    return 0;
}
