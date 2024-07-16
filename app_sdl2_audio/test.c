#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_messagebox.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_stdinc.h>

static SDL_Window *window = NULL;
static SDL_Surface *screen_surface;
static SDL_Renderer *renderer = NULL;

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

	Uint32 width = 640;
	Uint32 height = 480;
	
	// Video stuff
	window = SDL_CreateWindow("Hello SDL!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
	if (!window) {
		panic_and_abort("SDL_CreateWindow failed!", SDL_GetError());
	}

	screen_surface = SDL_GetWindowSurface(window);
	if (!screen_surface) {
		panic_and_abort("SurfaceGet failed!", SDL_GetError());
	}
	printf("screen_pixelformat: %s\n", SDL_GetPixelFormatName((Uint32)screen_surface->format));

	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		panic_and_abort("SDL_CreateRenderer failed!", SDL_GetError());
	}

	Uint32 *pixels_sur = (Uint32*)screen_surface->pixels;
	for (int i = 0; i < (width*height); i++) {
		pixels_sur[i] = 0xFF0000FF;
	}

	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, width, height);
	Uint32 pixels[width*height];
	Uint32 start = 0;
	Uint32 end = 0;
	Uint32 delta_ns = 0;
	Uint32 test = 0;

	int keep_going = 1;
	SDL_Event event;
	while (keep_going) {
		start = SDL_GetPerformanceCounter();
		while (SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_QUIT:
					keep_going = 0;
					break;
			}
		}

		//SDL_RenderClear(renderer);
		if (test < (width * height)) {
			test++;
		} else {
			test = 0;
		}

		for (int i = 0; i < width*height; i++) {
			pixels[i] = 0x00FFFFFF;
			pixels_sur[i] = 0x00FFFFFF;
		}

		for (int i = 0; i < 10 && i < ((width*height)-10); i++) {
			pixels[test+i] = 0xFF0000FF;
			pixels_sur[test+i] = 0xFF0000FF;
		}

		SDL_UpdateWindowSurface(window);
		/* SDL_UpdateTexture(texture, NULL, pixels, width * sizeof(Uint32)); */
		/* SDL_RenderCopy(renderer, texture, NULL, NULL); */
		/* SDL_RenderPresent(renderer); */
		end = SDL_GetPerformanceCounter();
		delta_ns = (1000000000*(end - start)) / SDL_GetPerformanceFrequency();
		printf("delta_ns: %ul\n", delta_ns);
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	return 0;
}
