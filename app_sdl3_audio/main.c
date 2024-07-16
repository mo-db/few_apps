#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#define PI2 6.28318530718

static struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
} init_sdl;

static struct {
	uint32_t accum;
	uint32_t tick;
	float fixed_dt;
} state;


static int generate_sine();

static void panic_and_abort(const char *title, const char *text)
{/*{{{*/
	fprintf(stderr, "PANIC: %s ... %s\n", title, text);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, text, NULL); // if window exists attach to window
	SDL_Quit(); // It's allways save to call, even if not initialised before!
	exit(1);
}/*}}}*/


int main(int argc, char **argv)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
		panic_and_abort("SDL_Init error!", SDL_GetError());
	}

	init_sdl.window = SDL_CreateWindow("Hello SDL!", 640, 480, SDL_WINDOW_METAL);
	if (!init_sdl.window) {
		panic_and_abort("SDL_CreateWindow failed!", SDL_GetError());
	}

	init_sdl.renderer = SDL_CreateRenderer(init_sdl.window, "renderer");
	if (!init_sdl.renderer) {
		panic_and_abort("SDL_CreateRenderer failed!", SDL_GetError());
	}

	uint8_t keep_going = 1;
	while (keep_going) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if ( event.type == SDL_EVENT_QUIT) {
				keep_going = 0;
			}
		}
		printf("test\n");
		SDL_RenderPresent(init_sdl.renderer);
	}

	SDL_DestroyWindow(init_sdl.window);
	SDL_DestroyRenderer(init_sdl.renderer);
	SDL_Quit();
	return 0;
}


static int generate_sine() {
	return 1;
}

