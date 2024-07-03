#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>

// only when using gcc or clang set the noreturn attribute > indicate function will exit()
#if defined(__GNUC__) || defined(__CLANG__)
static void panic_and_abort(const char *title, const char *text) __attribute__((noreturn));
#endif

static void panic_and_abort(const char *title, const char *text)
{
	fprintf(stderr, "PANIC: %s ... %s\n", title, text);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, text, NULL);
	SDL_Quit(); // It's allways save to call, even if not initialised before!
	exit(1);
}

static SDL_AudioDeviceID audio_device = 0;
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

typedef struct myrect {
	int x, y, w, h;
} myrect;

int main(int argc, char **argv)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1) {
		panic_and_abort("SDL_Init error!", SDL_GetError());
	}

	// Video stuff
	window = SDL_CreateWindow("Hello SDL!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
	if (!window) {
		panic_and_abort("SDL_CreateWindow failed!", SDL_GetError());
	}

	// SDL_RENDERER_PRESENTVSYNC -> will wait at present function to sync frames to display
	// so the loop doesn't run at max cpu speed
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) {
		panic_and_abort("SDL_CreateRenderer failed!", SDL_GetError());
	}

	int green = 0;

	SDL_Rect rect_01 = { 50, 100, 300, 200 };

	SDL_Event event;
	int keep_going = 1;
	while (keep_going) {
		while (SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_QUIT:
					keep_going = 0;
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.x > rect_01.x &&
						event.button.x < rect_01.w &&)

			}
		}
		SDL_SetRenderDrawColor(renderer, 0, green, 0, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_Rect rect;
		rect.x = 0;
		rect.y = 0;
		rect.w = green;
		rect.h = 100;

		myrect mr;
		mr.x = 200;
		mr.y = 100;
		mr.w = 200;
		mr.h = green;


		SDL_RenderFillRect(renderer, &mr);
		SDL_RenderFillRect(renderer, &rect);
		SDL_RenderPresent(renderer);


		green = (green + 1) % 256;
	}

	SDL_Quit();
	return 0;

#if 0

	SDL_AudioSpec wavspec;
	Uint8 *wavbuf = NULL;
	Uint32 wavlen = 0;
	if (SDL_LoadWAV("CantinaBand60.wav", &wavspec, &wavbuf, &wavlen) == NULL) {
		panic_and_abort("Couldn't load WAV file!", SDL_GetError());
	}

	// FIXME: use later!
    SDL_AudioSpec desired;
    SDL_zero(desired);
    desired.format = AUDIO_F32;
    desired.channels = 2;
    desired.freq = 44100;
    desired.samples = 4096;
    desired.callback = NULL;
    //desired.userdata = &my_audio_callback_user_data;

	audio_device = SDL_OpenAudioDevice(NULL, 0, &wavspec, NULL, 0);
	if (audio_device == 0) {
		panic_and_abort("Couldn't open audio_device!", SDL_GetError());
	}


	SDL_QueueAudio(audio_device, wavbuf, wavlen);
	SDL_FreeWAV(wavbuf); // no longer in SDL3
	SDL_PauseAudioDevice(audio_device, 0); // device start pause -> unpause like this
	
	//SDL_Delay(5000);

	SDL_CloseAudioDevice(audio_device);
	SDL_Quit();
	return 0;

#endif
}
