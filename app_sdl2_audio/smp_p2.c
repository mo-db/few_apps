#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_messagebox.h>

static SDL_AudioDeviceID audio_device = 0;
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

// only when using gcc or clang set the noreturn attribute > indicate function will exit()
#if defined(__GNUC__) || defined(__CLANG__)
static void panic_and_abort(const char *title, const char *text) __attribute__((noreturn));
#endif

static void panic_and_abort(const char *title, const char *text)
{
	fprintf(stderr, "PANIC: %s ... %s\n", title, text);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, text, window); // if window exists attach to window
	SDL_Quit(); // It's allways save to call, even if not initialised before!
	exit(1);
}

typedef struct myrect {
	int x, y, w, h;
} myrect;

int main(int argc, char **argv)
{
	Uint8 *wavbuf = NULL;
	Uint32 wavlen = 0;
	SDL_AudioSpec wavspec;
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
	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

	int green = 0;

	SDL_Rect rect_pause = { 50, 300, 300, 200 };
	int rect1_color = 0;
	SDL_Rect rect_rewind = { 500, 50, 400, 100};
	int rect2_color = 0;

	int pause = 1;

	int keep_going = 1;
	while (keep_going) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_QUIT:
					keep_going = 0;
					break;
				case SDL_MOUSEBUTTONDOWN: {
					const SDL_Point pt = { event.button.x, event.button.y };
					if (SDL_PointInRect(&pt, &rect_rewind)) {
						if (audio_device) {
							SDL_ClearQueuedAudio(audio_device);
							SDL_QueueAudio(audio_device, wavbuf, wavlen);
							rect1_color = 25;
						}
					} else if (SDL_PointInRect(&pt, &rect_pause)) {
						pause = pause ? 0 : 1;
						if (audio_device) {
							SDL_PauseAudioDevice(audio_device, pause);
						}
						rect1_color = 133;
					}
					break;
				}
				case SDL_DROPFILE: {
					if (audio_device) {
						SDL_CloseAudioDevice(audio_device); // this will also close queue
						audio_device = 0;
					}
					SDL_FreeWAV(wavbuf);
					wavbuf = NULL;
					wavlen = 0;
					if (SDL_LoadWAV(event.drop.file, &wavspec, &wavbuf, &wavlen) == NULL) {
						SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "coudln't load wav!", SDL_GetError(), window);
					}

					#if 0
					// FIXME: use later!
					SDL_AudioSpec desired;
					SDL_zero(desired);
					desired.format = AUDIO_F32;
					desired.channels = 2;
					desired.freq = 44100;
					desired.samples = 4096;
					desired.callback = NULL;
					//desired.userdata = &my_audio_callback_user_data;
					#endif

					audio_device = SDL_OpenAudioDevice(NULL, 0, &wavspec, NULL, 0);
					if (audio_device == 0) {
						SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Couldn't open audio_device!", SDL_GetError(), window);
						SDL_FreeWAV(wavbuf);
						wavbuf = NULL;
						wavlen = 0;
					} else {
						SDL_QueueAudio(audio_device, wavbuf, wavlen);
						SDL_PauseAudioDevice(audio_device, pause);
					}
					SDL_free(event.drop.file);
					break;
				}
			}
		}
		SDL_SetRenderDrawColor(renderer, 0, green, 0, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, rect1_color, 255, 255, 255);

		SDL_RenderFillRect(renderer, &rect_pause);
		SDL_RenderFillRect(renderer, &rect_rewind);
		SDL_RenderPresent(renderer);


		green = (green + 1) % 256;
	}

	SDL_FreeWAV(wavbuf); // no longer in SDL3
	SDL_CloseAudioDevice(audio_device);

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	return 0;
}
