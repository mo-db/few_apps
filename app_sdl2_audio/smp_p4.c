#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_messagebox.h>

// FIXME:no global vars!
static SDL_AudioDeviceID audio_device = 0;
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
// FIXME:no global vars!
static SDL_AudioSpec desired;

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
static Uint8 *wavbuf = NULL;
static Uint32 wavlen = 0;
static Uint32 wavpos = 0;
static SDL_AudioSpec wavspec; // static variables in C are always initialized to 0
static SDL_AudioStream *stream;

static int open_new_audio_file(const char *file_name) 
{
	SDL_FreeAudioStream(stream);
	stream = NULL;
	SDL_FreeWAV(wavbuf);
	wavbuf = NULL;
	wavlen = 0;
	if (SDL_LoadWAV(file_name, &wavspec, &wavbuf, &wavlen) == NULL) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "coudln't load wav!", SDL_GetError(), window);
		return 0;
	}
	stream = SDL_NewAudioStream(wavspec.format, wavspec.channels, wavspec.freq, AUDIO_F32, 2, 48000);
	if (!stream) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "coudln't create new audio stream!", SDL_GetError(), window);
		SDL_FreeWAV(wavbuf);
		wavbuf = NULL;
		wavlen = 0;
		return 0;
	}
	if (SDL_AudioStreamPut(stream, wavbuf, wavlen) != 0) {
		panic_and_abort("couldn't put data to audio stream", SDL_GetError());
	}
	SDL_AudioStreamFlush(stream);
	return 1;
}

int main(int argc, char **argv)
{
	open_new_audio_file("CantinaBand60.wav");

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


	SDL_zero(desired);
	desired.format = AUDIO_F32;
	desired.channels = 2;
	desired.freq = 48000;
	desired.samples = 4096;
	desired.callback = NULL;

	audio_device = SDL_OpenAudioDevice(NULL, 0, &desired, NULL, 0);
	if (audio_device == 0) {
		panic_and_abort("couldn't open audio device!", SDL_GetError());
	}

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
						// FIXME: change!
						SDL_ClearQueuedAudio(audio_device);
						SDL_AudioStreamClear(stream);
						if (SDL_AudioStreamPut(stream, wavbuf, wavlen) == -1) {
							panic_and_abort("couldnt put audio data to stream", SDL_GetError());
						}
						wavpos = 0;
						rect1_color = 25;
					} else if (SDL_PointInRect(&pt, &rect_pause)) {
						// FIXME: change!
						pause = pause ? 0 : 1;
						SDL_PauseAudioDevice(audio_device, pause);
						rect1_color = 133;
					}
					break;
				}
				case SDL_DROPFILE: {
					open_new_audio_file(event.drop.file);
					SDL_free(event.drop.file);
					break;
				}
			}
		}
		// audio
		// ? push all data into stream and then queue the audio to the device
		// ? all that only to not have to track wavpos
		// ok -> so you send it through the stream to convert the audio data
		if (SDL_GetQueuedAudioSize(audio_device) < 8192) {
			int bytes_remaining = SDL_AudioStreamAvailable(stream); 
			if (bytes_remaining > 0 ) {
				const Uint32 new_bytes = SDL_min(bytes_remaining, 32 * 1024);
				static Uint8 converted_buffer [32 * 1024]; // NOTE: ?
				SDL_AudioStreamGet(stream, converted_buffer, new_bytes); // draw bytes from stream into new buffer
				SDL_QueueAudio(audio_device, converted_buffer, new_bytes);
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
