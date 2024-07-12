#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_messagebox.h>
#include <math.h>

#define PI2 6.28318530718

// Global variables
static SDL_AudioDeviceID audio_device = 0;
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_AudioSpec desired;
static float volume = 1.0f; // Global volume variable
static float frequency = 440.0f; // Global frequency variable
static const int num_voices = 9; // Number of unison voices

#if defined(__GNUC__) || defined(__CLANG__)
static void panic_and_abort(const char *title, const char *text) __attribute__((noreturn));
#endif

static void panic_and_abort(const char *title, const char *text)
{
	fprintf(stderr, "PANIC: %s ... %s\n", title, text);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, text, window); // if window exists attach to window
	SDL_Quit(); // It's always safe to call, even if not initialized before!
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
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "couldn't load wav!", SDL_GetError(), window);
		return 0;
	}
	stream = SDL_NewAudioStream(wavspec.format, wavspec.channels, wavspec.freq, AUDIO_F32, 2, 48000);
	if (!stream) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "couldn't create new audio stream!", SDL_GetError(), window);
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

// Function to update the volume
void update_volume()
{
	SDL_ClearQueuedAudio(audio_device);
	SDL_AudioStreamClear(stream);
	if (SDL_AudioStreamPut(stream, wavbuf, wavlen) == -1) {
		panic_and_abort("couldn't put audio data to stream", SDL_GetError());
	}
	SDL_AudioStreamFlush(stream);
}

// Function to generate and queue sine wave audio
void play_synth_audio()
{
	const int samples_per_second = 48000;
	const int duration_seconds = 2; // Duration of the synth sound
	const int num_samples = samples_per_second * duration_seconds;

	float *synth_buffer = (float *)malloc(num_samples * sizeof(float));
	float phase[num_voices] = {0.0f};
	float detune_factor[num_voices] = {-0.05f, -0.03f, -0.02f, -0.01f, 0.0f, 0.01f, 0.02f, 0.03f, 0.05f};

	for (int i = 0; i < num_samples; ++i) {
		float sample = 0.0f;
		for (int v = 0; v < num_voices; ++v) {
			sample += sinf(phase[v]);
			phase[v] += PI2 * (frequency + detune_factor[v] * frequency) / samples_per_second;
			if (phase[v] >= PI2) {
				phase[v] -= PI2;
			}
		}
		synth_buffer[i] = (volume / num_voices) * sample;
	}

	SDL_ClearQueuedAudio(audio_device);
	SDL_QueueAudio(audio_device, synth_buffer, num_samples * sizeof(float));
	SDL_PauseAudioDevice(audio_device, 0); // Start playing

	free(synth_buffer);
}
int main(int argc, char **argv)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1) {
		panic_and_abort("SDL_Init error!", SDL_GetError());
	}

	open_new_audio_file("CantinaBand60.wav");

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
	SDL_Rect rect_rewind = { 500, 50, 400, 100 };
	int rect2_color = 0;

	// Slider rectangle for volume control
	SDL_Rect slider_rect = { 50, 50, 200, 20 };
	SDL_Rect slider_handle = { 50, 45, 20, 30 }; // The handle that will be dragged

	// Slider rectangle for frequency control
	SDL_Rect freq_slider_rect = { 50, 100, 200, 20 };
	SDL_Rect freq_slider_handle = { 50, 95, 20, 30 }; // The handle that will be dragged

	int pause = 1;
	int keep_going = 1;
	while (keep_going) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					keep_going = 0;
					break;
				case SDL_MOUSEBUTTONDOWN: {
					const SDL_Point pt = { event.button.x, event.button.y };
					if (SDL_PointInRect(&pt, &rect_rewind)) {
						SDL_ClearQueuedAudio(audio_device);
						SDL_AudioStreamClear(stream);
						if (SDL_AudioStreamPut(stream, wavbuf, wavlen) == -1) {
							panic_and_abort("couldn't put audio data to stream", SDL_GetError());
						}
						wavpos = 0;
						rect1_color = 25;
					} else if (SDL_PointInRect(&pt, &rect_pause)) {
						pause = pause ? 0 : 1;
						SDL_PauseAudioDevice(audio_device, pause);
						rect1_color = 133;
					} else if (SDL_PointInRect(&pt, &slider_handle)) {
						// Handle the volume slider
						int x = event.button.x;
						if (x < slider_rect.x) {
							x = slider_rect.x;
						} else if (x > slider_rect.x + slider_rect.w - slider_handle.w) {
							x = slider_rect.x + slider_rect.w - slider_handle.w;
						}
						slider_handle.x = x;
						volume = (float)(x - slider_rect.x) / (slider_rect.w - slider_handle.w);
						update_volume();
					} else if (SDL_PointInRect(&pt, &freq_slider_handle)) {
						// Handle the frequency slider
						int x = event.button.x;
						if (x < freq_slider_rect.x) {
							x = freq_slider_rect.x;
						} else if (x > freq_slider_rect.x + freq_slider_rect.w - freq_slider_handle.w) {
							x = freq_slider_rect.x + freq_slider_rect.w - freq_slider_handle.w;
						}
						freq_slider_handle.x = x;
						frequency = 220.0f + (float)(x - freq_slider_rect.x) / (freq_slider_rect.w - freq_slider_handle.w) * 1760.0f; // Range from 220Hz to 1980Hz
					}
					break;
				}
				case SDL_KEYDOWN: {
					if (event.key.keysym.sym == SDLK_a) {
						play_synth_audio();
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
		if (SDL_GetQueuedAudioSize(audio_device) < 8192) {
			int bytes_remaining = SDL_AudioStreamAvailable(stream);
			if (bytes_remaining > 0) {
				const Uint32 new_bytes = SDL_min(bytes_remaining, 32 * 1024);
				static Uint8 converted_buffer[32 * 1024]; // NOTE: ?
				SDL_AudioStreamGet(stream, converted_buffer, new_bytes); // draw bytes from stream into new buffer

				// Apply volume control
				float *samples = (float *)converted_buffer;
				for (int i = 0; i < new_bytes / sizeof(float); i++) {
					samples[i] *= volume;
				}
				SDL_QueueAudio(audio_device, converted_buffer, new_bytes);
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, green, 0, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, rect1_color, 255, 255, 255);
		SDL_RenderFillRect(renderer, &rect_pause);
		SDL_RenderFillRect(renderer, &rect_rewind);

		// Draw the volume slider
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red slider background
		SDL_RenderFillRect(renderer, &slider_rect);
		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue slider handle
		SDL_RenderFillRect(renderer, &slider_handle);

		// Draw the frequency slider
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green slider background
		SDL_RenderFillRect(renderer, &freq_slider_rect);
		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue slider handle
		SDL_RenderFillRect(renderer, &freq_slider_handle);

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
