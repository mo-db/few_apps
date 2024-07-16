#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_messagebox.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_stdinc.h>

static SDL_Window *window = NULL;
static SDL_Surface *screen_surface;

// only when using gcc or clang set the noreturn attribute > indicate function will exit()
#if defined(__GNUC__) || defined(__CLANG__)
static void panic_and_abort(const char *title, const char *text) __attribute__((noreturn));
#endif

static void panic_and_abort(const char *title, const char *text)
{/*{{{*/
	fprintf(stderr, "PANIC: %s ... %s\n", title, text);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, text, window); // if window exists attach to window
	SDL_Quit(); // It's allways save to call, even if not initialised before!
	exit(1);
}/*}}}*/

int main(int argc, char **argv)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) == -1) {
		panic_and_abort("SDL_Init error!", SDL_GetError());
	}

	Uint64 width = 3840;
	Uint64 height = 2160;
	
	// Video stuff
	window = SDL_CreateWindow("Hello SDL!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
	if (!window) {
		panic_and_abort("SDL_CreateWindow failed!", SDL_GetError());
	}

	screen_surface = SDL_GetWindowSurface(window);
	if (!screen_surface) {
		panic_and_abort("SurfaceGet failed!", SDL_GetError());
	}
	printf("screen_pixelformat: %s\n", SDL_GetPixelFormatName(screen_surface->format->format));

	Uint32 *pixels_sur = (Uint32*)screen_surface->pixels;
	for (int i = 0; i < (width*height); i++) {
		pixels_sur[i] = 0xFF0000FF;
	}

	Uint64 start = 0;
	Uint64 end = 0;
	Uint64 delta_ns = 0;
	Uint64 test = 0;

	int keep_going = 1;
	SDL_Event event;
	while (keep_going) {
		/* start = SDL_GetPerformanceCounter(); */
		while (SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_QUIT:
					keep_going = 0;
					break;
			}
		}

		//SDL_RenderClear(renderer);
		if (test < (screen_surface->w * screen_surface->h)) {
			test++;
		} else {
			test = 0;
		}

		for (int i = 0; i < screen_surface->w * screen_surface->h; i++) {
			pixels_sur[i] = 0xFF00FF00;
		}

		for (int i = 0; i < 10; i++) {
			pixels_sur[test+i] = 0xFF0000FF;
		}


		/* SDL_UpdateWindowSurface(window); */
		if (test%100 == 2) {
			start = SDL_GetPerformanceCounter();
			SDL_UpdateWindowSurface(window);
			end = SDL_GetPerformanceCounter();
			delta_ns = (1000000000*(end - start)) / SDL_GetPerformanceFrequency();
			printf("freq:		%llu\n", SDL_GetPerformanceFrequency());
			printf("delta_ns: 	%llu\n", delta_ns);
		}

	}

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
